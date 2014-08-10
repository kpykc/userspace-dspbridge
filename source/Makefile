#
#  Copyright 2001-2008 Texas Instruments - http://www.ti.com/
#
#  Licensed under the Apache License, Version 2.0 (the "License");
#  you may not use this file except in compliance with the License.
#  You may obtain a copy of the License at
#
#  http://www.apache.org/licenses/LICENSE-2.0
#
#  Unless required by applicable law or agreed to in writing, software
#  distributed under the License is distributed on an "AS IS" BASIS,
#  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#  See the License for the specific language governing permissions and
#  limitations under the License.
#

MAKE = make
RM = rm
CP = cp
MKDIR = mkdir

ifndef PREFIX
PREFIX = $(shell pwd)
endif
ifndef TDIR
TDIR = target
endif

TARGETDIR=$(PREFIX)/$(TDIR)

ifndef DDIR
TARGETDSPDIR = $(TARGETDIR)/dspbridge
else
TARGETDSPDIR = $(PREFIX)/$(DDIR)
endif

DEPOT ?= ~/tidsp
CROSS_COMPILE=arm-linux-gnueabi-

# we dont support parallel builds yet - force single CPU build
# TODO: we need to define proper dependencies in *all* make files
# before we enable this flag
MAKEFLAGS += -j 1

PREBUILT_DSPBINARY_DIR ?= $(DEPOT)/userspace-dspbridge/source/samples/dsp/
PREBUILT_DSPBINARY_FILES ?= \
		      ddspbase_tiomap3430.dof64P	\
		      dmmcopydyn_3430.dll64P		\
		      dmmcopyPhasedyn_3430.dll64P	\
		      dynbase_tiomap3430.dof64P		\
		      loaddyn_3430.dll64P		\
		      monitor_tiomap3430.dof64P		\
		      pingdyn_3430.dll64P		\
		      pingPhasedyn_3430.dll64P		\
		      scaledyn_3430.dll64P		\
		      scalePhasedyn_3430.dll64P		\
		      strmcopydyn_3430.dll64P		\
		      strmcopyPhasedyn_3430.dll64P	\
		      zcmsgdyn_3430.dll64P		\
		      zcmsgPhasedyn_3430.dll64P


.EXPORT_ALL_VARIABLES:
API_DIR := mpu_api/src
MPU_SAMPLE_DIR := samples/mpu/src


SUBDIRS :=  $(API_DIR) $(MPU_SAMPLE_DIR)
SUBDIRS_CLEAN := $(patsubst %, %-c, $(SUBDIRS))
SUBDIRS_INSTALL := $(patsubst %, %-i, $(SUBDIRS))

.PHONY: $(SUBDIRS)

all: $(SUBDIRS_INSTALL) predsp
	@echo INSTALL: abc $(SUBDIRS_ALL)  cln$(SUBDIRS_CLEAN) inst$(SUBDIRS_INSTALL) sdr$(SUBDIRS)

clean: $(SUBDIRS_CLEAN)
	$(MAKE) -f samplemakefile .clean
	if test -d $(TARGETDSPDIR); then	\
		$(RM) -rf $(TARGETDSPDIR);	\
	fi
	@echo clean: abc $(SUBDIRS_ALL)  cln$(SUBDIRS_CLEAN) inst$(SUBDIRS_INSTALL) sdr$(SUBDIRS)

mpu_build: $(patsubst %, %-i, $(API_DIR))

dsp_build: .dsp


.samples-install: $(patsubst %, %-i, $(MPU_SAMPLE_DIR))

.samples: .samples-install

%-c:
	$(MAKE) -C $(patsubst %-c,%,$@) clean
	@echo DONE $@-c clean

%-i:
	$(MAKE) -C $(patsubst %-i,%,$@) all install
	@echo DONE $@-i install

.dsp:
	$(MAKE) -f samplemakefile .clean .bridge_samples .install
	@echo "DONE dsp"
