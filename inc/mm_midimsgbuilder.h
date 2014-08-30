#ifndef MM_MIDIEVENTBUILDER_H
#define MM_MIDIEVENTBUILDER_H 

#ifdef __cplusplus
extern "C" {
#endif

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
    MIDIMsg *msg;
} MIDIMsgBuilder;

typedef void(*MIDIMsgBuilder_OnComplete_CB_t)(MIDIMsgBuilder *);

#define MIDIMsgBuilder_getMsg(mmb) (mmb->msg)
#define MIDIMsgBuilder_appendByte(mmb,byte) MIDIMsg_setByte(MIDIMsgBuilder_getMsg(mmb),mmb->waitByte++,byte)

void MIDIMsgBuilder_init(MIDIMsgBuilder *mmb);
MIDIMsgBuilder_State_t MIDIMsgBuilder_update(MIDIMsgBuilder *mmb, MIDIMsg_Byte_t byte);

#ifdef __cplusplus
}
#endif  
#endif /* MM_MIDIEVENTBUILDER_H */
