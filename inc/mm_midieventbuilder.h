#ifndef MM_MIDIEVENTBUILDER_H
#define MM_MIDIEVENTBUILDER_H 
#include <stdlib.h>
#include <stdint.h> 

typedef enum
{
    MIDIEventBuilder_State_WAIT_STATUS = 0x00,
    MIDIEventBuilder_State_WAIT_DATA   = 0x01,
    MIDIEventBuilder_State_COMPLETE    = 0x02
} MIDIEventBuilder_State_t;

typedef struct __MIDIEventBuilder {
    MIDIEventBuilder_State_t state;
    size_t wantByte;
    MIDIEvent *event;
} MIDIEventBuilder;

typedef (void)(*MIDIEventBuilder_Update_CB_t)(MIDIEventBuilder *);


#endif /* MM_MIDIEVENTBUILDER_H */
