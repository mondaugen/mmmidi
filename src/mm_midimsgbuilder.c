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

static void MIDIMsgBuilder_updateIfCOMPLETE(MIDIMsgBuilder *mmb)
{
    mmb->state = MIDIMsgBuilder_State_COMPLETE;
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
                             MIDIMsg_Byte_t byte,
                             MIDIMsgBuilder_OnComplete_CB_t on_complete)
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
                free(mmb->msg);
                MIDIMsgBuilder_init(mmb);
                MIDIMsgBuilder_updateIfSTATUS(mmb, byte);
            } else if (MIDIMSG_IS_DATA(byte)) {
                MIDIMsgBuilder_appendByte(mmb, byte);
                if (MIDIMSG_GET_LENGTH(MIDIMsgBuilder_getMsg(mmb)->data[0]) == mmb->waitByte) {
                    MIDIMsgBuilder_updateIfCOMPLETE(mmb);
                }
            }
            return mmb->state;
    }
}

