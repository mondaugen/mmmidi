#ifndef MM_MIDIROUTER_H
#define MM_MIDIROUTER_H

#include "mm_midimsg.h" 

#define  MIDI_Router_NUM_CHANS 16;

typedef enum {
    MIDI_Router_Err_GOOD,
    MIDI_Router_Err_NOTIMP, /* Output when something is not implemented, such as the kind of message */
    MIDI_Router_Err_NULL    /* Something was NULL, like perhaps the message that was passed */
} MIDI_Router_Err;


typedef struct __MIDI_Router MIDI_Router;
typedef struct __MIDI_Router_CB_Set MIDI_Router_CB_Set;
typedef struct __MIDI_Router_CB_Info MIDI_Router_CB_Info;
typedef void (*MIDI_Router_CB)(void*,MIDIMsg*);

#define MIDI_CB_Info_call(cbinfo,msg) cbinfo.call(cbinfo.data,msg)

struct __MIDI_Router_CB_Info {
    MIDI_Router_CB callback;
    void *data;
};

struct __MIDI_Router_CB_Set {
    MIDI_Router_CB_Info noteOff;
    MIDI_Router_CB_Info noteOn;
    MIDI_Router_CB_Info aftertouch;
    MIDI_Router_CB_Info controlChange;
    MIDI_Router_CB_Info programChange;
    MIDI_Router_CB_Info channelPressure;
    MIDI_Router_CB_Info pitchBendChange;
    MIDI_Router_CB_Info systemCommon;
};

struct __MIDI_Router {
    MIDI_Router_CB_Set cbSets[MIDI_Router_NUM_CHANS];
};

int MIDI_Router_handleMsg(MIDI_Router *router, MIDIMsg *msg)
{
    if((!msg) || (!msg->data)) {
        return MIDI_Router_Err_NULL;
    }
    MIDIMsg_Byte_t chan = MIDIMSG_GET_CHANNEL(msg->data[0]);
    switch (MIDIMSG_GET_STATUS(msg->data[0])) {
        case MIDIMSG_NOTE_OFF :
            MIDI_CB_Info_call(router->cbSets[chan].noteOff,msg);
        case MIDIMSG_NOTE_ON :
            MIDI_CB_Info_call(router->cbSets[chan].noteOn,msg);
        case MIDIMSG_POLY_PRS :
            MIDI_CB_Info_call(router->cbSets[chan].aftertouch,msg);
        case MIDIMSG_CNTRL_CHNG :
            MIDI_CB_Info_call(router->cbSets[chan].controlChange,msg);
        case MIDIMSG_PRGRM_CHNG :
            MIDI_CB_Info_call(router->cbSets[chan].programChange,msg);
        case MIDIMSG_CHN_PRS :
            MIDI_CB_Info_call(router->cbSets[chan].channelPressure,msg);
        case MIDIMSG_PCH_BND :
            MIDI_CB_Info_call(router->cbSets[chan].pitchBendChange,msg);
        case MIDIMSG_SYS_COMMON :
            MIDI_CB_Info_call(router->cbSets[chan].systemCommon,msg);
        default :
            return MIDI_Router_Err_NOTIMP;
    }
}

#endif /* MM_MIDIROUTER_H */
