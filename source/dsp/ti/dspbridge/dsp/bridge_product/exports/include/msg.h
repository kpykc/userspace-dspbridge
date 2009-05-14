/*
 *! ============================================================
 *! Copyright 2004 by Texas Instruments Incorporated.
 *! All rights reserved. Property of Texas Instruments Incorporated.
 *! Restricted rights to use, duplicate or disclose this code are
 *! granted through contract.
 *! ============================================================
 */
/*
 *  ======== msg.h ========
 *
 *  RM Server Messenger definitions.
 *
 */

#ifndef MSG_
#define MSG_

#include <que.h>
#include <sem.h>
#include <swi.h>
#include <rmstypes.h>
#include <msg_sh.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Pre-defined mailbox bit locations for Messenger SWIs: */
#define MSG_WORKNEEDED	0x0001  /* WORK_NEEDED bit in mailbox of both SWIs */
#define MSG_BUFFEREMPTY	0x0002  /* BUFFER_EMPTY bit in fromDSP SWI's mailbox */
#define MSG_ENABLE	0x0004  /* ENABLE bit in mailbox of both SWIs */

/* Pre-defined (manifest constant) NODE_EnvPtr when message source is GPP: */
#define MSG_FROMGPP	0

/* Default memory segment for allocating message frames: */
#define MSG_FRAMESEG	0	/* segment used for message frames */

/*
 *  The MSG_Obj structure is used for passing short, fixed-length messages to
 *  a node.
 *
 *  Inbound messages are contained within frames, and placed in 'queue'.
 *  'count' tracks the number of messages in the queue.  Buffer descriptors
 *  can be passed as messages, and the 'segid' field in MSG_Obj defines the
 *  memory segment to be used for these buffers.  The node can be notified of
 *  the arrival of a new message; 'notifyType' defines the type of
 *  notification, and 'notifyHandle' defines a handle to be used for the
 *  notification.  The last field, 'status', maintains node status information,
 *  including whether or not RMS_createNode ran the node's create-phase
 *  function, and the number of message frames actually allocated for the node,
 *  by RMS_createNode.
 */
typedef struct {
    QUE_Obj	queue;		  /* queue to hold message frames */
    volatile Uns count;	  /* number of messages in queue */
    Uns 	segid;		  /* segid for NODE_allocMsgBuf/NODE_freeMsgBuf */
    Uns		notifyType;	  /* type of notification when message arrives */
    Ptr		notifyHandle; /* handle for notification */
    Uns		status;		  /* node create status: currently the left-
                                 * most bit indicates if RMS_createNode ran
                                 * the node's create function, and the
                                 * remaining bits indicate the number of
                                 * message frames alloc'd for the node */
} MSG_Obj, *MSG_Handle;


/*
 *  The MSG_Msg structure holds a message's information content, plus a field
 *  for routing information.  MSG_Msg is an integral part of a MSG_Frame, and
 *  is also used to pass messages in the shared memory windows between the GPP
 *  and the RM Server Messenger SWIs.
 *
 *  A message's information content is passed via three fields: 'cmd', 'arg1',
 *  and 'arg2', contained within a RMS_DSPMSG structure.  The last element of
 *  the MSG_Msg structure, 'nodeEnv', is used for routing:
 *
 *  1) For messages transferred between the GPP to the Messenger SWIs, this
 *  field contains the node environment pointer, (the node's MSG_Obj
 *  pointer), that is the destination for GPP->DSP bound messages, or the
 *  source of DSP->GPP bound messages.
 *
 *  2) For messages posted via the (DSP-side API function) NODE_putMsg, (bound
 *  for either the GPP, or another node), nodeEnv carries the NODE_EnvPtr of the
 *  node that sourced the message.
 */
typedef struct {
    RMS_DSPMSG msg;		/* message's information content */
    RMS_WORD nodeEnv;		/* message destination or source identifier */
} MSG_Msg;


/*
 *  The MSG_Frame structure is used for moving messages between individual
 *  nodes, and between the nodes and the Messenger SWIs.
 *
 *  A 'link' field allows the message frames to be routed via queues.
 *  The MSG_Msg structure contains the information content, and routing info.
 */
typedef struct {
    QUE_Elem	link;		/* link allowing frames to be queued */
    MSG_Msg	msg;		/* message content plus routing info */
} MSG_Frame;


/*
 *  MSG_MemWindow defines the data structure used for passing a buffer of
 *  messages between the GPP and the Messenger's SWIs.
 *
 *  The 'count' field indicates the number of actual messages contained in the
 *  messages[] array.  The 'empty' flag is used to signal the presence of new
 *  messages: the recieving side will assert (set) this flag when it has
 *  processed all previous messages and is ready for more; the sending side
 *  will deassert (clear) this flag when it has placed new messages into the
 *  shared message buffer.  The 'postSWIFlag' is used to signal the (DSP-side)
 *  host driver that it should post the corresponding Messenger SWI.
 */
typedef struct {
    Uns emptyFlag;                      /* indicates if window is empty */
    Uns postSWIFlag;                    /* driver should post messenger SWI? */
    Uns count;                          /* # of messages currently in window */
    Uns resvd;                          /* Keep number of words even */
} MSG_Control;

typedef struct {
    Uns emptyFlag;                      /* indicates if window is empty */
    Uns postSWIFlag;                    /* driver should post messenger SWI? */
    Uns count;                          /* # of messages currently in window */
    Uns resvd;                          /* Keep number of words even */
    MSG_Msg     messages[1];  	        /* array of fixed-length messages */
} MSG_MemWindow;


/*
 * The Bridge config generates an array of structure like that
 * it contains all the callback functions for each custom commands
 */
typedef struct {
    short (*fxn)();
    Uns id;
    Uns ack;
} SHM_customCmd;
/* Maximum number of input (or output) messages allowed in shared memory */
extern Uns MSG_maxMessages;

/* Message object representing the GPP: */
extern MSG_Obj MSG_toGPP;

/* Queue of allocated but not currently in use MSG_Frames: */
extern QUE_Obj MSG_freeFramePool;

/* Semaphore for tracking the number of MSG_Frames in MSG_freeFramePool: */
extern SEM_Obj MSG_freeFrameSem;

/* Handle of the Messenger's 'toDSP' SWI: */
extern SWI_Handle MSG_toDSPSWI;

/* Handle of the Messenger's 'fromDSP' SWI: */
extern SWI_Handle MSG_fromDSPSWI;

/* Shared memory window for messages from DSP to GPP: */
extern MSG_MemWindow * MSG_pfromDSPBuffer;

/* Shared memory window for messages from GPP to DSP: */
extern MSG_MemWindow * MSG_ptoDSPBuffer;


/*
 *  ======== MSG_init ========
 *  RM Server Messenger initialization function.
 *
 *  This function will initialize the free frame pool and its counting
 *  semaphore, create the Messenger's two SWIs, and initialize the message
 *  object representing the GPP.
 *
 *  Parameters:
 *    None
 *
 *  Returns:
 *    Nothing
 *
 */
extern Void MSG_init(Void);


/*
 *  ======== MSG_interruptGPP ========
 *  A utility function called by the Messenger SWIs to generate an interrupt
 *  to the GPP.
 *
 *  Parameters:
 *    None
 *
 *  Returns:
 *    Nothing
 *
 */
extern Void MSG_interruptGPP(Void);


/*
 *  ======== MSG_notify ========
 *  A utility function used for notifying a node that a new message has just
 *  been deposited into its message queue.
 *
 *  Parameters:
 *    dest:	MSG_Obj pointer for the node to be notified.
 *
 *  Returns:
 *    Nothing
 *
 *  Requires:
 *    dest != NULL
 *
 */
extern Void MSG_notify(MSG_Obj * dest);


/*
 *  ======== MSG_signalBufferEmpty ========
 *  A utility function that the host driver can call to signal the Messenger's
 *  'fromDSP' SWI that previous messages to the GPP have been read, and the
 *  shared buffer to the GPP can now accept more messages.
 *
 *  Parameters:
 *    None
 *
 *  Returns:
 *    Nothing
 *
 */
extern Void MSG_signalBufferEmpty(Void);


/*
 *  ======== MSG_signalWorkNeeded ========
 *  A utility function that the host driver can call to signal the Messenger's
 *  'toDSP' SWI that new messages from the GPP are available, and some work
 *  is needed.
 *
 *  Parameters:
 *    None
 *
 *  Returns:
 *    Nothing
 *
 */
extern Void MSG_signalWorkNeeded(Void);


#ifdef __cplusplus
}
#endif

#endif /* MSG_ */
