#include <stdlib.h> 
#include <stdio.h> 
#include <assert.h> 
#include "mm_midieventbuilder.h"

#define MIDI_STATUS_1       0x91 /* Note on, channel 2 */
#define MIDI_DATA_1         0x77
#define MIDI_DATA_2         0x66
#define MIDI_SYSEX_STATUS   0xf0 /* System exclusive */
#define MIDI_STATUS_2       (MIDIMSG_PRGRM_CHNG | 0x03) /* Program change, channel 4 */

#define MIDIEventBuilder_assertEqual(a,b) assert((a.state == b.state) && \
        (a.waitByte == b.waitByte) && \
        (a.event == b.event))

MIDIEvent_Time_t dumb_get_time(void)
{
    return 1234;
}

void dumb_on_complete(MIDIEventBuilder *meb)
{
    if (meb->event) {
        printf("Event complete:\n Time: %llu\n", meb->event->time);
        size_t i;
        for (i = 0; i < MIDIMSG_GET_LENGTH(MIDIEventBuilder_getMsg(meb).data[0]); ++i) {
            printf(" Data %lu: %#x\n", i, MIDIEventBuilder_getMsg(meb).data[i]);
        }
        free(meb->event);
        return;
    }
    printf("Event is NULL.\n");
}

int main(void)
{
    MIDIEventBuilder meb,
                     test1Meb = {MIDIEventBuilder_State_WAIT_STATUS, 0, NULL};
    MIDIEventBuilder_init(&meb);

    /* if receives data, should do nothing */
    MIDIEventBuilder_update(&meb, MIDI_DATA_1, dumb_get_time, dumb_on_complete);
    MIDIEventBuilder_assertEqual(meb,test1Meb);

    /* if receives SYSEX, should not allocate event */
    MIDIEventBuilder_update(&meb, MIDI_SYSEX_STATUS, dumb_get_time, dumb_on_complete);
    MIDIEventBuilder_assertEqual(meb,test1Meb);

    /* if receives note on, should be waiting for 2 data bytes */
    MIDIEventBuilder_update(&meb, MIDI_STATUS_1, dumb_get_time, dumb_on_complete);
    assert(meb.state == MIDIEventBuilder_State_WAIT_DATA);
    MIDIEventBuilder_update(&meb, MIDI_DATA_1, dumb_get_time, dumb_on_complete);
    assert(meb.state == MIDIEventBuilder_State_WAIT_DATA);
    MIDIEventBuilder_update(&meb, MIDI_DATA_2, dumb_get_time, dumb_on_complete);
    assert(meb.state == MIDIEventBuilder_State_WAIT_STATUS);

    /* if receives two status in a row, should be waiting for data with latter
     * status at byte 0*/
    MIDIEventBuilder_update(&meb, MIDI_STATUS_2, dumb_get_time, dumb_on_complete);
    assert(meb.state == MIDIEventBuilder_State_WAIT_DATA);
    MIDIEventBuilder_update(&meb, MIDI_STATUS_1, dumb_get_time, dumb_on_complete);
    assert(meb.state == MIDIEventBuilder_State_WAIT_DATA);
    MIDIEventBuilder_update(&meb, MIDI_STATUS_2, dumb_get_time, dumb_on_complete);
    assert(meb.state == MIDIEventBuilder_State_WAIT_DATA);
    MIDIEventBuilder_update(&meb, MIDI_DATA_2, dumb_get_time, dumb_on_complete);
    assert(meb.state == MIDIEventBuilder_State_WAIT_STATUS);

    /* if waiting for data and receive a bad byte (ie. sysex) should go back to
     * waiting for status state */
    MIDIEventBuilder_update(&meb, MIDI_STATUS_1, dumb_get_time, dumb_on_complete);
    assert(meb.state == MIDIEventBuilder_State_WAIT_DATA);
    MIDIEventBuilder_update(&meb, MIDI_SYSEX_STATUS, dumb_get_time, dumb_on_complete);
    assert(meb.state == MIDIEventBuilder_State_WAIT_STATUS);

    return 0;
}
