/*
 * This is a little header file which doxygen parses to generate the main
 * documentation page
 */

/**
 * @mainpage DSP Bridge DSP-side API Reference
 *
 * @section naming_conventions Modules and Naming Conventions
 *
 * The DSP-side API is partitioned into two hierarchical modules:
 * - @ref DSPNODE "Node Module" - Provides node messaging and wait functions.
 * - @ref DSPSTRM "Stream Module" - Provides data streaming functions.
 *
 * @note Each module has a prefix identifying it, and this prefix is
 * used in the name of each function in that module. The prefixes are:
 * NODE_ for the Node Module, and STRM_ for the Stream Module
 *
 * Additionally, DSP Bridge provides the following API modules to support
 * XDAIS algorithms, power management, cache management, Resource Manager
 * Server, and Design By Contract software methodology:
 *
 * - XDAIS:
 *   - @ref DSPACPY2_OMAP "ACPY2 OMAP Extension (C55x)" - Provides
 *     ACPY2 add-on utilities to  manage system DMA resources. 
 *   - @ref DSPDMAN "DMAN (C55x)" - Application interface to the DMA
 *     Manager.  Grants DMA to XDAIS algorithms. 
 *   - @ref DSPDSKT2 "DSKT2" (C55x) - XDAIS Socket Library for XDAIS 
 *     algorithms. 
 *   - For C64x+ devices (2430, 3430) DMAN3, ACPY3 and DSKT2 are 
 *     packaged in a stand-alone product independently of DSP Bridge:
 *     Framework Components. "FC" is delivered along with DSP Bridge 
 *     release as a TAR file in the "bdsptools" folder. "FC" contains
 *     its own documentation and examples.
 *
 * - @ref DSPBPWR "Power Management (BPWR)" - Provides DSP clock and
 *     are specified properlypower resource management.
 *
 * - BCACHE APIs have been removed from DSP Bridge and are provided by 
 *     DSP BIOS since DSP BIOS 5.21.
 *
 * - Resource Manager Server (RMS):
 *   - @ref DSPRMSSH "RMS Shared Header" - Defintion of RMS objects
 *     and definitions shared between GPP and DSP
 *   - @ref DSPRMS "External Header" - List of all supported RMS
 *     services
 *   - @ref DSPRMSTYPES "Type defintion" - RMS types and objects used
 *     on the DSP
 *   - @ref DSPRMSDEFS "RMS error codes" - RMS related defines for errors
 *   - @ref DSPUSR "USR functions" - Default SYS handler for error, abort and
 *     putchar
 *
 * - @ref DSPDBC "Design By Contract" - Provides basic Design By
 *     Contract support,including asserts, preconditions, and
 *     postconditions. DSP Bridge uses DBC macros extensively to check
 *     if API parameters are specified properly.
 * 
 * @section data_type_names Data Type Names
 *
 * The following table summarizes the data types used in the DSP-side API
 * definitions.  Instead of using fundamental C-type definitions, (such as int
 * or char), a set of portable types are defined.
 *
 * <table align="center" border="1" cellpadding="3" cellspacing="1">
 * <tr>
 *   <th>Type</th>
 *   <th>Description</th>
 * </tr>
 * <tr>
 *   <td>Arg</td>
 *   <td>Type capable of holding both Ptr and Int arguments</td>
 * </tr>
 * <tr>
 *   <td>Bool</td>
 *   <td>Boolean value</td>
 * </tr>
 * <tr>
 *   <td>Char</td>
 *   <td>Character value</td>
 * </tr>
 * <tr>
 *   <td>Int</td>
 *   <td>Signed integer value</td>
 * </tr>
 * <tr>
 *   <td>LgUns</td>
 *   <td>Unsigned 32-bit integer value</td>
 * </tr>
 * <tr>
 *   <td>Ptr</td>
 *   <td>Generic pointer value</td>
 * </tr>
 * <tr>
 *   <td>String</td>
 *   <td>Zero-terminated array of characters</td>
 * </tr>
 * <tr>
 *   <td>Uns</td>
 *   <td>Unsigned integer value</td>
 * </tr>
 * <tr>
 *   <td>Void</td>
 *   <td>Empty type</td>
 * </tr>
 * </table>
 */

