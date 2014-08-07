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

#define MIDIMSG_GET_STATUS(x) ((x) & 0xf0) 
#define MIDIMSG_GET_CHANNEL(x) ((x) & 0x0f) 

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

typedef struct __MIDIMsg {
    int8_t data[0];
} MIDIMsg;

typedef struct __MIDIEvent {
    uint64_t time;
    MIDIMsg msg;
} MIDIEvent;

/* returns the size of the MIDIMsg that would be allocated */
size_t MIDIMsg_sizeof(size_t length)
{
    return sizeof(MIDIMsg) + sizeof(int8_t) * length;
}

MIDIMsg *MIDIMsg_new(size_t length)
{
    if (length == 0) { return NULL; }
    MIDIMsg *result = (MIDIMsg *)malloc(MIDIMsg_sizeof(length));
    return result;
}

size_t MIDIMsg_lengthFromStatus(int8_t status)
{
    if (MIDIMSG_IS_2_BYTE_MSG(status)) {
        return 2;
    } else if (MIDIMSG_IS_3_BYTE_MSG(status)) {
        return 3;
    } else {
        return 0;
    }
}

/* msg is pointer to MIDIMsg you want to init and n is number of args you have
 * supplied to the function */
void MIDIMsg_init(MIDIMsg *msg, size_t n, ...)
{
    va_list args;
    int datum;
    size_t idx;
    va_start(args, n);
    for (idx = 0; idx < n; idx++) {
        datum = va_arg(args, int);
        msg->data[idx] = (int8_t)datum;
    }
    va_end(args);
}

MIDIMsg *MIDIMsg_newFromStatus(int8_t status)
{
    MIDIMsg *result;
    if (!(result = MIDIMsg_new(MIDIMsg_lengthFromStatus(status)))) {
        /* no message allocated because bad status */
        return NULL;
    }
    MIDIMsg_init(result, 1, status);
    return result;
}

MIDIEvent *MIDIEvent_new(size_t length, uint64_t time) {
    if (length == 0) { return NULL; }
    MIDIEvent *result = (MIDIEvent *)malloc(sizeof(MIDIEvent)
            + MIDIMsg_sizeof(length));
    result->time = time;
    return result;
}

MIDIEvent *MIDIEvent_newFromStatus(size_t status, uint64_t time) {
    MIDIEvent *result;
    if (!(result = MIDIEvent_new(MIDIMsg_lengthFromStatus(status),
                    time))) {
        return NULL;
    }
    MIDIMsg_init(&(result->msg), 1, status);
}

int main(void)
{
    MIDIMsg *msg1, *msg2, *msg3;
    MIDIEvent *event1;
    size_t i;
    msg1 = MIDIMsg_new(5);
    MIDIMsg_init(msg1, 5, -128, -129, -130, -131, -132);
    msg2 = MIDIMsg_newFromStatus(0x97); /* note on channel 8 */
    msg2->data[1] = 58; /* B flat */
    msg2->data[2] = 69; /* velocity.. */
    msg3 = MIDIMsg_newFromStatus(0xc5); /* program change channel 6 */
    msg3->data[1] = 100; /* some program */
    event1 = MIDIEvent_newFromStatus(0xbc, 200); /* control change, channel 13 */
    MIDIMsg_init(&(event1->msg),3,0xbc,25,50);
    free(msg1); free(msg2); free(msg3);
    free(event1);
    return 0;
}
