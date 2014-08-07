#include "mm_midieventbuilder.h" 
#include "mm_midievent.h" 

#define MIDIEventBuilder_appendByte(meb,byte) meb->event.msg->data[meb->waitByte++] = byte

void MIDIEventBuilder_init(MIDIEventBuilder *meb)
{
    meb->state = MIDIEventBuilder_State_WAIT_STATUS;
    meb->waitByte = 0;
    meb->event = NULL;
}

static void MIDIEventBuilder_updateIfSTATUS(MIDIEventBuilder *meb, int8_t byte, uint64_t time)
{
    if (!(meb->event = MIDIEvent_newFromStatus(byte, time))) {
        /* couldn't make event, don't change state */
        return;
    }
    meb->state = MIDIEventBuilder_State_WAIT_DATA;
    meb->waitByte = 1;
}

static void MIDIEventBuilder_updateIfCOMPLETE(MIDIEventBuilder *meb, int8_t byte,
        MIDIEventBuilder_Update_CB_t on_complete)
{


/* Function usually called when a byte has been received (over UART for
 * example). meb should have been initialized already, byte is the byte that was
 * just received, get_time is a function that will return the time, most likely
 * at which the byte was received, and on_complete is a callback to call if meb
 * is complete after the byte is appended to it.
 */
void MIDIEventBuilder_update(MIDIEventBuilder *meb,
                             int8_t byte,
                             uint64_t(*get_time)(void),
                             MIDIEventBuilder_Update_CB_t on_complete)
{

    switch (meb->state) {
        case MIDIEventBuilder_State_WAIT_STATUS :
            if (MIDIMSG_IS_STATUS(byte)) {
                MIDIEventBuilder_updateIfSTATUS(meb, byte, time);
            }
            /* return if not a status byte (don't change state) */
            return;
        case MIDIEventBuilder_State_WAIT_DATA :
            if (MIDIMSG_IS_STATUS(byte)) {
                /* old event is now garbage, start over again */
                free(meb->event);
                MIDIEventBuilder_init(meb);
                MIDIEventBuilder_updateIfSTATUS(meb, byte, time);
                return;
            } else if (MIDIMSG_IS_DATA(byte)) {
                MIDIEventBuilder_appendByte(meb, byte);
                if ((MIDIMSG_IS_2_BYTE_MSG(meb->event.msg->data[0])
                        && (meb->waitByte == 2))
                    || (MIDIMSG_IS_3_BYTE_MSG(meb->event.msg->data[0])
                        && (meb->waitByte == 3))) {




            


