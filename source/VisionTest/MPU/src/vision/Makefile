#
#  Copyright 2001-2008 Texas Instruments - http://www.ti.com/
# 
#  Licensed under the Apache License, Version 2.0 (the "License");
#  you may not use this file except in compliance with the License.
#  You may obtain a copy of the License at
# 
#     http://www.apache.org/licenses/LICENSE-2.0
# 
#  Unless required by applicable law or agreed to in writing, software
#  distributed under the License is distributed on an "AS IS" BASIS,
#  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#  See the License for the specific language governing permissions and
#  limitations under the License.
#


PROJROOT=../..

include $(PROJROOT)/make/start.mk

INCLUDE=-I $(PROJROOT)/inc
LDPATH=$(TARGETDIR)/lib/ 
LDFLAGS = -L $(LDPATH)
CFLAGS=-Wall -g -O2 $(INCLUDE) -finline-functions -D$(PROCFAMILY) $(LDFLAGS)

LIBS = -lm -lbridge -lpthread -lrt -static

all: vision.out photos.out

vision.out: dsp.h dsp.c main.c opticFlowHist.c
	$(CC) $(CFLAGS) -std=gnu99 -o vision.out main.c dsp.c opticFlowHist.c encoding/jpeg.c encoding/rtp.c udp/socket.c v4l/video.c $(LIBS)
	
photos.out: Photos.c
	$(CC) $(CFLAGS) -std=gnu99 -o photos.out Photos.c v4l/video.c $(LIBS)


install:
	$(INSTALL) -D vision.out $(TARGETDIR)/MPU/vision.out
	$(STRIP) -s $(TARGETDIR)/MPU/vision.out
	$(INSTALL) -D photos.out $(TARGETDIR)/MPU/photos.out
	$(STRIP) -s $(TARGETDIR)/MPU/photos.out

clean:
	\rm -f vision.out
	\rm -f $(TARGETDIR)/MPU/vision.out

