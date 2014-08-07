#ifndef MM_MIDIEVENT_H
#define MM_MIDIEVENT_H 

typedef struct __MIDIEvent {
    uint64_t time;
    MIDIMsg msg;
} MIDIEvent;

MIDIEvent *MIDIEvent_new(size_t length, uint64_t time);
MIDIEvent *MIDIEvent_newFromStatus(size_t status, uint64_t time);

#endif /* MM_MIDIEVENT_H */
