#include <stdlib.h>
#include <stdio.h>
#include <stdint.h> 
#include <time.h> 
#include <unistd.h> 

#define MIDI_MSG_DEBUG

#include "mm_midimsgbuilder.h" 

static MIDIMsgBuilder_State_t lastState; 
static MIDIMsgBuilder midiMsgBuilder;
static time_t recentEventsTime;

typedef struct __MsgTimePair {
    time_t  time;
    MIDIMsg *msg;
} MsgTimePair;

size_t mtpsIdx = 0;
MsgTimePair mtps[100];

// say you want to include the time
time_t get_time()
{
    return time(NULL);
} 

void do_stuff_with_msg(MIDIMsg *msg, time_t time)
{
    mtps[mtpsIdx].msg = msg;
    mtps[mtpsIdx].time = time;
    mtpsIdx++;
}


void some_callback_supplying_byte(int8_t byte)
{
   // keep track of the last state so you only query the time once the
   // status byte received
    switch(MIDIMsgBuilder_update(&midiMsgBuilder,byte)) {
        // we care about this event because this is when we get the time
        case MIDIMsgBuilder_State_WAIT_DATA:
            if (lastState == MIDIMsgBuilder_State_WAIT_STATUS) {
                // We've just received the status byte, get the time now!
                recentEventsTime = get_time();
            }
            break;
        case MIDIMsgBuilder_State_COMPLETE:
            do_stuff_with_msg(midiMsgBuilder.msg,recentEventsTime);
            MIDIMsgBuilder_init(&midiMsgBuilder); // reset the builder
            break;
    }
    lastState = midiMsgBuilder.state; // keep track of the last state
}

int main (void)
{
    int8_t bytes[] = {
        0x71, /* data bytes, should get ignored */
        0x72,
        0x92, /* note on, channel 3 */
        0xb4, /* control change, channel 4, should throw out note on */
        75,   /* control change number */
        0xb4, /* control change, channel 5, should throw out control change */
        76,   /* control change number */
        99,   /* control change value, should be complete, yay */
        0xc1, /* program change, channel 2 */
        2     /* program change value, should be complete now */
    }; /* should be enough for now */
    size_t i;
    printf("Takes some time to run because delays need to happen for time to change.\n");
    for (i = 0; i < (sizeof(bytes)/sizeof(int8_t)); ++i) {
        some_callback_supplying_byte(bytes[i]);
        sleep(1);
    }
    for (i = 0; i < mtpsIdx; ++i) {
        printf("Time: %d\n", (int)mtps[i].time);
        MIDIMsg_print(mtps[i].msg);
    }
    int result = 1;
    MIDIMsg_Byte_t testMsgs[][3] = {{0xb4,76,99},{0xc1,2,0}};
    size_t testMsgsSizes[]      = {3           ,2         };

    for (i = 0; i < 2; i++) {
        result = result && (MIDIMsg_checkEqual(mtps[i].msg,testMsgs[i],
                    testMsgsSizes[i]));
    }
    result ? exit(0) : exit(1);
}
        


