#ifndef MM_MIDIMSG_H
#define MM_MIDIMSG_H 

#include <stdlib.h>
#include <stdint.h> 
#include <stdarg.h> 

#define MIDIMSG_NOTE_OFF    0x80
#define MIDIMSG_NOTE_ON     0x90
#define MIDIMSG_POLY_PRS    0xa0
#define MIDIMSG_CNTRL_CHNG  0xb0
#define MIDIMSG_PRGRM_CHNG  0xc0
#define MIDIMSG_CHN_PRS     0xd0
#define MIDIMSG_PCH_BND     0xe0

#define MIDIMSG_IS_STATUS(x) ((x) & 0x80) 
#define MIDIMSG_IS_DATA(x)   (!(MIDIMSG_IS_STATUS(x))) 

#define MIDIMSG_GET_STATUS(x)       ((x) & 0xf0) 
#define MIDIMSG_GET_CHANNEL(x)      ((x) & 0x0f) 
#define MIDIMSG_KNOW_LENGTH(x)      (MIDIMSG_IS_2_BYTE_MSG(x) || MIDIMSG_IS_3_BYTE_MSG(x))  
#define MIDIMSG_GET_LENGTH(x)       (MIDIMSG_IS_2_BYTE_MSG(x) ? 2 : (MIDIMSG_IS_3_BYTE_MSG(x) ? 3 : 0))

#define MIDIMSG_IS_NOTE_OFF(x)      (MIDIMSG_GET_STATUS(x) == MIDIMSG_NOTE_OFF)
#define MIDIMSG_IS_NOTE_ON(x)       (MIDIMSG_GET_STATUS(x) == MIDIMSG_NOTE_ON)
#define MIDIMSG_IS_POLY_PRS(x)      (MIDIMSG_GET_STATUS(x) == MIDIMSG_POLY_PRS)
#define MIDIMSG_IS_CNTRL_CHNG(x)    (MIDIMSG_GET_STATUS(x) == MIDIMSG_CNTRL_CHNG)
#define MIDIMSG_IS_PRGRM_CHNG(x)    (MIDIMSG_GET_STATUS(x) == MIDIMSG_PRGRM_CHNG)
#define MIDIMSG_IS_CHN_PRS(x)       (MIDIMSG_GET_STATUS(x) == MIDIMSG_CHN_PRS)
#define MIDIMSG_IS_PCH_BND(x)       (MIDIMSG_GET_STATUS(x) == MIDIMSG_PCH_BND)

#define MIDIMSG_IS_2_BYTE_MSG(x)    (MIDIMSG_IS_PRGRM_CHNG(x) || \
                                     MIDIMSG_IS_CHN_PRS(x))
#define MIDIMSG_IS_3_BYTE_MSG(x)    (MIDIMSG_IS_NOTE_OFF(x)   || \
                                     MIDIMSG_IS_NOTE_ON(x)    || \
                                     MIDIMSG_IS_POLY_PRS(x)   || \
                                     MIDIMSG_IS_CNTRL_CHNG(x) || \
                                     MIDIMSG_IS_PCH_BND(x))

/* These are provided for future extensions where, perhaps, the data will be a
 * linked list rather than an array of bytes, to support sysex messages, for
 * example.
 * Just so you don't forget, the way you would use a sort of message subclass
 * would be you would check the status byte to see what kind of message you
 * have, if you have a channel message, use the classic array getting and
 * setting, DGAF. If you have a sysex message, well then you gotta cast to that
 * subclass type and use the linked-list getter setter methods.
 * */
#define MIDIMsg_setByte(msg,index,value) (msg)->data[index] = value
#define MIDIMsg_getByte(msg,index) (msg)->data[index]
#define MIDIMsg_free(msg) free(msg) 

typedef uint8_t MIDIMsg_Byte_t;

typedef struct __MIDIMsg {
    MIDIMsg_Byte_t data[0];
} MIDIMsg;

size_t MIDIMsg_sizeof(size_t length);
MIDIMsg *MIDIMsg_new(size_t length);
size_t MIDIMsg_lengthFromStatus(MIDIMsg_Byte_t status);
void MIDIMsg_init(MIDIMsg *msg, size_t n, ...);
MIDIMsg *MIDIMsg_newFromStatus(MIDIMsg_Byte_t status);

#ifdef MIDI_MSG_DEBUG
#include <stdio.h> 

static inline void MIDIMsg_print(MIDIMsg *msg)
{
    size_t length = MIDIMsg_lengthFromStatus(msg->data[0]), i;
    for (i = 0; i < length; i++) {
        printf("%0#4x (%3d) ", msg->data[i], msg->data[i]);
    }
    printf("\n");
}

/* returns 1 if message equal to string of bytes supplied, 0 otherwise */
static inline int MIDIMsg_checkEqual(MIDIMsg *msg, MIDIMsg_Byte_t *bytes, size_t length)
{
    size_t msgLength = MIDIMsg_lengthFromStatus(msg->data[0]), i;
    if (msgLength != length) { return 0; }
    int result = 1;
    for (i = 0; i < length; i++) {
        result = result && (msg->data[i] == bytes[i]);
    }
    return result;
}

#endif  

#endif /* MM_MIDIMSG_H */
