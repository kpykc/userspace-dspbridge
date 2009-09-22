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
 *  ======== dsptoolExecute.c ========
 *
 *  DSP/BIOS Bridge Task Node that gets DSP debug info tool.
 *
 */

#include <std.h>
#include <tsk.h>
#include <que.h>
#include <obj.h>

#include <sys.h>
#include <dbc.h>      /* Design by Contract assertions                        */
#include <rmsdefs.h>  /* DSP/BIOS Bridge Resource Mangager Server definitions */
#include <rms_sh.h>   /* RMS definitions shared on both GPP and DSP           */
#include <node.h>     /* DSP/BIOS Bridge Node APIs                            */
#include <shm.h>

#include <pwrm.h>
 
#include "dsptooldata.h"

#define TRACE &trace

#define  DSPTOOLMEMSEG   0    /* Segment used as default DSP/BIOS bridge heap    */
#define DEBUG           0x01
#define TSKLIST			0x02
#define PRCMDUMP  		0x03
#define LOADMON			0x04

#define PRM_IVA2_BASE_ADDR          0x1181E000
#define PM_PWSTCTRL_IVA2_OFFSET     0xE0
#define PM_PWSTST_IVA2_OFFSET       0xE4 
#define PM_PWSTCTRL_IVA2            (PRM_IVA2_BASE_ADDR + PM_PWSTCTRL_IVA2_OFFSET)
#define PM_PWSTST_IVA2              (PRM_IVA2_BASE_ADDR + PM_PWSTST_IVA2_OFFSET)

#define REG32(A)    (*(volatile Uint32*)(A))
 
 
/* Definition needed to retrieve task list: */
extern QUE_Obj OBJ_table[OBJ_MODULES];

/* Definition needed to retrieve the load monitoring info */
extern far loadMonStruct *loadMonValues;
/*
 *  ======== DSPTOOL_TI_Obj ========
 *  DSPTOOL task node context.
 */
typedef struct DSPTOOL_TI_Obj {
   Uns instance;      /* Instance number of this DSPTOOL node.                   */
} DSPTOOL_TI_Obj;


/*
 *  ======== DSPTOOL_TI_execute ========
 *  Requires:  node's moreEnv pointer is set.
 */
RMS_STATUS DSPTOOL_TI_execute(NODE_EnvPtr node)
{
    RMS_DSPMSG   msg;
    Uns          instance;
    Bool         status;
	
	TSK_Handle task;
	OBJ_Header *pobj;
	TSK_Stat stat;
	String taskName;
	Int priority;
	char *pwstctrl="";
	char *pwstst="";
	char *tskstatus[4] =
		{
			"RUNNING",
			"READY ",
			"BLOCKED",
			"TERMINATED",
        };
	pobj = (OBJ_Header *)OBJ_table[OBJ_TSK].next;
    task = (TSK_Handle)(pobj + 1);

    /* Assert Requires */
    DBC_require(node->moreEnv != NULL);
    msg.cmd = ~(RMS_EXIT);  /* Initialize */

    /* Extract attributes from node context object: */
    instance = ((DSPTOOL_TI_Obj *)node->moreEnv)->instance;
    for (;;) {
		/* Get message. */
		status= NODE_getMsg(node, &msg, NODE_FOREVER);
		/* Handle the message */
	    if (msg.cmd == TSKLIST){
			SYS_printf("\n\t DSP TASK LIST");
			SYS_printf("\n---------------------------------------------------------------------------");
			SYS_printf("\n|      Task Name          |Priority| Stack Addr |  Alloc  |  Used   |  STATUS |");
			SYS_printf("\n---------------------------------------------------------------------------");
			pobj = (OBJ_Header *)OBJ_table[OBJ_TSK].next;
			while (pobj != (OBJ_Header *)&OBJ_table[OBJ_TSK])
			{
				task = (TSK_Handle)(pobj + 1);
				taskName = TSK_getname(task);
				priority = TSK_getpri(task);
				TSK_stat(task, &stat);
				pobj = (OBJ_Header *)pobj->link.next;
				SYS_printf("\n|0x%8x %14s|   %2d   | 0x%8x | 0x%5x | 0x%5x | %s",
					task,taskName,priority,stat.sp,stat.attrs.stacksize,stat.used,tskstatus[stat.mode]);  
			}
			SYS_printf("\n---------------------------------------------------------------------------\n");
			break;
		}else if (msg.cmd == PRCMDUMP){
			switch (REG32(PM_PWSTCTRL_IVA2) & 0x3)
			{
				case 0x3: pwstctrl = "ON"; break;
				case 0x1: pwstctrl = "RETENTION"; break;
				case 0x0: pwstctrl = "OFF"; break;
				default: break;
			}
			switch (REG32(PM_PWSTST_IVA2) & 0x3)
			{
				case 0x3: pwstst = "Power domain is ON"; break;
				case 0x2: pwstst = "Power domain is INACTIVE"; break;
				case 0x1: pwstst = "Power domain is RETENTION"; break;
				case 0x0: pwstst = "Power domain is OFF"; break;
				default: break;
			}
			
			SYS_printf("\n\tPRCM registers status");
			SYS_printf("\n---------------------------------------------------------------------------");
			SYS_printf("\nPM_PWSTCTRL_IVA2 0x%x, %s",REG32(PM_PWSTCTRL_IVA2),pwstctrl);
			SYS_printf("\nPM_PWSTST_IVA2   0x%x, %s",REG32(PM_PWSTST_IVA2),pwstst);
			SYS_printf("\n---------------------------------------------------------------------------\n");
			break;
		}else if (msg.cmd == LOADMON){
			SYS_printf("\n\tLoad Monitoring info");
			SYS_printf("\n---------------------------------------------------------------------------");
			SYS_printf("\nCurrent CPULoad %d", loadMonValues->currDspLoad);
			SYS_printf("\nPredicted CPULoad %d\n",loadMonValues->predDspLoad);
			SYS_printf("---------------------------------------------------------------------------\n");
			break;
		}else if (msg.cmd == RMS_EXIT) {
			SYS_printf("RMS_EXIT\n");
			break;
		}else {
			SYS_printf("Unknown command\n");
			break;
		} 
		
	}
	if (status){
		msg.cmd  = RMS_USER + instance;   /* Node instance number */
		/* Send message, *without* blocking until message queue is ready.   */
		status = NODE_putMsg(node, NODE_TOGPP, &msg, 0);
        if (!status) SYS_printf("DSPTOOL Message queue not emptied \n" );
	}
    return (RMS_EOK);
}
