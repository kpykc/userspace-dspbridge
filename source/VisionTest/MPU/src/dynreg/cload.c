/*
 *  Copyright 2001-2008 Texas Instruments - http://www.ti.com/
 * 
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 * 
 *     http://www.apache.org/licenses/LICENSE-2.0
 * 
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */


#include "header.h"
#include <host_os.h>
#include "module_list.h"
#define LINKER_MODULES_HEADER ("_" MODULES_HEADER)

#ifdef NEGATIVE_TESTING
extern int test_permit_error;/* if true, API call made which may be ignored */
#endif

/*
 * we use the fact that DOFF section records are shaped just
 * like LDR_SECTION_INFO to * reduce our section storage usage.  
 * This macro marks the places where that assumption * is made
 */
#define DOFFSEC_IS_LDRSEC(pdoffsec) ((struct LDR_SECTION_INFO *)(pdoffsec))

/*
 * forward references
 */
static void dload_symbols(struct dload_state *dlthis);
static void dload_data(struct dload_state *dlthis);
static void allocate_sections(struct dload_state *dlthis);
static void string_table_free(struct dload_state *dlthis);
static void symbol_table_free(struct dload_state *dlthis);
static void section_table_free(struct dload_state *dlthis);
static void init_module_handle(struct dload_state *dlthis);
#if BITS_PER_AU > BITS_PER_BYTE
static char *unpack_name(struct dload_state *dlthis, uint32_t soffset);
#endif
#if LEAD3
static boolean chk_revision_mix(struct dload_state *dlthis, const char *sname);
static boolean chk_memmodel_mix(struct dload_state *dlthis, 
				const struct doff_syment_t *sym,
					const struct dynload_symbol *defSym, const char *sname);
#endif

static const char CINITNAME[] = { ".cinit" };
static const char LOADER_DLLVIEW_ROOT[] = { "?DLModules?" };

/*
 * Error strings
 */
static const char E_READSTRM[] = { "Error reading %s from input stream" };
static const char E_ALLOC[] = { "Syms->Allocate( %d ) failed" };
static const char E_TGTALLOC[] = { "Target memory allocate failed, section %s"
															"size "FMT_UI32};
/*static const char E_SEEK[]            = {"Set file position to %d failed"};*/
static const char E_INITFAIL[] = { "%s to target address " FMT_UI32 " failed" };
static const char E_DLVWRITE[] = { "Write to DLLview list failed" };
static const char E_ICONNECT[] = { "Connect call to init interface failed" };
static const char E_CHECKSUM[] = { "Checksum failed on %s" };

/*****************************************************************************
 * Procedure dload_error
 *
 * Parameters:
 *	errtxt	description of the error, printf style
 *	...		additional information
 *
 * Effect:
 *	Reports or records the error as appropriate.
 *****************************************************************************/
void dload_error(struct dload_state *dlthis, const char *errtxt, ...)
{
	va_list args;
	va_start(args, errtxt);
	dlthis->mysym->Error_Report(dlthis->mysym, errtxt, args);
	va_end(args);
	dlthis->dload_errcount += 1;

}				/* dload_error */

#define DL_ERROR(zza, zzb) dload_error(dlthis, zza, zzb)

/*****************************************************************************
 * Procedure dload_syms_error
 *
 * Parameters:
 *	errtxt	description of the error, printf style
 *	...		additional information
 *
 * Effect:
 *	Reports or records the error as appropriate.
 *****************************************************************************/
void dload_syms_error(struct Dynamic_Loader_Sym *syms, const char *errtxt, ...)
{
	va_list args;
	va_start(args, errtxt);
	syms->Error_Report(syms, errtxt, args);
	va_end(args);
}

/*****************************************************************************
 * Procedure Dynamic_Load_Module
 *
 * Parameters:
 *	module	The input stream that supplies the module image
 *	syms	Host-side symbol table and malloc/free functions
 *	alloc	Target-side memory allocation
 *	init	Target-side memory initialization
 *	options	Option flags DLOAD_*
 *	mhandle	A module handle for use with Dynamic_Unload
 *
 * Effect:
 *	The module image is read using *module.  Target storage for the new image is
 * obtained from *alloc.  Symbols defined and referenced by the module are
 * managed using *syms.  The image is then relocated and references resolved
 * as necessary, and the resulting executable bits are placed into target memory
 * using *init.
 *
 * Returns:
 *	On a successful load, a module handle is placed in *mhandle, and zero is
 * returned.  On error, the number of errors detected is returned.  Individual
 * errors are reported during the load process using syms->Error_Report().
 *****************************************************************************/
/* the source for the module image*/ /* host support for symbols and storage*/
/* the target memory allocator*/ /* the target memory initializer*/ 
/* option flags*/ /* the returned module handle*/
extern int Dynamic_Load_Module(struct Dynamic_Loader_Stream *module,
		struct Dynamic_Loader_Sym *syms,struct Dynamic_Loader_Allocate *alloc,
		struct Dynamic_Loader_Initialize *init,unsigned options,
			DLOAD_mhandle *mhandle)
{
	register unsigned *dp, sz;
	struct dload_state dl_state;	/* internal state for this call */

	/* blast our internal state */
	dp = (unsigned *)&dl_state;
	for (sz = sizeof(dl_state) / sizeof(unsigned);sz > 0;sz -= 1)
		*dp++ = 0;
	/* Enable _only_ BSS initialization if enabled by user */
	if ((options & DLOAD_INITBSS) == DLOAD_INITBSS) {
		dl_state.myoptions = DLOAD_INITBSS;
	}
	/* Check that mandatory arguments are present */
	if (!module || !syms) {
		dload_error(&dl_state, "Required parameter is NULL");
	} else {
		dl_state.strm = module;
		dl_state.mysym = syms;
		dload_headers(&dl_state);
		if (!dl_state.dload_errcount)
			dload_strings(&dl_state, false);
		if (!dl_state.dload_errcount)
			dload_sections(&dl_state);
		if (init && !dl_state.dload_errcount) {
			if (init->connect(init)) {
				dl_state.myio = init;
				dl_state.myalloc = alloc;
				allocate_sections(&dl_state);	
				/* do now, before reducing symbols */
			} else
				dload_error(&dl_state, E_ICONNECT);
		}
		if (!dl_state.dload_errcount) {
			/* fix up entry point address */
			unsigned sref = dl_state.dfile_hdr.df_entry_secn - 1;
			if (sref < dl_state.allocated_secn_count) 
				dl_state.dfile_hdr.df_entrypt += 
										dl_state.ldr_sections[sref].run_addr;
			dload_symbols(&dl_state);
		}
		if (init && !dl_state.dload_errcount)
			dload_data(&dl_state);
		init_module_handle(&dl_state);
		if (dl_state.myio) {
			if ((!dl_state.dload_errcount) &&
			   (dl_state.dfile_hdr.df_entry_secn != DN_UNDEF)) {
				if(init!=NULL) {
					if (!init->execute(init,
					   dl_state.dfile_hdr.df_entrypt))
						dload_error(&dl_state,
						   "Init->Execute Failed");
						init->release(init);
				} else
					dload_error(&dl_state, "init is NULL");
			}
			init->release(init);
		}
		symbol_table_free(&dl_state);
		section_table_free(&dl_state);
		string_table_free(&dl_state);
		if (dl_state.dload_errcount) {
			Dynamic_Unload_Module(dl_state.myhandle, syms, alloc,init);
			dl_state.myhandle = NULL;
		}
	}
	if (mhandle)
		*mhandle = dl_state.myhandle;	/* give back the handle */
	return dl_state.dload_errcount;
}				/* DLOAD_File */

#ifdef OPT_ELIMINATE_EXTRA_DLOAD
/*************************************************************************
 * Procedure Dynamic_Open_Module
 *
 * Parameters:
 *      module  The input stream that supplies the module image
 *      syms    Host-side symbol table and malloc/free functions
 *      alloc   Target-side memory allocation
 *      init    Target-side memory initialization
 *      options Option flags DLOAD_*
 *      mhandle A module handle for use with Dynamic_Unload
 *
 * Effect:
 *      The module image is read using *module.  Target storage for the new
        image is
 * obtained from *alloc.  Symbols defined and referenced by the module are
 * managed using *syms.  The image is then relocated and references
        resolved as necessary, and the resulting executable bits are placed
        into target memory using *init.
 *
 * Returns:
 *      On a successful load, a module handle is placed in *mhandle,
        and zero is returned.  On error, the number of errors detected is
        returned.  Individual errors are reported during the load process
        using syms->Error_Report().
 ***********************************************************************/
extern int
Dynamic_Open_Module(struct Dynamic_Loader_Stream *module, struct Dynamic_Loader_Sym *syms
                  ,struct Dynamic_Loader_Allocate *alloc,struct Dynamic_Loader_Initialize *init,
                  unsigned options, DLOAD_mhandle *mhandle)
{
        register unsigned *dp, sz;
        struct  dload_state dl_state;   /* internal state for this call */

        /* blast our internal state */
        dp = (unsigned *)&dl_state;
        for (sz = sizeof(dl_state) / sizeof(unsigned);sz > 0;sz -= 1)
                *dp++ = 0;

        /* Enable _only_ BSS initialization if enabled by user */
        if ((options & DLOAD_INITBSS) == DLOAD_INITBSS) {
                dl_state.myoptions = DLOAD_INITBSS;
        }

        /* Check that mandatory arguments are present */
        if (!module || !syms) {
                dload_error(&dl_state, "Required parameter is NULL");
        } else {
                dl_state.strm = module;
                dl_state.mysym = syms;
                dload_headers(&dl_state);
                if (!dl_state.dload_errcount)
                        dload_strings(&dl_state, false);
                if (!dl_state.dload_errcount)
                        dload_sections(&dl_state);

                if (init && !dl_state.dload_errcount) {
                        if (init->connect(init)) {
                                dl_state.myio = init;
                                dl_state.myalloc = alloc;
                                /* do now, before reducing symbols */
                                allocate_sections(&dl_state);
                        } else
                                dload_error(&dl_state, E_ICONNECT);
                }

                if (!dl_state.dload_errcount) {
                        /* fix up entry point address */
                        unsigned sref = dl_state.dfile_hdr.df_entry_secn - 1;
                        if (sref < dl_state.allocated_secn_count)
                                dl_state.dfile_hdr.df_entrypt +=
                                    dl_state.ldr_sections[sref].run_addr;

                        dload_symbols(&dl_state);
                }

                init_module_handle(&dl_state);

                if (dl_state.myio) {
                        if ((!dl_state.dload_errcount)
                            && (dl_state.dfile_hdr.df_entry_secn != DN_UNDEF))
                                if (!init->execute(init,dl_state.dfile_hdr.df_entrypt))
                                        dload_error(&dl_state, "Init->Execute Failed");
                        init->release(init);
                }

                symbol_table_free(&dl_state);
                section_table_free(&dl_state);
                string_table_free(&dl_state);

                if (dl_state.dload_errcount) {
                        Dynamic_Unload_Module(dl_state.myhandle, syms, alloc,init);
                        dl_state.myhandle = NULL;
                }
        }

        if (mhandle)
                *mhandle = dl_state.myhandle;   /* give back the handle */

        return dl_state.dload_errcount;
}                               /* DLOAD_File */
#endif


/*****************************************************************************
 * Procedure dload_headers
 *
 * Parameters:
 *	none
 *
 * Effect:
 *	Loads the DOFF header and verify record.  Deals with any byte-order
 * issues and checks them for validity.
 *****************************************************************************/
#define COMBINED_HEADER_SIZE (sizeof(struct doff_filehdr_t) + \
											sizeof(struct doff_verify_rec_t))

void dload_headers(struct dload_state *dlthis)
{
	uint_least32_t map;
	/* Read the header and the verify record as one.  
	 * If we don't get it all, we're done */
	if (dlthis->strm->read_buffer(dlthis->strm, &dlthis->dfile_hdr, 
								COMBINED_HEADER_SIZE) != COMBINED_HEADER_SIZE) {
		DL_ERROR(E_READSTRM, "File Headers");
		return;
	}
	/*
	 * Verify that we have the byte order of the file correct.
	 * If not, must fix it before we can continue
	 */
	map = REORDER_MAP(dlthis->dfile_hdr.df_byte_reshuffle);
	if (map != REORDER_MAP(BYTE_RESHUFFLE_VALUE)) {
		/* input is either byte-shuffled or bad */
		if ((map & 0xFCFCFCFC) == 0) {	/* no obviously bogus bits */
			dload_reorder(&dlthis->dfile_hdr, COMBINED_HEADER_SIZE, map);
		}
		if (dlthis->dfile_hdr.df_byte_reshuffle != BYTE_RESHUFFLE_VALUE) {
			/* didn't fix the problem, the byte swap map is bad */
			dload_error(dlthis, "Bad byte swap map " FMT_UI32 " in header",
										dlthis->dfile_hdr.df_byte_reshuffle);
			return;
		}
		dlthis->reorder_map = map;	/* keep map for future use */
	}
	/*
	 * Verify checksum of header and verify record
	 */
	if (~dload_checksum(&dlthis->dfile_hdr, sizeof(struct doff_filehdr_t)) ||
			~dload_checksum(&dlthis->verify,sizeof(struct doff_verify_rec_t))) {
		DL_ERROR(E_CHECKSUM, "header or verify record");
		return;
	}
#if HOST_ENDIANNESS
	dlthis->dfile_hdr.df_byte_reshuffle = map;	/* put back for later */
#endif
	/* Check for valid target ID */
	if ((dlthis->dfile_hdr.df_target_id != TARGET_ID) && 
								(dlthis->dfile_hdr.df_target_id != TMS470_ID)) {
		dload_error(dlthis,"Bad target ID 0x%x",dlthis->dfile_hdr.df_target_id);
		return;
	}
	/* Check for valid file format */
	if ((dlthis->dfile_hdr.df_doff_version != DOFF0)) {
		dload_error(dlthis,"Bad DOFF version 0x%x",
											dlthis->dfile_hdr.df_doff_version);
		return;
	}
	/*
	 * Apply reasonableness checks to count fields
	 */
	if (dlthis->dfile_hdr.df_strtab_size > MAX_REASONABLE_STRINGTAB) {
		dload_error(dlthis, "Excessive string table size " FMT_UI32,
											dlthis->dfile_hdr.df_strtab_size);
		return;
	}
	if (dlthis->dfile_hdr.df_no_scns > MAX_REASONABLE_SECTIONS) {
		dload_error(dlthis, "Excessive section count 0x%x",
												dlthis->dfile_hdr.df_no_scns);
		return;
	}

#ifndef TARGET_ENDIANNESS
	/*
	 * Check that endianness does not disagree with explicit specification
	 */
	if ((dlthis->dfile_hdr.df_flags >> ALIGN_COFF_ENDIANNESS) &
										dlthis->myoptions & ENDIANNESS_MASK) {
		dload_error(dlthis,"Input endianness disagrees with specified option");
		return;
	}
	dlthis->big_e_target = dlthis->dfile_hdr.df_flags & DF_BIG;
#endif

}				/* dload_headers */

/*
 *		COFF Section Processing
 *
 *	COFF sections are read in and retained intact.  Each record is embedded
 * in a new structure that records the updated load and run addresses of the
 * section
 */

static const char SECN_ERRID[] = { "section" };

/*****************************************************************************
 * Procedure dload_sections
 *
 * Parameters:
 *	none
 *
 * Effect:
 *	Loads the section records into an internal table.
 *****************************************************************************/
#ifdef PERFORMANCE_DATA
unsigned count_sections;
#endif
void dload_sections(struct dload_state *dlthis)
{
	int_least16_t siz;
	struct doff_scnhdr_t *shp;
	unsigned nsecs = dlthis->dfile_hdr.df_no_scns;
#ifdef PERFORMANCE_DATA
	count_sections = nsecs;
#endif
	/* allocate space for the DOFF section records */
	siz = nsecs * sizeof(struct doff_scnhdr_t);
	shp = (struct doff_scnhdr_t *)dlthis->mysym->Allocate(dlthis->mysym, siz);
	if (!shp) {		/* not enough storage */
		DL_ERROR(E_ALLOC, siz);
		return;
	}
	dlthis->sect_hdrs = shp;
	/* read in the section records */
	if (dlthis->strm->read_buffer(dlthis->strm, shp, siz) != siz) {
		DL_ERROR(E_READSTRM, SECN_ERRID);
		return;
	}
	/* if we need to fix up byte order, do it now */
	if (dlthis->reorder_map) {
		dload_reorder(shp, siz, dlthis->reorder_map);
	}
	/* check for validity */
	if (~dload_checksum(dlthis->sect_hdrs, siz) !=
										dlthis->verify.dv_scn_rec_checksum) {
		DL_ERROR(E_CHECKSUM, SECN_ERRID);
		return;
	}

}				/* dload_sections */

/*****************************************************************************
 * Procedure allocate_sections
 *
 * Parameters:
 *	alloc	target memory allocator class
 *
 * Effect:
 *	Assigns new (target) addresses for sections
 *****************************************************************************/
static void allocate_sections(struct dload_state *dlthis)
{
	uint_least16_t curr_sect, nsecs, siz;
	struct doff_scnhdr_t *shp;
	struct LDR_SECTION_INFO *asecs;
	struct my_handle *hndl;

	nsecs = dlthis->dfile_hdr.df_no_scns;
	if (!nsecs)
		return;
	if ((dlthis->myalloc == NULL) && (dlthis->dfile_hdr.df_target_scns > 0)) {
		DL_ERROR("Arg 3 (alloc) required but NULL", 0);
		return;
	}
	 /* allocate space for the module handle, which we
	 * will keep for unload purposes */
	siz = dlthis->dfile_hdr.df_target_scns * sizeof(struct LDR_SECTION_INFO) +
																MY_HANDLE_SIZE;
	hndl = (struct my_handle *)dlthis->mysym->Allocate(dlthis->mysym, siz);
	if (!hndl) {		/* not enough storage */
		DL_ERROR(E_ALLOC, siz);
		return;
	}
	/* initialize the handle header */
	hndl->dm.hnext = hndl->dm.hprev = hndl;	/* circular list */
	hndl->dm.hroot = 0;
	hndl->dm.dbthis = 0;
	dlthis->myhandle = hndl;	/* save away for return */
	dlthis->ldr_sections = asecs = hndl->secns;	
	/* pointer to the section list of allocated sections */

	/*
	 * Insert names into all sections, make copies of the sections we allocate
	 */
	shp = dlthis->sect_hdrs;
	for (curr_sect = 0;curr_sect < nsecs;curr_sect++) {
		uintptr_t soffset = shp->ds_offset;
#if BITS_PER_AU <= BITS_PER_BYTE
		/* attempt to insert the name of this section */
		if (soffset < dlthis->dfile_hdr.df_strtab_size)
			DOFFSEC_IS_LDRSEC(shp)->name = dlthis->str_head + soffset;
		else {
			dload_error(dlthis, "Bad name offset in section %d", curr_sect);
			DOFFSEC_IS_LDRSEC(shp)->name = NULL;
		}
#endif
		/* allocate target storage for sections that require it */
		if (DS_NEEDS_ALLOCATION(shp)) {
			*asecs = *DOFFSEC_IS_LDRSEC(shp);
			asecs->context = 0;	/* zero the context field */
#if BITS_PER_AU > BITS_PER_BYTE
			asecs->name = unpack_name(dlthis, soffset);
			dlthis->debug_string_size = soffset + dlthis->temp_len;
#else
			dlthis->debug_string_size = soffset;
#endif
			if (!dlthis->myalloc->Allocate(dlthis->myalloc, asecs,
												DS_ALIGNMENT(asecs->type))) {
				dload_error(dlthis, E_TGTALLOC, asecs->name, asecs->size);
				return;
			}
			/* keep address deltas in original section table */
			shp->ds_vaddr = asecs->load_addr - shp->ds_vaddr;
			shp->ds_paddr = asecs->run_addr - shp->ds_paddr;
			dlthis->allocated_secn_count += 1;
		}		/* allocate target storage */
		shp += 1;
		asecs += 1;
	}

#if BITS_PER_AU <= BITS_PER_BYTE
	dlthis->debug_string_size += strlen(dlthis->str_head + 
												dlthis->debug_string_size) + 1;
#endif

}				/* allocate sections */

/*****************************************************************************
 * Procedure section_table_free
 *
 * Parameters:
 *	none
 *
 * Effect:
 *	Frees any state used by the symbol table.
 *
 * WARNING:
 *	This routine is not allowed to declare errors!
 *****************************************************************************/
static void section_table_free(struct dload_state *dlthis)
{
	struct doff_scnhdr_t *shp;

	shp = dlthis->sect_hdrs;
	if (shp) {
		dlthis->mysym->Deallocate(dlthis->mysym, shp);
	}
}				/* section_table_free */

/*****************************************************************************
 * Procedure dload_strings
 *
 * Parameters:
 *  sec_names_only   If true only read in the "section names" portion of the
 *                   string table
 *
 * Effect:
 *	Loads the DOFF string table into memory. DOFF keeps all strings in a
 * big unsorted array.  We just read that array into memory in bulk.
 *****************************************************************************/
static const char S_STRINGTBL[] = { "string table" };

void dload_strings(struct dload_state *dlthis, boolean sec_names_only)
{
	uintptr_t ssiz;
	char *strbuf;
	if (sec_names_only) {
		ssiz = BYTE_TO_HOST(DOFF_ALIGN(dlthis->dfile_hdr.df_scn_name_size));
	} else {
		ssiz = BYTE_TO_HOST(DOFF_ALIGN(dlthis->dfile_hdr.df_strtab_size));
	}
	if (ssiz == 0)
		return;

	/* get some memory for the string table */
#if BITS_PER_AU > BITS_PER_BYTE
	strbuf = (char *)dlthis->mysym->Allocate(dlthis->mysym, ssiz +
											dlthis->dfile_hdr.df_max_str_len);
#else
	strbuf = (char *)dlthis->mysym->Allocate(dlthis->mysym, ssiz);
#endif
	if (strbuf == NULL) {
		DL_ERROR(E_ALLOC, ssiz);
		return;
	}
	dlthis->str_head = strbuf;
#if BITS_PER_AU > BITS_PER_BYTE
	dlthis->str_temp = strbuf + ssiz;
#endif
	/* read in the strings and verify them */
	if ((unsigned)(dlthis->strm->read_buffer(dlthis->strm, strbuf, ssiz))
																	!= ssiz) {
		DL_ERROR(E_READSTRM, S_STRINGTBL);
	}
	/* if we need to fix up byte order, do it now */
#ifndef _BIG_ENDIAN
	if (dlthis->reorder_map) {
		dload_reorder(strbuf, ssiz, dlthis->reorder_map);
	}
	if ((!sec_names_only) && (~dload_checksum(strbuf, ssiz) !=
										dlthis->verify.dv_str_tab_checksum)) {
		DL_ERROR(E_CHECKSUM, S_STRINGTBL);
	}
#else
	if (dlthis->dfile_hdr.df_byte_reshuffle != 
						HOST_BYTE_ORDER(REORDER_MAP(BYTE_RESHUFFLE_VALUE))) {
		/* put strings in big-endian order, not in PC order */
		dload_reorder(strbuf, ssiz, 
						HOST_BYTE_ORDER(dlthis->dfile_hdr.df_byte_reshuffle));
	}
	if ((!sec_names_only) && (~dload_reverse_checksum(strbuf, ssiz) !=
										dlthis->verify.dv_str_tab_checksum)) {
		DL_ERROR(E_CHECKSUM, S_STRINGTBL);
	}
#endif
}				/* dload_strings */

/*****************************************************************************
 * Procedure string_table_free
 *
 * Parameters:
 *	none
 *
 * Effect:
 *	Frees any state used by the string table.
 *
 * WARNING:
 *	This routine is not allowed to declare errors!
 *****************************************************************************/
static void string_table_free(struct dload_state *dlthis)
{
	if (dlthis->str_head) {
		dlthis->mysym->Deallocate(dlthis->mysym, dlthis->str_head);
	}
}				/* string_table_free */

/*
 *
 *	Symbol Table Maintenance Functions
 *
 *	COFF symbols are read by dload_symbols(), which is called after sections
 * have been allocated.  Symbols which might be used in relocation (ie, not
 * debug info) are retained in an internal temporary compressed table (type
 * Local_Symbol).
 * A particular symbol is recovered by index by calling dload_find_symbol().
 * dload_find_symbol reconstructs a more explicit representation (type SLOTVEC)
 * which is used by reloc.c
 *
 */
#define DBG_HDR_SIZE (sizeof(struct dll_module)-sizeof(struct dll_sect))	
/* real size of debug header*/

static const char SYM_ERRID[] = { "symbol" };

/*****************************************************************************
 * Procedure dload_symbols
 *
 * Parameters:
 *	none
 *
 * Effect:
 *	Reads in symbols and retains ones that might be needed for relocation
 * purposes.
 *****************************************************************************/
/* size of symbol buffer no bigger than target data buffer, to limit stack
 * usage*/
#define MY_SYM_BUF_SIZ (BYTE_TO_HOST(IMAGE_PACKET_SIZE)/\
												sizeof(struct doff_syment_t))

#ifdef PERFORMANCE_DATA
unsigned count_symbols;
#endif
static void dload_symbols(struct dload_state *dlthis)
{
	uintptr_t s_count, siz, dsiz, symbols_left;
	uint32_t checks;
	struct Local_Symbol *sp;

	if ((s_count = dlthis->dfile_hdr.df_no_syms) == 0)
		return;
#ifdef PERFORMANCE_DATA
	count_symbols = s_count;
#endif

	/*
	 * We keep a local symbol table for all of the symbols in the input.
	 * This table contains only section & value info, as we do not have
	 * to do any name processing for locals.  We reuse this storage
	 * as a temporary for .dllview record construction.
	 * Allocate storage for the whole table.
	 */
	siz = s_count * sizeof(struct Local_Symbol);
	dsiz = DBG_HDR_SIZE + (sizeof(struct dll_sect) * 
									dlthis->allocated_secn_count) +
							BYTE_TO_HOST_ROUND(dlthis->debug_string_size + 1);
	if (dsiz > siz)
		siz = dsiz;	/* larger of symbols and .dllview temp */
	sp = (struct Local_Symbol *)dlthis->mysym->Allocate(dlthis->mysym, siz);
	if (!sp) {
		DL_ERROR(E_ALLOC, siz);
		return;
	}
	dlthis->local_symtab = sp;
	 /* Read the symbols in the input, store them in the table, and post any
	 * globals to the global symbol table.  In the process, externals become
	 * defined from the global symbol table */
	checks = dlthis->verify.dv_sym_tab_checksum;
	symbols_left = s_count;
	do {			/* read all symbols */
		char *sname;
		uint32_t val;
		int32_t delta;
		struct doff_syment_t *input_sym;
		unsigned syms_in_buf;
		int siz;
		struct doff_syment_t my_sym_buf[MY_SYM_BUF_SIZ];

		input_sym = my_sym_buf;
		syms_in_buf = symbols_left > MY_SYM_BUF_SIZ ? MY_SYM_BUF_SIZ :
																symbols_left;
		siz = syms_in_buf * sizeof(struct doff_syment_t);
		if (dlthis->strm->read_buffer(dlthis->strm, input_sym, siz) != siz) {
			DL_ERROR(E_READSTRM, SYM_ERRID);
			return;
		}
		if (dlthis->reorder_map) {
			dload_reorder(input_sym, siz, dlthis->reorder_map);
		}
		checks += dload_checksum(input_sym, siz);
		do {		/* process symbols in buffer */
			symbols_left -= 1;
			/* attempt to derive the name of this symbol */
			sname = NULL;
			if (input_sym->dn_offset > 0) {
#if BITS_PER_AU <= BITS_PER_BYTE
				if ((uintptr_t) input_sym->dn_offset < 
											dlthis->dfile_hdr.df_strtab_size)
					sname = dlthis->str_head+BYTE_TO_HOST(input_sym->dn_offset);
				else
					dload_error(dlthis, "Bad name offset in symbol %d",
																symbols_left);
#else
				sname = unpack_name(dlthis, input_sym->dn_offset);
#endif
			}
			val = input_sym->dn_value;
			delta = 0;
			sp->sclass = input_sym->dn_sclass;
			sp->secnn = input_sym->dn_scnum;
			/* if this is an undefined symbol, define it (or fail) now */
			if (sp->secnn == DN_UNDEF) {
				if (input_sym->dn_sclass == DN_EXT) {	
					/* pointless for static undefined */
					struct dynload_symbol *symp;
					/* try to define symbol from previously loaded images */
					symp = dlthis->mysym->Find_Matching_Symbol(dlthis->mysym,
																		sname);
					if (symp) {
						val = delta = symp->value;
					} else {
						DL_ERROR("Undefined external symbol %s",sname);
					}
				}
			} else {	/* symbol defined by this module */
				if (sp->secnn > 0) {	/* symbol references a section */
					if ((unsigned)sp->secnn <= dlthis->allocated_secn_count) {
						/* section was allocated */
						struct doff_scnhdr_t *srefp =
												&dlthis->sect_hdrs[sp->secnn-1];
						if (input_sym->dn_sclass == DN_STATLAB ||
											input_sym->dn_sclass == DN_EXTLAB)
							delta = srefp->ds_vaddr;	/* load */
						else
							delta = srefp->ds_paddr;	/* run */
						val += delta;
					}
#if LEAD3
					if (is_data_scn_num(sp->secnn)) {
						/* adjust for 16-bit data addresses */
						delta >>= 1;
						val >>= 1;
					}
#endif
				} else {
					/* This symbol is an absolute symbol */
					if (sp->secnn == DN_ABS && ((sp->sclass == DN_EXT) || 
												(sp->sclass == DN_EXTLAB))) {
						struct dynload_symbol *symp;
						symp = dlthis->mysym->Find_Matching_Symbol
														(dlthis->mysym, sname);
						if (symp) {
							/*
							 * This absolute symbol is already defined.
							 */
#if LEAD3
							/* Check for memory model mixing */
							if (chk_memmodel_mix(dlthis, input_sym, symp, 
																	sname)) {
								/* If memory models are mixed, return */
								return;
							}
#endif
							if (symp->value == input_sym->dn_value) {
								/*
								 * If symbol values are equal, continue but
								 * don't add to the global symbol table
								 */
								sp->value = val;
								sp->delta = delta;
								sp += 1;
								input_sym += 1;
								continue;
							} else {
								/*
								 * If symbol values are not equal, return
								 * with redefinition error
								 */
								DL_ERROR("Absolute symbol %s is defined "
												"multiple times with different"
															"values", sname);
								return;
							}
						}
#if LEAD3
						else {
							/*
							 * This absolute symbol is not already defined.
							 * Check for revision clash before proceeding.
							 */
							if (chk_revision_mix (dlthis, sname)) {
								/* If revision IDs are mixed, return */
								return;
							}
						}
#endif
					}
				}

				/* if this is a global symbol, post it to the global table */
				if (input_sym->dn_sclass == DN_EXT || input_sym->dn_sclass ==
																	DN_EXTLAB) {
					/*
					 * Keep this global symbol for subsequent modules.
					 * Don't complain on error, to allow symbol API to
					 * suppress global symbols
					 */
					if (sname) {
						struct dynload_symbol *newsym;
						newsym = dlthis->mysym->Add_To_Symbol_Table
							(dlthis->mysym, sname, (unsigned)dlthis->myhandle);
						if (newsym) {
							newsym->value = val;
						}
					}
				}	/* global */
			}	/* symbol defined by this module */
			sp->value = val;
			sp->delta = delta;
			sp += 1;
			input_sym += 1;
		}
		while ((syms_in_buf -= 1) > 0);	/* process symbols in buffer */
	}
	while (symbols_left > 0);	/* read all symbols */

	if (~checks) {
		dload_error(dlthis, "Checksum of symbols failed");
	}
}				/* dload_symbols */

/*****************************************************************************
 * Procedure symbol_table_free
 *
 * Parameters:
 *	none
 *
 * Effect:
 *	Frees any state used by the symbol table.
 *
 * WARNING:
 *	This routine is not allowed to declare errors!
 *****************************************************************************/
static void symbol_table_free(struct dload_state *dlthis)
{
	if (dlthis->local_symtab) {
		if (dlthis->dload_errcount) {	/* blow off our symbols */
			dlthis->mysym->Purge_Symbol_Table(dlthis->mysym,
													(unsigned)dlthis->myhandle);
		}
		dlthis->mysym->Deallocate(dlthis->mysym, dlthis->local_symtab);
	}
}				/* symbol_table_free */

/*
 *	.cinit Processing
 *
 * The dynamic loader does .cinit interpretation.  cload_cinit() acts as a 
 * special * write-to-target function, in that it takes relocated data from
 * the normal data * flow, and interprets it as .cinit actions.  Because the
 * normal data flow does not * necessarily process the whole .cinit section in
 * one buffer, cload_cinit() must * be prepared to interpret the data piecemeal.
 * A state machine is used for this purpose.
 */
/* The following are only for use by reloc.c and things it calls */
static const struct LDR_SECTION_INFO CINIT_INFO_INIT = { CINITNAME, 0, 0,
												(LDR_ADDR) - 1,0,DLOAD_BSS,0};
#ifdef PERFORMANCE_DATA
unsigned count_cinits;
#endif

/*****************************************************************************
 * Procedure cload_cinit
 *
 * Parameters:
 *	ipacket		Pointer to data packet to be loaded
 *
 * Effect:
 *	Interprets the data in the buffer as .cinit data, and performs the
 * appropriate initializations.
 *****************************************************************************/
static void cload_cinit(struct dload_state *dlthis, 
												struct image_packet_t *ipacket)
{
#if TDATA_TO_HOST(CINIT_COUNT)*BITS_PER_AU > 16
	int_least32_t init_count, left;
#else
	int_least16_t init_count, left;
#endif
	unsigned char *pktp = ipacket->i_bits;
	unsigned char *pktend =
	    pktp + BYTE_TO_HOST_ROUND(ipacket->i_packet_size);
	int temp;
	LDR_ADDR atmp;
	struct LDR_SECTION_INFO cinit_info;

	/*------------------------------------------------------------------------*/
	/*  PROCESS ALL THE INITIALIZATION RECORDS IN THE BUFFER.                 */
	/*------------------------------------------------------------------------*/
	while (true) {
		left = pktend - pktp;
		switch (dlthis->cinit_state) {
		case CI_count:	/* count field */
			if (left < TDATA_TO_HOST(CINIT_COUNT))
				goto loopexit;
			temp = dload_unpack(dlthis, (TgtAU_t *)pktp, 
										CINIT_COUNT * TDATA_AU_BITS,0,ROP_SGN);
			pktp += TDATA_TO_HOST(CINIT_COUNT);
			if (temp <= 0) {/* negative signifies BSS table, zero means done */
#ifdef CINIT_BSSREF_BYTES
				dlthis->cinit_state = temp ? CI_addbss : CI_done;
				dlthis->cinit_count = 0 - temp;
				break;
#else
				dlthis->cinit_state = CI_done;
				break;
#endif
			}
			dlthis->cinit_count = temp;
			dlthis->cinit_state = CI_address;
			break;
#if CINIT_ALIGN < CINIT_ADDRESS
		case CI_partaddress:
			pktp -= TDATA_TO_HOST(CINIT_ALIGN);	
			/* back up pointer into space courtesy of caller */
			*(uint16_t *)pktp = dlthis->cinit_addr;/* stuff in saved bits*/
			/* !! FALL THRU !! */
#endif
		case CI_address:	/* Address field for a copy packet */
			if (left < TDATA_TO_HOST(CINIT_ADDRESS)) {
#if CINIT_ALIGN < CINIT_ADDRESS
				if (left == TDATA_TO_HOST(CINIT_ALIGN)) {	
					/* address broken into halves */
					dlthis->cinit_addr = *(uint16_t *)pktp;	
					/* remember 1st half */
					dlthis->cinit_state = CI_partaddress;
					left = 0;
				}
#endif
				goto loopexit;
			}
			atmp = dload_unpack(dlthis, (TgtAU_t *)pktp, 
									CINIT_ADDRESS * TDATA_AU_BITS, 0, ROP_UNS);
			pktp += TDATA_TO_HOST(CINIT_ADDRESS);
#if CINIT_PAGE_BITS > 0
			dlthis->cinit_page = atmp & ((1 << CINIT_PAGE_BITS) - 1);
			atmp >>= CINIT_PAGE_BITS;
#else
			dlthis->cinit_page = CINIT_DEFAULT_PAGE;
#endif
			dlthis->cinit_addr = atmp;
			dlthis->cinit_state = CI_copy;
			break;

		case CI_copy:	/* copy bits to the target */
#ifdef PERFORMANCE_DATA
			count_cinits += 1;
#endif
			init_count = HOST_TO_TDATA(left);
			if (init_count > dlthis->cinit_count)
				init_count = dlthis->cinit_count;
			if (init_count == 0)
				goto loopexit;	/* get more bits */
			cinit_info = CINIT_INFO_INIT;
			cinit_info.page = dlthis->cinit_page;
			if (!dlthis->myio->writemem(dlthis->myio, pktp, 
								TDATA_TO_TADDR(dlthis->cinit_addr), &cinit_info,
													TDATA_TO_HOST(init_count))){
				dload_error(dlthis, E_INITFAIL, "write", dlthis->cinit_addr);
			}
			if ((dlthis->cinit_count -= init_count) <= 0) {
				dlthis->cinit_state = CI_count;
				init_count = (init_count + CINIT_ALIGN - 1) & -CINIT_ALIGN;
				/* align to next init */
			}
			pktp += TDATA_TO_HOST(init_count);
			dlthis->cinit_addr += init_count;
			break;
		case CI_done:	/* no more .cinit to do */
			return;
		}		/* switch (cinit_state) */
	}			/* while */
loopexit:
	if (left > 0) {
		dload_error(dlthis, "%d bytes left over in cinit packet", left);
		dlthis->cinit_state = CI_done;	/* left over bytes are bad */
	}
}				/* cload_cinit */

/*
 *		Functions to interface to reloc.c
 *
 * reloc.c is the relocation module borrowed from the linker, with minimal
 * (we hope) * changes for our purposes.  cload_sect_data() invokes this module
 * on a section to * relocate and load the image data for that section.  
 * The actual read and write  actions are supplied by the global routines below.
 */

/*****************************************************************************
 * Procedure relocate_packet
 *
 * Parameters:
 *	ipacket		Pointer to an image packet to relocate
 *
 * Effect:
 *	Performs the required relocations on the packet.  Returns a checksum
 * of the relocation operations.
 *****************************************************************************/
#ifdef PERFORMANCE_DATA
unsigned count_relocs;
#endif
#define MY_RELOC_BUF_SIZ 8	
/* careful! exists at the same time as the image buffer*/
static int relocate_packet(struct dload_state *dlthis, 
								struct image_packet_t *ipacket,uint32_t *checks)
{
	uintptr_t rnum;
	rnum = ipacket->i_num_relocs;
#ifdef PERFORMANCE_DATA
	count_relocs += rnum;
#endif
	do {			
		/* all relocs */
		unsigned rinbuf;
		int siz;
		struct reloc_record_t *rp, rrec[MY_RELOC_BUF_SIZ];
		rp = rrec;
		rinbuf = rnum > MY_RELOC_BUF_SIZ ? MY_RELOC_BUF_SIZ : rnum;
		siz = rinbuf * sizeof(struct reloc_record_t);
		if (dlthis->strm->read_buffer(dlthis->strm, rp, siz) != siz) {
			DL_ERROR(E_READSTRM, "relocation");
			return 0;
		}
		/* reorder the bytes if need be */
		if (dlthis->reorder_map) {
			dload_reorder(rp, siz, dlthis->reorder_map);
		}
		*checks += dload_checksum(rp, siz);
		do {
			/* perform the relocation operation */
			dload_relocate(dlthis, (TgtAU_t *) ipacket->i_bits, rp);
			rp += 1;
			rnum -= 1;
		}
		while ((rinbuf -= 1) > 0);
	}
	while (rnum > 0);	/* all relocs */
	return 1;
}				/* dload_read_reloc */

#define IPH_SIZE (sizeof(struct image_packet_t) - sizeof(uint32_t))	
/* VERY dangerous*/
static const char IMAGEPAK[] = { "image packet" };

/*****************************************************************************
 * Procedure dload_data
 *
 * Parameters:
 *	none
 *
 * Effect:
 *	Read image data from input file, relocate it, and download it to the target.
 *****************************************************************************/
#ifdef PERFORMANCE_DATA
unsigned count_image;
#endif
static void dload_data(struct dload_state *dlthis)
{
	uint_least16_t curr_sect;
	struct doff_scnhdr_t *sptr = dlthis->sect_hdrs;
	struct LDR_SECTION_INFO *lptr = dlthis->ldr_sections;
#ifdef OPT_ZERO_COPY_LOADER
	boolean bZeroCopy = false;
#endif
	uint_least8_t *pDest;

	struct {
		struct image_packet_t ipacket;
		uint_least8_t bufr[BYTE_TO_HOST(IMAGE_PACKET_SIZE)];
	} ibuf;

	/* Indicates whether CINIT processing has occurred */
	boolean cinit_processed = false;

	/*
	 * Loop through the sections and load them one at a time.
	 */
	for (curr_sect = 0;curr_sect < dlthis->dfile_hdr.df_no_scns;curr_sect += 1){
		if (DS_NEEDS_DOWNLOAD(sptr)) {
			int_least32_t nip;
			LDR_ADDR image_offset = 0;
			/* set relocation info for this section */
			if (curr_sect < dlthis->allocated_secn_count) {
				dlthis->delta_runaddr = sptr->ds_paddr;
#if LEAD3
				if (is_data_scn(lptr))
					dlthis->delta_runaddr >>= 1;	/* word-addressed data */
#endif
			} else {
				lptr = DOFFSEC_IS_LDRSEC(sptr);
				dlthis->delta_runaddr = 0;
			}
			dlthis->image_secn = lptr;
#if BITS_PER_AU > BITS_PER_BYTE
			lptr->name = unpack_name(dlthis, sptr->ds_offset);
#endif
			nip = sptr->ds_nipacks;
			while ((nip -= 1) >= 0) {	/* process packets */
				int_least32_t ipsize;
				uint32_t checks;
				/* get the fixed header bits */
				if (dlthis->strm->read_buffer(dlthis->strm, &ibuf.ipacket,
														IPH_SIZE) != IPH_SIZE) {
					DL_ERROR(E_READSTRM, IMAGEPAK);
					return;
				}
				/* reorder the header if need be */
				if (dlthis->reorder_map) {
					dload_reorder(&ibuf.ipacket, IPH_SIZE,
						      dlthis->reorder_map);
				}
#ifdef PERFORMANCE_DATA
				count_image += ibuf.ipacket.i_packet_size;
#endif
				/* now read the rest of the packet */
				ipsize = BYTE_TO_HOST(DOFF_ALIGN(ibuf.ipacket.i_packet_size));
				if (ipsize > BYTE_TO_HOST(IMAGE_PACKET_SIZE)) {
					DL_ERROR("Bad image packet size %d",ipsize);
					return;
				}
                /* Determine whether we can read directly into the DSP memory */
				pDest = ibuf.bufr;
#ifdef OPT_ZERO_COPY_LOADER
		                bZeroCopy = false;
		if (DLOAD_SECT_TYPE(sptr) != DLOAD_CINIT) {
		dlthis->myio->writemem(dlthis->myio, &pDest, lptr->load_addr + image_offset, lptr, 0);
		bZeroCopy = (pDest != ibuf.bufr);
		}
#endif
                /* End of determination */

				if (dlthis->strm->read_buffer(dlthis->strm, pDest,ipsize)
																	!= ipsize) {
					DL_ERROR(E_READSTRM, IMAGEPAK);
					return;
				}
				ibuf.ipacket.i_bits = pDest;
				/* reorder the bytes if need be */
#if !defined(_BIG_ENDIAN) || (TARGET_AU_BITS > 16)
				if (dlthis->reorder_map) {
					dload_reorder(pDest, ipsize, dlthis->reorder_map);
				}
				checks = dload_checksum(pDest, ipsize);
#else
				if (dlthis->dfile_hdr.df_byte_reshuffle != 
							TARGET_ORDER(REORDER_MAP(BYTE_RESHUFFLE_VALUE))) {
					/* put image bytes in big-endian order, not PC order */
					dload_reorder(pDest,ipsize,TARGET_ORDER(
										dlthis->dfile_hdr.df_byte_reshuffle));
				}
#if TARGET_AU_BITS > 8
				checks = dload_reverse_checksum_16(pDest, ipsize);
#else
				checks = dload_reverse_checksum(pDest, ipsize);
#endif
#endif 
				checks += dload_checksum(&ibuf.ipacket, IPH_SIZE);
				/* relocate the image bits as needed */
				if (ibuf.ipacket.i_num_relocs) {
					dlthis->image_offset = image_offset;
					if (!relocate_packet(dlthis, &ibuf.ipacket, &checks))
						return;	/* serious error */
				}
				if (~checks)
					DL_ERROR(E_CHECKSUM, IMAGEPAK);
				/* stuff the result into target memory */
				if (DLOAD_SECT_TYPE(sptr) == DLOAD_CINIT) {
					cload_cinit(dlthis, &ibuf.ipacket);
					cinit_processed = true;
				} else {
#ifdef OPT_ZERO_COPY_LOADER
                                    if (!bZeroCopy) {
#endif

					if (!dlthis->myio->writemem(dlthis->myio, ibuf.bufr, 
								lptr->load_addr + image_offset, lptr,
									BYTE_TO_HOST(ibuf.ipacket.i_packet_size))) {
						DL_ERROR("Write to " FMT_UI32 " failed", 
												lptr->load_addr + image_offset);
					}
#ifdef OPT_ZERO_COPY_LOADER
                                }
#endif

				}
				image_offset += BYTE_TO_TADDR(ibuf.ipacket.i_packet_size);
			}	/* process packets */
			/* if this is a BSS section, we may want to fill it */
			if (DLOAD_SECT_TYPE(sptr) == DLOAD_BSS) {
				if (dlthis->myoptions & DLOAD_INITBSS) {
					if (!cinit_processed) {
						/* fill the .bss area */
						dlthis->myio->fillmem(dlthis->myio, 
									TADDR_TO_HOST(lptr->load_addr), lptr,
									TADDR_TO_HOST(lptr->size),dload_fill_bss);
					} else {
						/* Don't clear BSS _after_ load-time initialization! */
						DL_ERROR ("Zero-initialization at " FMT_UI32 " after "
								"load-time initialization!",lptr->load_addr);
					}
				}
			}

		} /* if DS_DOWNLOAD_MASK */
		else {
			/* If not loading, but BSS, zero initialize */
			if (DLOAD_SECT_TYPE(sptr) == DLOAD_BSS) {
				if (dlthis->myoptions & DLOAD_INITBSS) {
					if (curr_sect >= dlthis->allocated_secn_count) {
						lptr = DOFFSEC_IS_LDRSEC(sptr);
					}
					if (!cinit_processed) {
						/* fill the .bss area */
						dlthis->myio->fillmem(dlthis-> myio,
									TADDR_TO_HOST(lptr->load_addr), lptr, 
									TADDR_TO_HOST(lptr->size),dload_fill_bss);
					} else {
						/* Don't clear BSS _after_ load-time initialization! */
						DL_ERROR ("Zero-initialization at " FMT_UI32 " \
									attempted after load-time initialization!",
															lptr->load_addr);
					}
				}
			}
		}
		sptr += 1;
		lptr += 1;
	}			/* load sections */
}				/* dload_data */

/*****************************************************************************
 * Procedure dload_reorder
 *
 * Parameters:
 *	data	32-bit aligned pointer to data to be byte-swapped
 *	dsiz	size of the data to be reordered in sizeof() units.
 *	map		32-bit map defining how to reorder the data.  Value must be
 *			REORDER_MAP() of some permutation of 0x00 01 02 03
 *
 * Effect:
 *	Re-arranges the bytes in each word according to the map specified.
 *
 *****************************************************************************/
#define SHIFT_COUNT_MASK (3 << LOG_BITS_PER_BYTE)
/* mask for byte shift count */

void dload_reorder(void *data, int dsiz, uint_least32_t map)
{
	register uint_fast32_t tmp, tmap, datv;
	uint32_t *dp = (uint32_t *) data;
	map <<= LOG_BITS_PER_BYTE;	/* align map with SHIFT_COUNT_MASK */
	do {
		tmp = 0;
		datv = *dp;
		tmap = map;
		do {
			tmp |= (datv & BYTE_MASK) << (tmap & SHIFT_COUNT_MASK);
			tmap >>= BITS_PER_BYTE;
		}
		while (datv >>= BITS_PER_BYTE);
		*dp++ = tmp;
	}
	while ((dsiz -= sizeof(uint32_t)) > 0);
}				/* dload_reorder */

/*****************************************************************************
 * Procedure dload_checksum
 *
 * Parameters:
 *	data	32-bit aligned pointer to data to be checksummed
 *	siz		size of the data to be checksummed in sizeof() units.
 *
 * Effect:
 *	Returns a checksum of the specified block
 *
 *****************************************************************************/
uint32_t dload_checksum(void *data, unsigned siz)
{
	uint_fast32_t sum;
	uint32_t *dp;
	int left;

	sum = 0;
	dp = (uint32_t *) data;

	for (left = siz;left > 0;left -= sizeof(uint32_t))
		sum += *dp++;

	return sum;
}				/* dload_checksum */

#if HOST_ENDIANNESS
/*****************************************************************************
 * Procedure dload_reverse_checksum
 *
 * Parameters:
 *	data	32-bit aligned pointer to data to be checksummed
 *	siz		size of the data to be checksummed in sizeof() units.
 *
 * Effect:
 *	Returns a checksum of the specified block, which is assumed to be bytes
 * in big-endian order.
 *
 * Notes:
 *	In a big-endian host, things like the string table are stored as bytes
 * in host order. But dllcreate always checksums in little-endian order.
 * It is most efficient to just handle the difference a word at a time.
 *
 *****************************************************************************/
uint32_t dload_reverse_checksum(void *data, unsigned siz)
{
	uint_fast32_t sum, temp;
	uint32_t *dp;
	int left;

	sum = 0;
	dp = (uint32_t *) data;

	for (left = siz;left > 0;left -= sizeof (uint32_t)) {
		temp = *dp++;
		sum += temp << BITS_PER_BYTE * 3;
		sum += temp >> BITS_PER_BYTE * 3;
		sum += (temp >> BITS_PER_BYTE) & (BYTE_MASK << BITS_PER_BYTE);
		sum += (temp & (BYTE_MASK << BITS_PER_BYTE)) << BITS_PER_BYTE;
	}

	return sum;
}				/* dload_reverse_checksum */

#if (TARGET_AU_BITS > 8) && (TARGET_AU_BITS < 32)
uint32_t dload_reverse_checksum_16(void *data, unsigned siz)
{
	uint_fast32_t sum, temp;
	uint32_t *dp;
	int left;

	sum = 0;
	dp = (uint32_t *)data;

	for (left = siz;left > 0;left -= sizeof(uint32_t)) {
		temp = *dp++;
		sum += temp << BITS_PER_BYTE * 2;
		sum += temp >> BITS_PER_BYTE * 2;
	}

	return sum;
}				/* dload_reverse_checksum_16 */
#endif
#endif

/*****************************************************************************
 * Procedure swap_words
 *
 * Parameters:
 *	data	32-bit aligned pointer to data to be swapped
 *	siz		size of the data to be swapped.
 *	bitmap	Bit map of how to swap each 32-bit word; 1 => 2 shorts, 0 => 1 long
 *
 * Effect:
 *	Swaps the specified data according to the specified map
 *
 *****************************************************************************/
static void swap_words(void *data, unsigned siz, unsigned bitmap)
{
	register int i;
#if TARGET_AU_BITS < 16
	register uint16_t *sp;
#endif
	register uint32_t *lp;

	siz /= sizeof(uint16_t);

#if TARGET_AU_BITS < 16
	/* pass 1: do all the bytes */
	i = siz;
	sp = (uint16_t *)data;
	do {
		register uint_fast16_t tmp;
		tmp = *sp;
		*sp++ = SWAP16BY8(tmp);
	} while ((i -= 1) > 0);
#endif

#if TARGET_AU_BITS < 32
	/* pass 2: fixup the 32-bit words */
	i = siz >> 1;
	lp = (uint32_t *)data;
	do {
		if ((bitmap & 1) == 0) {
			register uint_fast32_t tmp;
			tmp = *lp;
			*lp = SWAP32BY16(tmp);
		}
		lp += 1;
		bitmap >>= 1;
	} while ((i -= 1) > 0);
#endif

}				/* swap_words */

/*****************************************************************************
 * Procedure copy_tgt_strings
 *
 * Parameters:
 *	dstp		Destination address.  Assumed to be 32-bit aligned
 *	srcp		Source address.  Assumed to be 32-bit aligned
 *	charcount	Number of characters to copy.
 *
 * Effect:
 *	Copies strings from the source (which is in usual .dof file order on the
 * loading processor) to the destination buffer (which should be in proper
 * target addressable unit order).  Makes sure the last string in the
 * buffer is NULL terminated (for safety).
 * Returns the first unused destination address.
 *
 *****************************************************************************/
static char *copy_tgt_strings(void *dstp, void *srcp, unsigned charcount)
{
	register TgtAU_t *src = (TgtAU_t *)srcp;
	register TgtAU_t *dst = (TgtAU_t *)dstp;
	register int cnt = charcount;

	do {
		/* byte-swapping issues may exist for strings on target */
		*dst++ = *src++;
	}
	while ((cnt -= (sizeof(TgtAU_t) * BITS_PER_AU / BITS_PER_BYTE)) > 0);
	/* apply force to make sure that the string table has a null terminator */
#if (BITS_PER_AU == BITS_PER_BYTE) && (TARGET_AU_BITS == BITS_PER_BYTE)
	dst[-1] = 0;
#else
	dst[-1] &= (1 << (BITS_PER_AU - BITS_PER_BYTE)) - 1;	/*  little endian */
#endif
	return (char *) dst;
}				/* copy_tgt_strings */

/*****************************************************************************
 * Procedure init_module_handle
 *
 * Parameters:
 *	none
 *
 * Effect:
 *	Initializes the module handle we use to enable unloading, and installs the
 * debug information required by the target.
 *
 * Notes:
 * The handle returned from Dynamic_Load_Module needs to encapsulate all the
 * allocations done for the module, and enable them plus the modules symbols to
 * be deallocated.
 *
 *****************************************************************************/
#ifndef _BIG_ENDIAN
static const struct LDR_SECTION_INFO DLLVIEW_INFO_INIT =
    { ".dllview", 0, 0, (LDR_ADDR) - 1, DBG_LIST_PAGE, DLOAD_DATA, 0 };
#else
static const struct LDR_SECTION_INFO DLLVIEW_INFO_INIT =
    { ".dllview", 0, 0, (LDR_ADDR) - 1, DLOAD_DATA, DBG_LIST_PAGE, 0 };
#endif

static void init_module_handle(struct dload_state *dlthis)
{
	struct my_handle *hndl;
	uint_least16_t curr_sect;
	struct LDR_SECTION_INFO *asecs;
	struct dll_module *dbmod;
	struct dll_sect *dbsec;
	struct dbg_mirror_root *mlist;
	register char *cp;
	struct modules_header mhdr;
	struct LDR_SECTION_INFO dllview_info;
	struct dynload_symbol *debug_mirror_sym;

	hndl = dlthis->myhandle;
	if (!hndl)
		return;		/* must be errors detected, so forget it */

	hndl->secn_count = dlthis->allocated_secn_count << 1;
#ifndef TARGET_ENDIANNESS
	if (dlthis->big_e_target)
		hndl->secn_count += 1;	/* flag for big-endian */
#endif
	if (dlthis->dload_errcount)
		return;		/* abandon if errors detected */
	 /* Locate the symbol that names the header for the CCS debug list of 
	 * modules.  * If not found, we just don't generate the debug record. 
	 * If found, we create * our modules list.  We make sure to create the
	 * LOADER_DLLVIEW_ROOT even if * there is no relocation info to record,
	 * just to try to put both symbols in * the same symbol table and module.*/
	debug_mirror_sym = dlthis->mysym->Find_Matching_Symbol(dlthis->mysym,
														LOADER_DLLVIEW_ROOT);
	if (!debug_mirror_sym) {
		struct dynload_symbol *dlmodsym;
		struct dbg_mirror_root *mlst;
#ifdef NEGATIVE_TESTING
		test_permit_error = 1;
#endif
		/* our root symbol is not yet present; check if we have DLModules
		 * defined */
		dlmodsym = dlthis->mysym->Find_Matching_Symbol(dlthis->mysym,
														LINKER_MODULES_HEADER);
		if (!dlmodsym)
			return;	/* no DLModules list so no debug info */
		/* if we have DLModules defined, construct our header */
		mlst = (struct dbg_mirror_root *)dlthis->mysym->Allocate(dlthis->mysym, 
												sizeof(struct dbg_mirror_root));
		if (!mlst) {
			DL_ERROR(E_ALLOC, sizeof(struct dbg_mirror_root));
			return;
		}
		mlst->hnext = NULL;
		mlst->changes = 0;
		mlst->refcount = 0;
		mlst->dbthis = TDATA_TO_TADDR(dlmodsym->value);
		/* add our root symbol */
		debug_mirror_sym = dlthis->mysym->Add_To_Symbol_Table(dlthis->mysym, 
								LOADER_DLLVIEW_ROOT,(unsigned)dlthis->myhandle);
		if (!debug_mirror_sym) {
			dlthis->mysym->Deallocate(dlthis->mysym, mlst);	
			/* failed, recover memory */
			return;
		}
		debug_mirror_sym->value = (uintptr_t)mlst;
	}
	 /* First create the DLLview record and stuff it into the buffer.
	 * Then write it to the DSP.  Record pertinent locations in our hndl, and
	 * add it to the per-processor * list of handles with debug info.  */
#ifndef DEBUG_HEADER_IN_LOADER
	mlist = (struct dbg_mirror_root *)debug_mirror_sym->value;
	if (!mlist)
		return;
#else
	mlist = (struct dbg_mirror_root *)&debug_list_header;
#endif
	hndl->dm.hroot = mlist;	/* set pointer to root into our handle */
	if (!dlthis->allocated_secn_count)
		return;		/* no load addresses to be recorded */
	dbmod = (struct dll_module *)dlthis->local_symtab;	
	/* reuse temporary symbol storage */
	 /* Create the DLLview record in the memory we retain for our handle */
	dbmod->num_sects = dlthis->allocated_secn_count;
	dbmod->timestamp = dlthis->verify.dv_timdat;
	dbmod->version = INIT_VERSION;
	dbmod->verification = VERIFICATION;
	asecs = dlthis->ldr_sections;
	dbsec = dbmod->sects;
	for (curr_sect = dlthis->allocated_secn_count;curr_sect > 0;curr_sect -= 1){
#if LEAD3
#define LEAD3_DATA_FLAG (3UL << 30)
		if (is_data_scn(asecs)) {
			dbsec->sect_load_adr = asecs->load_addr >> 1;
			dbsec->sect_run_adr = (asecs->run_addr >> 1) | LEAD3_DATA_FLAG;
		} else
#endif
		{
			dbsec->sect_load_adr = asecs->load_addr;
			dbsec->sect_run_adr = asecs->run_addr;
		}
		dbsec += 1;
		asecs += 1;
	}
	/* now cram in the names */
	cp = copy_tgt_strings(dbsec, dlthis->str_head, dlthis->debug_string_size);
	/* round off the size of the debug record, and remember same */
	hndl->dm.dbsiz = HOST_TO_TDATA_ROUND(cp - (char *)dbmod);
	*cp = 0;		/* strictly to make our test harness happy */
	dllview_info = DLLVIEW_INFO_INIT;
	dllview_info.size = TDATA_TO_TADDR(hndl->dm.dbsiz);
	/* Initialize memory context to default heap */
	dllview_info.context = 0;
	hndl->dm.context = 0;
	/* fill in next pointer and size */
	if (mlist->hnext) {
		dbmod->next_module = TADDR_TO_TDATA(mlist->hnext->dm.dbthis);
		dbmod->next_module_size = mlist->hnext->dm.dbsiz;
	} else {
		dbmod->next_module_size = 0;
		dbmod->next_module = 0;
	}
	/* allocate memory for on-DSP DLLview debug record */
	if (!dlthis->myalloc)
		return;
	if (!dlthis->myalloc->
	    Allocate(dlthis->myalloc, &dllview_info,
											HOST_TO_TADDR(sizeof(uint32_t)))) {
#ifdef NEGATIVE_TESTING
		test_permit_error = 1;
#endif
		return;
	}
	/* Store load address of .dllview section */
	hndl->dm.dbthis = dllview_info.load_addr;
	/* Store memory context (segid) in which .dllview section was allocated */
	hndl->dm.context = dllview_info.context;
	mlist->refcount += 1;
	/* swap bytes in the entire debug record, but not the string table */
	if (TARGET_ENDIANNESS_DIFFERS(TARGET_BIG_ENDIAN)) {
		swap_words(dbmod, (char *)dbsec - (char *)dbmod, DLL_MODULE_BITMAP);
	}
	 /* Update the DLLview list on the DSP */
	/* write new record */
	if (!dlthis->myio->writemem(dlthis->myio, dbmod, dllview_info.load_addr,
							&dllview_info, TADDR_TO_HOST(dllview_info.size))) {
#ifdef NEGATIVE_TESTING
		test_permit_error = 1;
#endif
		return;
	}
	/* write new header */
	mhdr.first_module_size = hndl->dm.dbsiz;
	mhdr.first_module = TADDR_TO_TDATA(dllview_info.load_addr);
	/* swap bytes in the module header, if needed */
	if (TARGET_ENDIANNESS_DIFFERS(TARGET_BIG_ENDIAN)) {
		swap_words(&mhdr, sizeof(struct modules_header) - sizeof(uint16_t),
														MODULES_HEADER_BITMAP);
	}
	dllview_info = DLLVIEW_INFO_INIT;
	if (!dlthis->myio->writemem(dlthis->myio,&mhdr,mlist->dbthis,&dllview_info,
							sizeof(struct modules_header) - sizeof(uint16_t))){
#ifdef NEGATIVE_TESTING
		test_permit_error = 1;
#endif
		return;
	}
	 /* Add the module handle to this processor's list of 
	  * handles with debug info */
	hndl->dm.hnext = mlist->hnext;
	if (hndl->dm.hnext)
		hndl->dm.hnext->dm.hprev = hndl;
	hndl->dm.hprev = (struct my_handle *) mlist;
	mlist->hnext = hndl;	/* insert after root*/

}				/* init_module_handle */

/*****************************************************************************
 * Procedure Dynamic_Unload_Module
 *
 * Parameters:
 *	mhandle	A module handle from Dynamic_Load_Module
 *	syms	Host-side symbol table and malloc/free functions
 *	alloc	Target-side memory allocation
 *
 * Effect:
 *	The module specified by mhandle is unloaded.  Unloading causes all
 * target memory to be deallocated, all symbols defined by the module to
 * be purged, and any host-side storage used by the dynamic loader for
 * this module to be released.
 *
 * Returns:
 *	Zero for success. On error, the number of errors detected is returned.
 * Individual errors are reported using syms->Error_Report().
 *****************************************************************************/
/* the module handle*/ /* host support for symbols and storage*/
/* the target memory allocator*/ /* the target memory initializer*/
extern int Dynamic_Unload_Module(DLOAD_mhandle mhandle,
					struct Dynamic_Loader_Sym *syms,
					struct Dynamic_Loader_Allocate *alloc, 
					struct Dynamic_Loader_Initialize *init)
{
	int_least16_t curr_sect;
	struct LDR_SECTION_INFO *asecs;
	struct my_handle *hndl;
	struct dbg_mirror_root *root;
	unsigned errcount = 0;

	hndl = (struct my_handle *)mhandle;
	if (!hndl)
		return 0;	/* if handle is null, nothing to do */
	 /* Clear out the module symbols  Note that if this is the module that
	  * defined MODULES_HEADER (the head of the target debug list) then this
	  * operation will blow away that symbol.  It will therefore be impossible
	  for subsequent operations to add entries to this un-referenceable list. */
	if (!syms)
		return 1;
	syms->Purge_Symbol_Table(syms, (unsigned)hndl);
	 /* Deallocate target memory for sections */
	asecs = hndl->secns;
	if (alloc)
		for (curr_sect = (hndl->secn_count >> 1);curr_sect > 0;curr_sect -= 1) {
			asecs->name = NULL;
			alloc->Deallocate(alloc, asecs++);
		}
	root = hndl->dm.hroot;
	if (root) {		/* there is a debug list containing this module */
		if (hndl->dm.dbthis) {	/* target-side dllview record exists */
			struct LDR_SECTION_INFO dllview_info = DLLVIEW_INFO_INIT;
			/* Retrieve memory context in which .dllview was allocated */
			dllview_info.context = hndl->dm.context;
			if (hndl->dm.hprev != hndl) {	
				/* target-side dllview record is in list */
				struct modules_header mhdr;
				/* dequeue this record from our GPP-side mirror list */
				hndl->dm.hprev->dm.hnext = hndl->dm.hnext;
				if (hndl->dm.hnext)
					hndl->dm.hnext->dm.hprev = hndl->dm.hprev;
				 /* Update next_module of previous entry in target list
				 * We are using mhdr here as a surrogate for either a
				 * modules_header or a struct dll_module */
				if (hndl->dm.hnext) {
					mhdr.first_module = 
									TADDR_TO_TDATA(hndl->dm.hnext->dm.dbthis);
					mhdr.first_module_size = hndl->dm.hnext->dm.dbsiz;
				} else {
					mhdr.first_module = 0;
					mhdr.first_module_size = 0;
				}
				if (init) {
					if (!init->connect(init)) {
						dload_syms_error(syms, E_ICONNECT);
						errcount += 1;
						goto exitunltgt;
					}
					/* swap bytes in the module header, if needed */
					if (TARGET_ENDIANNESS_DIFFERS(hndl->secn_count & 0x1)) {
						swap_words(&mhdr, sizeof(struct modules_header) - 
										sizeof(uint16_t),MODULES_HEADER_BITMAP);
					}
					if (!init->writemem(init, &mhdr, hndl->dm.hprev->dm.dbthis,
								&dllview_info, sizeof(struct modules_header) -
													sizeof(mhdr.update_flag))) {
						dload_syms_error(syms, E_DLVWRITE);
						errcount += 1;
					}
					/* update change counter */
					root->changes += 1;
					if (!init->writemem(init,&(root->changes),root->dbthis + 
								HOST_TO_TADDR(sizeof(mhdr.first_module) + 
								sizeof(mhdr.first_module_size)),&dllview_info,
													sizeof(mhdr.update_flag))) {
						dload_syms_error(syms, E_DLVWRITE);
						errcount += 1;
					}
					init->release(init);
				}

			}	/* target-side dllview record is in list */
exitunltgt:
			/* release target storage */
			dllview_info.size = TDATA_TO_TADDR(hndl->dm.dbsiz);
			dllview_info.load_addr = hndl->dm.dbthis;
			if (alloc)
				alloc->Deallocate(alloc, &dllview_info);
			root->refcount -= 1;

		}
		/* target-side dllview record exists */
#ifndef DEBUG_HEADER_IN_LOADER
		if (root->refcount <= 0) {	
			/* if all references gone, blow off the header */
			/* our root symbol may be gone due to the Purge above, but if not,
			 * do not destroy the root */
			if (syms->Find_Matching_Symbol(syms, LOADER_DLLVIEW_ROOT) == NULL)
				syms->Deallocate(syms, root);
		}
#endif
	}
	/* there is a debug list containing this module */
	syms->Deallocate(syms, mhandle);	/* release our storage */
	return errcount;
}				/* Dynamic_Unload_Module */

#if BITS_PER_AU > BITS_PER_BYTE
/*****************************************************************************
 * Procedure unpack_name
 *
 * Parameters:
 *	soffset	Byte offset into the string table
 *
 * Effect:
 *	Returns a pointer to the string specified by the offset supplied, or
 * NULL for error.
 *
 *****************************************************************************/
static char *unpack_name(struct dload_state *dlthis, uint32_t soffset)
{
	uint_least8_t tmp, *src;
	char *dst;

	if (soffset >= dlthis->dfile_hdr.df_strtab_size) {
		dload_error(dlthis, "Bad string table offset " FMT_UI32, soffset);
		return NULL;
	}

	src = (uint_least8_t *)dlthis->str_head + (soffset >>
										(LOG_BITS_PER_AU - LOG_BITS_PER_BYTE));
	dst = dlthis->str_temp;
	if (soffset & 1)
		*dst++ = *src++;	/* only 1 character in first word */
	do {
		tmp = *src++;
		if (!(*dst++ = (tmp >> BITS_PER_BYTE)))
			break;
	} while ((*dst++ = tmp & BYTE_MASK));
	dlthis->temp_len = dst - dlthis->str_temp;	
	/* squirrel away length including terminating null */
	return dlthis->str_temp;
}				/* unpack_name */
#endif

#if LEAD3
/*****************************************************************************
 * Procedure chk_revision_mix
 *
 * Parameters:
 *  sname: Name of absolute symbol
 *
 * Effect:
 *  Given a symbol name, determine whether it reflects a CPU revision ID.  If
 *  it does, determine whether a different revision ID has already been
 *  defined.  If so, this would indicate that there is a CPU revision conflict,
 *  so produce an error and return true.  Otherwise, there is no conflict,
 *  return false.
 *
 *****************************************************************************/
static boolean chk_revision_mix(struct dload_state *dlthis, const char *sname)
{
	char *filename;
	struct dynload_symbol *symp;

#if BITS_PER_AU > BITS_PER_BYTE
	filename = unpack_name(dlthis, 0);
#else
	filename = dlthis->str_head + BYTE_TO_HOST(0);
#endif

	if (DL_STRCMP(sname, TI_C55X_REV2) == 0) {
		symp = dlthis->mysym->Find_Matching_Symbol(dlthis->mysym, TI_C55X_REV3);
		if (symp) {
			DL_ERROR("CPU rev 3 required, but %s forbids rev 3", filename);
			return true;
		}
	} else if (DL_STRCMP(sname, TI_C55X_REV3) == 0) {
		symp = dlthis->mysym->Find_Matching_Symbol(dlthis->mysym, TI_C55X_REV2);
		if (symp) {
			DL_ERROR("CPU rev 2 required, but %s forbids rev 2", filename);
			return true;
		}
	}

	return false;
}

/*****************************************************************************
 * Procedure chk_memmodel_mix
 *
 * Parameters:
 *	sname: Name of absolute symbol
 *
 * Effect:
 *  Given an old memory model symbol definition and new memory model
 *  symbol definition, evaluate whether the memory models are equal.  If they
 *  aren't, produce error message and return true.  If they are, return
 *  false.
 *
 *****************************************************************************/
static boolean chk_memmodel_mix(struct dload_state *dlthis, 
				const struct doff_syment_t *sym, 
					const struct dynload_symbol *defSym, const char *sname)
{
	char *filename;
	const char *mem_model_names[] = { "?", "small", "large", "?"	/* 0-3 */
		    "reserved", "?", "?", "?",	/* 4-7 */
		"huge"		/*  8  */
	};

	if (DL_STRCMP(sname, TI_C55X_MEM_MODEL) == 0) {

#if BITS_PER_AU > BITS_PER_BYTE
		filename = unpack_name(dlthis, 0);
#else
		filename = dlthis->str_head + BYTE_TO_HOST(0);
#endif

		if (defSym->value != sym->dn_value) {
			dload_error(dlthis, "%s is %s model, but CPU defines %s model",
									filename, mem_model_names[sym->dn_value],
												mem_model_names[defSym->value]);
			return true;
		}
	}

	return false;
}
#endif

