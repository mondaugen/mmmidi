#ifndef MM_MIDIEVENTBUILDER_H
#define MM_MIDIEVENTBUILDER_H 
#include <stdlib.h>
#include <stdint.h> 
#include "mm_midimsg.h" 

typedef enum
{
    MIDIMsgBuilder_State_WAIT_STATUS = 0x00,
    MIDIMsgBuilder_State_WAIT_DATA   = 0x01,
    MIDIMsgBuilder_State_COMPLETE    = 0x02
} MIDIMsgBuilder_State_t;

typedef struct __MIDIMsgBuilder {
    MIDIMsgBuilder_State_t state;
    size_t waitByte;
    MIDIMsg *event;
} MIDIMsgBuilder;

typedef void(*MIDIMsgBuilder_OnComplete_CB_t)(MIDIMsgBuilder *);
/* This doesn't make sense because MIDIMsg doesn't have a time associated with
 * it. */
typedef MIDIMsg_Time_t(*MIDIMsgBuilder_GetTime_CB_t)(void);

#define MIDIMsgBuilder_getMsg(mmb) (mmb->event->msg)
#define MIDIMsgBuilder_appendByte(mmb,byte) MIDIMsgBuilder_getMsg(mmb).data[mmb->waitByte++] = byte

#endif /* MM_MIDIEVENTBUILDER_H */
