#include "mm_midimsg.h" 

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
