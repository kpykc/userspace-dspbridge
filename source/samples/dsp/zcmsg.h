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

/*
 *  ======== zcmsg.h =========
 *  Sample DSP program to zcmsg buffers supplied from a host input channel back
 *  into a host output.  Demonstrates end-of-stream processing and flushing of
 *  the output channel on shutdown.
 */
/*  Usage:  zcmsg
 *
 *! Revision History
 *! ================
 *! 12-Dec-2005 connell  Created from zcmsg.c
 */

#include <stdlib.h>
#include <log.h>
#include <node.h>

/* ZCMSG task node context structure */
typedef struct {
    Ptr         msgBuf;         /* Buffer used for zero-copy messaging */
    Int         bufSize;        /* Buffer size */
} ZCMSG_TI_Obj;


#define TRACE &trace
extern LOG_Obj trace;
