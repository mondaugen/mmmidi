int main(void)
{
    MIDIMsg *msg1, *msg2, *msg3;
    MIDIEvent *event1;
    size_t i;
    msg1 = MIDIMsg_new(5);
    MIDIMsg_init(msg1, 5, -128, -129, -130, -131, -132);
    msg2 = MIDIMsg_newFromStatus(0x97); /* note on channel 8 */
    msg2->data[1] = 58; /* B flat */
    msg2->data[2] = 69; /* velocity.. */
    msg3 = MIDIMsg_newFromStatus(0xc5); /* program change channel 6 */
    msg3->data[1] = 100; /* some program */
    event1 = MIDIEvent_newFromStatus(0xbc, 200); /* control change, channel 13 */
    MIDIMsg_init(&(event1->msg),3,0xbc,25,50);
    free(msg1); free(msg2); free(msg3);
    free(event1);
    return 0;
}
