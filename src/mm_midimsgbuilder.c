#include "mm_midimsgbuilder.h" 


void MIDIMsgBuilder_init(MIDIMsgBuilder *mmb)
{
    mmb->state = MIDIMsgBuilder_State_WAIT_STATUS;
    mmb->waitByte = 0;
    mmb->n_data_desired = 0;
    mmb->msg = (MIDIMsg*)mmb->msgData;
}

/* Appends byte. If there is room, returns 0, otherwise returns -1 and byte
 * wasn't appended. */
int MIDIMsgBuilder_append_byte(MIDIMsgBuilder *mmb, MIDIMsg_Byte_t byte)
{
    if (byte < MIDI_MSG_BUILDER_MAX_MSG_LEN) {
        mmb->msgData[mmb->waitByte] = byte;
        mmb->waitByte++;
        return 0;
    }
    return -1;
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

static void MIDIMsgBuilder_updateIfCOMPLETE(MIDIMsgBuilder *mmb)
{
    mmb->state = MIDIMsgBuilder_State_COMPLETE;
}

MIDIMsgBuilder_Action_t MIDIMsgBuilder_update(MIDIMsgBuilder *mmb,
                                              MIDIMsg_Byte_t byte)
{
    switch(mmb->state) {
        case MIDIMsgBuilder_State_WAIT_DATA_SYSEX:
            if (MIDIMSG_IS_DATA(byte)) {
                /* Caller must itself pass byte to routine reading sysex bytes */
                return MIDIMsgBuilder_Action_RCVD_SYSEX_DATUM;
            } else if (MIDIMSG_IS_SYSEX_END(byte)) {
                mmb->status = MIDIMsgBuilder_State_WAIT_STATUS;
                return MIDIMsgBuilder_Action_RCVD_SYSEX_END;
            }
            /* Otherwise continue as if we just received a status byte we were
             * waiting for. */
        case MIDIMsgBuilder_State_WAIT_DATA:
            if (MIDIMSG_IS_DATA(byte)) {
                (void)MIDIMsgBuilder_append_byte(mmb,byte);
                mmb->n_data_desired--;
                if (mmb->n_data_desired == 0) {
                    /* Builder contains a full message */
                    mmb->state = MIDIMsgBuilder_State_WAIT_STATUS;
                    return MIDIMsgBuilder_Action_MSG_COMPLETE;
                }
                return MIDIMsgBuilder_Action_NONE;
            }
            /* Otherwise continue as if we just received a status byte we were
             * waiting for. */
        case MIDIMsgBuilder_State_WAIT_STATUS:
            if (MIDIMSG_IS_STATUS(byte)) {
                /* Reset builder. */
                MIDIMsgBuilder_init(mmb);
                (void)MIDIMsgBuilder_append_byte(mmb,byte);
                mmb->n_data_desired = MIDIMSG_GET_LENGTH(byte);
                if (mmb->n_data_desired == 0) {
                    /* No data needed */
                    mmb->status = MIDIMsgBuilder_State_WAIT_STATUS;
                    return MIDIMsgBuilder_Action_MSG_COMPLETE;
                } else if (mmb->n_data_desired == -1) {
                    /* Undefined number of data, which is for sysex */
                    mmb->status = MIDIMsgBuilder_State_WAIT_DATA_SYSEX;
                    return MIDIMsgBuilder_Action_RCVD_SYSEX_START;
                }
                mmb->status = MIDIMsgBuilder_State_WAIT_DATA;
                return MIDIMsgBuilder_Action_NONE;
            }
    }
}

/* Function usually called when a byte has been received (over UART for
 * example). mmb should have been initialized already, byte is the byte that was
 * just received. The function returns the state so that the application can
 * respond accordingly, here is an example of how you might use it:
 *
 * MIDIMsgBuilder mmb;
 * Some_Time_t recentEventsTime;
 * // say you want to include the time
 * Some_Time_t get_time() { ... return the time ... } 
 * void do_stuff_with_msg(MIDIMsg *msg, Some_Time_t time) { ... do stuff ... }
 * void some_callback_supplying_byte(int8_t byte)
 * {
 *     // keep track of the last state so you only query the time once the
 *     // status byte received, say
 *     static MIDIMsg_State_t lastState; 
 *     switch(MIDIMsgBuilder_update(&mmb,byte)) {
 *         // we care about this event because this is when we get the time
 *         case MIDIMsgBuilder_State_WAIT_DATA:
 *             if (lastState == MIDIMsgBuilder_State_WAIT_STATUS) {
 *                 // We've just received the status byte, get the time now!
 *                 recentEventsTime = get_time();
 *             }
 *             break;
 *         case MIDIMsgBuilder_State_COMPLETE:
 *             do_stuff_with_msg(mmb.msg,recentEventsTime);
 *             MIDIMsgBuilder_init(&mmb); // reset the builder
 *             break;
 *     }
 *     lastState = mmb.state; // keep track of the last state
 * }
 */
MIDIMsgBuilder_State_t MIDIMsgBuilder_update(MIDIMsgBuilder *mmb,
                             MIDIMsg_Byte_t byte)
{

    switch (mmb->state) {
        case MIDIMsgBuilder_State_WAIT_STATUS :
            if (MIDIMSG_IS_STATUS(byte)) {
                MIDIMsgBuilder_updateIfSTATUS(mmb, byte);
            }
            /* return if not a status byte (don't change state) */
            return mmb->state;
        case MIDIMsgBuilder_State_WAIT_DATA :
            if (MIDIMSG_IS_STATUS(byte)) {
                /* old event is now garbage, start over again */
                MIDIMsg_free(mmb->msg);
                MIDIMsgBuilder_init(mmb);
                MIDIMsgBuilder_updateIfSTATUS(mmb, byte);
            } else if (MIDIMSG_IS_DATA(byte)) {
                MIDIMsgBuilder_appendByte(mmb, byte);
                if (MIDIMSG_GET_LENGTH(MIDIMsg_getByte(MIDIMsgBuilder_getMsg(mmb),0)) 
                        == mmb->waitByte) {
                    MIDIMsgBuilder_updateIfCOMPLETE(mmb);
                }
            }
            return mmb->state;
        case MIDIMsgBuilder_State_COMPLETE:
            break;
        default:
            break;
    }
    return mmb->state;
}

