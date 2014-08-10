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

DEPOT ?= ~/tidsp
CROSS_COMPILE=arm-linux-gnueabi-

# we dont support parallel builds yet - force single CPU build
# TODO: we need to define proper dependencies in *all* make files
# before we enable this flag
MAKEFLAGS += -j 1


.EXPORT_ALL_VARIABLES:
API_DIR := MPU/src

SUBDIRS :=  $(API_DIR)
SUBDIRS_CLEAN := $(patsubst %, %-c, $(SUBDIRS))
SUBDIRS_INSTALL := $(patsubst %, %-i, $(SUBDIRS))


all: .mpu .dsp
	@echo Upload to ARDrone2
	./upload.py dsp

clean: $(SUBDIRS_CLEAN)
	$(MAKE) -f samplemakefile .clean
	if test -d $(TARGETDSPDIR); then	\
		$(RM) -rf $(TARGETDSPDIR);	\
	fi

.mpu: $(patsubst %, %-i, $(API_DIR))

.dsp: 
	$(MAKE) -f samplemakefile .all

%-c:
	$(MAKE) -C $(patsubst %-c,%,$@) clean
	@echo DONE $@-c clean

%-i:
	$(MAKE) -C $(patsubst %-i,%,$@) all install
	@echo DONE $@-i install
:
	
