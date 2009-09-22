#
#  ======== product.mak ========
#
#  This file is included by all makefiles.
#


# ---- Copyright/Version Generation ----
#


# ---- Package Dependencies ----
#

# DSP BIOS
SABIOS_VER   = 5.33.04
SABIOS_VER_2 = 5_33_04

# CodeGen Tools
CGT55_VER = 3.2.2
CGT6X_VER = 6.0.7

# Framework components
FC_VER = 1_10_04

# ---- Build targets ----
#

# define default targets that we need to build for
# this variable can be overriden from the command line to build only a subset
# of the targets. 
XDCTARGETS = C64P

#define default tests for the Bridge DSP side test suite build
TESTBUILDTYPE = ALL

#define default Framework components package availability
FC_PACKAGE = TRUE

#define Load Monitor module
L_MONITOR = TRUE

#define DVFS module
SCALING = TRUE

#define INST2 module
INST2 = FALSE