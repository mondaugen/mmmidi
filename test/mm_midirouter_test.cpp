//*****************************************//
//  qmidiin.cpp
//  by Gary Scavone, 2003-2004.
//
//  Simple program to test MIDI input and
//  retrieval from the queue.
//
//*****************************************//

#include <iostream>
#include <cstdlib>
#include <signal.h>
#include "RtMidi.h"
#include <cstdio> 

extern "C" {
 #include "mm_midirouter_standard.h"
} 

// Platform-dependent sleep routines.
#if defined(__WINDOWS_MM__)
  #include <windows.h>
  #define SLEEP( milliseconds ) Sleep( (DWORD) milliseconds ) 
#else // Unix variants
  #include <unistd.h>
  #define SLEEP( milliseconds ) usleep( (unsigned long) (milliseconds * 1000.0) )
#endif

bool done;
static void finish( int /*ignore*/ ){ done = true; }

void usage( void ) {
  // Error function in case of incorrect command-line
  // argument specifications.
  std::cout << "\nusage: qmidiin <port>\n";
  std::cout << "    where port = the device to use (default = 0).\n\n";
  exit( 0 );
}

struct MyMidiCCMsg {
    char *str;
};

void MyMidiCCMsg_do(void *data, MIDIMsg *msg)
{
    printf("%s: %d\n", ((struct MyMidiCCMsg*)data)->str, msg->data[2]);
}

int main( int argc, char *argv[] )
{
  RtMidiIn *midiin = 0;
  std::vector<unsigned char> message;
  int nBytes, i;
  double stamp;

  // Minimal command-line check.
  if ( argc > 2 ) usage();

  // RtMidiIn constructor
  try {
    midiin = new RtMidiIn();
  }
  catch ( RtMidiError &error ) {
    error.printMessage();
    exit( EXIT_FAILURE );
  }

  // Check available ports vs. specified.
  unsigned int port = 0;
  unsigned int nPorts = midiin->getPortCount();
  if ( argc == 2 ) port = (unsigned int) atoi( argv[1] );
  if ( port >= nPorts ) {
    delete midiin;
    std::cout << "Invalid port specifier!\n";
    usage();
  }

  try {
    midiin->openPort( port );
  }
  catch ( RtMidiError &error ) {
    error.printMessage();
    delete midiin;
    return 0;
  }

  // Don't ignore sysex, timing, or active sensing messages.
  midiin->ignoreTypes( false, false, false );

  // Install an interrupt handler function.
  done = false;
  (void) signal(SIGINT, finish);

  // Set up our control change handler
  MIDI_Router_Standard router;
  MIDI_Router_Standard_init(&router);
  struct MyMidiCCMsg MyMidiCCMsg1 = { "blah blah" };
  struct MyMidiCCMsg MyMidiCCMsg2 = { "matt damon" };
  // Set callbacks on cc 2 and 14 on channel 0
  MIDI_CC_CB_Router_addCB(&router.cbRouters[0],2,MyMidiCCMsg_do,(void*)&MyMidiCCMsg1);
  MIDI_CC_CB_Router_addCB(&router.cbRouters[0],14,MyMidiCCMsg_do,(void*)&MyMidiCCMsg2);

  // Periodically check input queue.
  std::cout << "Reading MIDI from port ... quit with Ctrl-C.\n";
  while ( !done ) {
    stamp = midiin->getMessage( &message );
    nBytes = message.size();
    MIDIMsg *midiMsg = MIDIMsg_new(nBytes);
    for ( i=0; i<nBytes; i++ )
        std::cout << "Byte " << i << " = " << (int)message[i] << ", ";
        midiMsg->data[i] = message[i];
    if ( nBytes > 0 )
        std::cout << "stamp = " << stamp << std::endl;

    MIDI_Router_handleMsg(&router.router,midiMsg);

    // Sleep for 10 milliseconds.
    SLEEP( 10 );
  }

  delete midiin;

  return 0;
}
