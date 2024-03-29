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

#if TMS32060
static const char BSSSYMBOL[] = {".bss"};					
/* the magic symbol for the start of BSS */
#endif

#if LEAD3
#include "reloc_table_c5500.c"
#elif LEAD
#include "reloc_table_c5400.c"
#elif TMS32060
#include "reloc_table_c6000.c"
#elif TMS470
#include "reloc_table_arm.c"
#endif

#if TMS32060
// From coff.h - ignore these relocation operations
#define R_C60ALIGN     0x76       /* C60: Alignment info for compressor   */
#define R_C60FPHEAD    0x77       /* C60: Explicit assembly directive     */
#define R_C60NOCMP    0x100       /* C60: Don't compress this code scn    */
#endif

/*****************************************************************************
 * Procedure dload_unpack
 *
 * Parameters:
 *	data	pointer to storage unit containing lowest host address of image data
 *	fieldsz	Size of bit field, 0 < fieldsz <= sizeof(RVALUE)*BITS_PER_AU
 *	offset	Offset from LSB, 0 <= offset < BITS_PER_AU
 *	sgn		Signedness of the field (ROP_SGN, ROP_UNS, ROP_MAX, ROP_ANY)
 *
 * Effect:
 *	Extracts the specified field and returns it.
 *****************************************************************************/
RVALUE dload_unpack(struct dload_state *dlthis, TgtAU_t *data, int fieldsz, 
														int offset,unsigned sgn)
{
	register RVALUE objval;
	register int shift, direction;
	register TgtAU_t *dp = data;

	fieldsz -= 1;	// avoid nastiness with 32-bit shift of 32-bit value

	/*
	 * collect up enough bits to contain the desired field
	 */
	if (TARGET_BIG_ENDIAN) {
		dp += (fieldsz + offset) >> LOG_TGTAU_BITS;
		direction = -1;
	}
	else direction = 1;

	objval = *dp >> offset; shift = TGTAU_BITS - offset;
	while (shift <= fieldsz) {
		dp += direction;
		objval += (RVALUE)*dp << shift; shift += TGTAU_BITS;
	}

	/*
	 * sign or zero extend the value appropriately
	 */
	if (sgn == ROP_UNS)
		objval &= (2 << fieldsz) -1;
	else {
		shift = sizeof(RVALUE)*BITS_PER_AU-1 - fieldsz;
		objval = (objval << shift) >> shift;
	}

	return objval;

} /* dload_unpack */


/*****************************************************************************
 * Procedure dload_repack
 *
 * Parameters:
 *	val		Value to insert
 *	data	Pointer to storage unit containing lowest host address of image data
 *	fieldsz	Size of bit field, 0 < fieldsz <= sizeof(RVALUE)*BITS_PER_AU
 *	offset	Offset from LSB, 0 <= offset < BITS_PER_AU
 *	sgn		Signedness of the field (ROP_SGN, ROP_UNS, ROP_MAX, ROP_ANY)
 *
 * Effect:
 *	Stuffs the specified value in the specified field.  Returns 0 for success
 * or 1 if the value will not fit in the specified field according to the
 * specified signedness rule.
 *****************************************************************************/
static const unsigned char ovf_limit[] = {1, 2, 2};
int dload_repack(struct dload_state *dlthis, RVALUE val, TgtAU_t *data,
											int fieldsz,int offset,unsigned sgn)
{
	register URVALUE objval, mask;
	register int shift, direction;
	register TgtAU_t *dp = data;


	fieldsz -= 1;	// avoid nastiness with 32-bit shift of 32-bit value
	mask = ((UINT32_C(2) << fieldsz) -1); objval = (val & mask);
	/* clip the bits */

	/*
	 * store the bits through the specified mask
	 */
	if (TARGET_BIG_ENDIAN) {
		dp += (fieldsz + offset) >> LOG_TGTAU_BITS;
		direction = -1;
	}
	else direction = 1;

	*dp = (*dp & ~(mask << offset)) + (objval << offset);	/* insert LSBs */
	shift = TGTAU_BITS-offset;	 objval >>= shift; mask >>= shift;
	/* align mask and objval with AU boundary */

	while (mask) {
		dp += direction;
		*dp = (*dp & ~mask) + objval;
		objval >>= TGTAU_BITS; mask >>= TGTAU_BITS;
	}

	/*
	 * check for overflow
	 */
	if (sgn) {
		unsigned tmp = (val >> fieldsz) + (sgn & 0x1);
		if (tmp > ovf_limit[sgn-1])
			return 1;
	}
	return 0;

} /* dload_repack */

/* lookup table for the scaling amount in a C6x instruction */
#if TMS32060
#define SCALE_BITS 4		/* there are 4 bits in the scale field */
#define SCALE_MASK 0x7		/* we really only use the bottom 3 bits */
static const uint_least8_t C60_Scale[SCALE_MASK+1] = {
	1, 0, 0, 0, 1, 1, 2, 2
};
#endif

/*****************************************************************************
 * Procedure dload_relocate
 *
 * Parameters:
 *	data	Pointer to base of image data
 *	rp		Pointer to relocation operation
 *
 * Effect:
 *	Performs the specified relocation operation
 *****************************************************************************/
void dload_relocate(struct dload_state *dlthis, TgtAU_t *data, 
													struct reloc_record_t *rp)
{
	RVALUE val, reloc_amt;
	unsigned fieldsz = 0, offset = 0, reloc_info, reloc_action;
	register int rx;
	RVALUE    *stackp = NULL;
	int top;
	struct Local_Symbol *svp=NULL;
#ifdef RFV_SCALE
	unsigned scale;
#endif

	rx = HASH_FUNC(rp->r_type);
	while ( rop_map1[rx] != rp->r_type) {
		if ((rx = HASH_L(rop_map2[rx])) < 0) {
#if TMS32060
            switch (rp->r_type) {
                case R_C60ALIGN:
                case R_C60NOCMP:
                case R_C60FPHEAD:
                    /* Ignore these reloc types and return */
                    break;
                default:
                    /* Unknown reloc type, print error and return */
                    dload_error(dlthis, "Bad coff operator 0x%x", rp->r_type);
            }
#else
            dload_error(dlthis, "Bad coff operator 0x%x", rp->r_type);
#endif
            return;
		}
	}
	rx = HASH_I(rop_map2[rx]);
	if((rx < (sizeof(rop_action)/sizeof(uint_least16_t))) 
			&& (rx<(sizeof(rop_info)/sizeof(uint_least16_t)))){
	reloc_action = rop_action[rx]; reloc_info = rop_info[rx];
	}else{
		dload_error(dlthis, "Buffer Overflow - Array Index Out of Bounds");
	}

	/*
	 * Compute the relocation amount for the referenced symbol, if any
	 */
	reloc_amt = rp->r_uval;
	if (RFV_SYM(reloc_info)) {	/* relocation uses a symbol reference */
		if ((uint32_t)rp->r_symndx < dlthis->dfile_hdr.df_no_syms) {	
			/* real symbol reference */
			svp = &dlthis->local_symtab[rp->r_symndx];
			reloc_amt = (RFV_SYM(reloc_info) == ROP_SYMD) ? svp->delta :
																	svp->value;
		}
		else if (rp->r_symndx == -1)	/* reloc references current section */
			reloc_amt = (RFV_SYM(reloc_info) == ROP_SYMD) ? 
						dlthis->delta_runaddr : dlthis->image_secn->run_addr;
	}	/* relocation uses a symbol reference */

	/*
	 * Handle stack adjustment
	 */
	val = 0;
	top = RFV_STK(reloc_info);
	if (top) {

		top += dlthis->relstkidx - RSTK_UOP;
		if (top >= STATIC_EXPR_STK_SIZE) {
			dload_error(dlthis, "Expression stack overflow in %s at\
								offset " FMT_UI32, dlthis->image_secn->name,
											rp->r_vaddr + dlthis->image_offset);
			return;
		}
		val = dlthis->relstk[dlthis->relstkidx];
		dlthis->relstkidx = top; stackp = &dlthis->relstk[top];
	}

	/*
	 * Derive field position and size, if we need them
	 */
	if (reloc_info & ROP_RW) {	/* read or write action in our future */

		fieldsz = RFV_WIDTH(reloc_action);
		if (fieldsz) {	/* field info from table */
			offset = RFV_POSN(reloc_action);
#if !LEAD3
			if (TARGET_BIG_ENDIAN)
				/* make sure r_vaddr is the lowest target 
				 * address containing bits */
				rp->r_vaddr += RFV_BIGOFF(reloc_info);
#endif
		}
		else {	/* field info from relocation op */
			fieldsz = rp->r_fieldsz; offset = rp->r_offset;
			if (TARGET_BIG_ENDIAN)
				/* make sure r_vaddr is the lowest target
				 * address containing bits */
				rp->r_vaddr += (rp->r_wordsz - offset - fieldsz) >>
															LOG_TARGET_AU_BITS;
		}

		data = (TgtAU_t *)((char *)data + TADDR_TO_HOST(rp->r_vaddr));	
		/* compute lowest host location of referenced data */

#if BITS_PER_AU > TARGET_AU_BITS
		/* conversion from target address to host address may lose 
		 * address bits; add loss to offset */
		if (TARGET_BIG_ENDIAN) {
			offset += -((rp->r_vaddr << LOG_TARGET_AU_BITS)+offset+fieldsz) & 
												(BITS_PER_AU-TARGET_AU_BITS);
		}
		else offset += (rp->r_vaddr << LOG_TARGET_AU_BITS) & (BITS_PER_AU-1);
#endif

#ifdef RFV_SCALE
		scale = RFV_SCALE(reloc_info);
#endif
	}

	/*
	 * read the object value from the current image, if so ordered
	 */
	if (reloc_info & ROP_R) {	/* relocation reads current image value */
		val = dload_unpack(dlthis, data, fieldsz, offset, RFV_SIGN(reloc_info));
#ifdef RFV_SCALE
		val <<= scale;
#endif
	}


	/*
	 * perform the necessary arithmetic
	 */
	switch( RFV_ACTION(reloc_action) ) {	/* relocation actions */
	case RACT_VAL:
		break;

	case RACT_ASGN:
		val = reloc_amt;
		break;

	case RACT_ADD:
		val += reloc_amt;
		break;

	case RACT_PCR:
		/*-----------------------------------------------------------------*/
		/* Handle special cases of jumping from absolute sections (special */
		/* reloc type) or to absolute destination (symndx == -1).  In      */
		/* either case, set the appropriate relocation amount to 0.        */
		/*-----------------------------------------------------------------*/
		if( rp->r_symndx == -1 ) reloc_amt = 0;
		val += reloc_amt - dlthis->delta_runaddr;
		break;

	case RACT_ADDISP:
		val += rp->r_disp + reloc_amt;
		break;

	case RACT_ASGPC:
		val = dlthis->image_secn->run_addr + reloc_amt;
		break;

	case RACT_PLUS:
		if(stackp!=NULL){
		val += *stackp; 
		}
		break;
	case RACT_SUB:
		if(stackp!=NULL){
		val = *stackp - val;
		}
		break;
	case RACT_NEG:
		if(stackp!=NULL){
		val = -val;
		}
		break;

	case RACT_MPY:
		if(stackp!=NULL){
		val *= *stackp;
		}
		break;
	case RACT_DIV:
		if(stackp!=NULL){
		val = *stackp / val;
		}
		break;
	case RACT_MOD:
		if(stackp!=NULL){
		val = *stackp % val;
		}
		break;

	case RACT_SR:
		if (val >= sizeof(RVALUE)*BITS_PER_AU){
		val = 0;
		}else if(stackp!=NULL){
		val = (URVALUE)*stackp >> val;
		}
		break;
	case RACT_ASR:
		if (val >= sizeof(RVALUE)*BITS_PER_AU){
			val = sizeof(RVALUE)*BITS_PER_AU -1;
		}else if(stackp!=NULL){
		val = *stackp >> val;
		}
		break;

	case RACT_SL:
		if (val >= sizeof(RVALUE)*BITS_PER_AU){
		val = 0;
		}else if(stackp!=NULL){
		val = *stackp << val;
		}
		break;

	case RACT_AND:
		if(stackp!=NULL){
		val &= *stackp;
		}
		break;
	case RACT_OR:
		if(stackp!=NULL){
		val |= *stackp;
		}
		break;
	case RACT_XOR:
		if(stackp!=NULL){
		val ^= *stackp;
		}
		break;
	case RACT_NOT:
		val = ~val;
		break;

#if TMS32060
	case RACT_C6SECT:
		if (rp->r_symndx >= 0)	
			/* actually needed address of secn containing symbol */
		if(svp!=NULL){
			if (svp->secnn > 0)
				reloc_amt = dlthis->ldr_sections[svp->secnn-1].run_addr;
			}
		/* !!! FALL THRU !!! */
	case RACT_C6BASE:
		if (dlthis->bss_run_base == 0) {
			struct dynload_symbol *symp;

			/* lookup value of global BSS base */
			if ((symp = dlthis->mysym->Find_Matching_Symbol(dlthis->mysym,
																	BSSSYMBOL)))
				dlthis->bss_run_base = symp->value;
			else
				dload_error(dlthis, "Global BSS base referenced in %s \
									offset " FMT_UI32 " but not defined",
												dlthis->image_secn->name, 
											rp->r_vaddr + dlthis->image_offset);
		}
		reloc_amt -= dlthis->bss_run_base;
		/* !!! FALL THRU !!! */
	case RACT_C6DSPL:
		scale = C60_Scale[val & SCALE_MASK];		
		/* scale factor determined by 3 LSBs of field */
		offset += SCALE_BITS; fieldsz -= SCALE_BITS;
		val >>= SCALE_BITS;	/* ignore the scale field hereafter */
		val <<= scale;
		val += reloc_amt;				/* do the usual relocation */
		if (((1 << scale)-1) & val)
			dload_error(dlthis, "Unaligned reference in %s\
								offset " FMT_UI32, dlthis->image_secn->name,
											rp->r_vaddr + dlthis->image_offset);
		break;
#endif
#if TMS470
	case RACT_PCR23T:
		/*
		 * ARM encodes Thumb long-branch as two 16-bit branches, first 
		 * containing 11 MSBs,
		 * second containing 11 LSBs of a 23-bit short-aligned field
		 */
		if( rp->r_symndx == -1 ) reloc_amt = 0;
		val += dload_unpack(dlthis, data + sizeof(uint16_t)/sizeof(TgtAU_t), 
										11, 0, ROP_UNS) << 1;	/* grab LSBs */
		val += reloc_amt - dlthis->delta_runaddr;
		dload_repack(dlthis, val >> 1, data + sizeof(uint16_t)/sizeof(TgtAU_t),
											11, 0, ROP_UNS);	/* store LSBs */
		break;
#endif
	}	/* relocation actions */


	/*
	 * Put back result as required
	 */

	if (reloc_info & ROP_W) {	/* relocation writes image value */
#ifdef RFV_SCALE
		val >>= scale;
#endif
		if (dload_repack(dlthis, val, data, fieldsz, offset,
														RFV_SIGN(reloc_info))) {
			dload_error(dlthis, "Relocation value " FMT_UI32 " overflows %d\
							bits in %s offset " FMT_UI32, val, fieldsz,
											dlthis->image_secn->name, 
											dlthis->image_offset + rp->r_vaddr);
		}
	}
	else if (top)
		*stackp = val;

} /* reloc_value */

