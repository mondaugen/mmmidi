#include "mm_midimsgbuilder.h" 


void MIDIMsgBuilder_init(MIDIMsgBuilder *mmb)
{
    mmb->state = MIDIMsgBuilder_State_WAIT_STATUS;
    mmb->waitByte = 0;
    mmb->msg = NULL;
}

static void MIDIMsgBuilder_updateIfSTATUS(MIDIMsgBuilder *mmb, MIDIMsg_Byte_t byte)
{
    if (!(mmb->msg = MIDIMsg_newFromStatus(byte))) {
        /* couldn't make event, don't change state */
        return;
    }
    mmb->state = MIDIMsgBuilder_State_WAIT_DATA;
    mmb->waitByte = 1;
}

static void MIDIMsgBuilder_updateIfCOMPLETE(MIDIMsgBuilder *mmb,
        MIDIMsgBuilder_OnComplete_CB_t on_complete)
{
    /* MIDIMsg ready to move on (get stored or whatever) */
    on_complete(mmb);
    /* return builder to initial state */
    MIDIMsgBuilder_init(mmb);
}


/* Function usually called when a byte has been received (over UART for
 * example). mmb should have been initialized already, byte is the byte that was
 * just received, and on_complete is a callback to call if mmb is complete after
 * the byte is appended to it.
 */
void MIDIMsgBuilder_update(MIDIMsgBuilder *mmb,
                             MIDIMsg_Byte_t byte,
                             MIDIMsgBuilder_OnComplete_CB_t on_complete)
{

    switch (mmb->state) {
        case MIDIMsgBuilder_State_WAIT_STATUS :
            if (MIDIMSG_IS_STATUS(byte)) {
                MIDIMsgBuilder_updateIfSTATUS(mmb, byte, get_time());
            }
            /* return if not a status byte (don't change state) */
            return;
        case MIDIMsgBuilder_State_WAIT_DATA :
            if (MIDIMSG_IS_STATUS(byte)) {
                /* old event is now garbage, start over again */
                free(mmb->msg);
                MIDIMsgBuilder_init(mmb);
                MIDIMsgBuilder_updateIfSTATUS(mmb, byte, get_time());
            } else if (MIDIMSG_IS_DATA(byte)) {
                MIDIMsgBuilder_appendByte(mmb, byte);
                /*
                if ((MIDIMSG_IS_2_BYTE_MSG(MIDIMsgBuilder_getMsg(mmb).data[0])
                        && (mmb->waitByte == 2))
                    || (MIDIMSG_IS_3_BYTE_MSG(MIDIMsgBuilder_getMsg(mmb).data[0])
                        && (mmb->waitByte == 3)))
                */
                if (MIDIMSG_GET_LENGTH(MIDIMsgBuilder_getMsg(mmb).data[0]) == meb->waitByte) {
                    MIDIMsgBuilder_updateIfCOMPLETE(mmb, on_complete);
                }
            }
            return;
    }
}

