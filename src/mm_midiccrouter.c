#include "mm_midiccrouter.h" 

/* Route the control change message to the correct callback and call it with val */
void MIDI_CC_CB_Router_route(void *router, MIDIMsg_Byte_t cc, MIDIMsg_Byte_t val)
{

    MIDI_CC_CB_Info_call(((MIDI_CC_CB_Router*)router)->cbInfos[cc],val);
}

/* Add the cb and its data to the cc number of router */
void MIDI_CC_CB_Router_addCB(MIDI_CC_CB_Router *router, MIDIMsg_Byte_t cc, MIDI_CC_CB_Router_CB cb, void *data)
{
    router->cbInfos[cc].callback = cb;
    router->cbInfos[cc].data = data;
}
