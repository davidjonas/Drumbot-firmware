#include <MIDI.h>
#define ARRAY_SIZE(array) (sizeof((array))/sizeof((array[0])))

MIDI_CREATE_DEFAULT_INSTANCE();

int pins [] =     {13, 12, 11, 10, 8,  7};
int notes [] =    {60, 61, 62, 63, 64, 65};
int onCycles [] = {0,  0,  0,  0,  0,  0};
int maxDuration = 2000;
int minDuration = 1000;
int availableNotes = 0;

void reset()
{
  for(int i=0; i<availableNotes; i++)
  {
    digitalWrite(pins[i],LOW);
    onCycles[i] = 0;
  }
}

void handleNoteOn(byte channel, byte pitch, byte velocity)
{
    int pin;

    if(velocity > 0){
      for(int i=0; i<availableNotes; i++)
      {
        if(notes[i] == pitch)
        {
          pin = pins[i];
          onCycles[i] = map((int)velocity, 1, 127, minDuration, maxDuration);
          break;
        }
      }
  
      digitalWrite(pin,HIGH);
    }
}


//Never called right now
void handleNoteOff(byte channel, byte pitch, byte velocity)
{
  int pin;

  for(int i=0; i<availableNotes; i++)
  {
    if(notes[i] == pitch)
    {
      pin = pins[i];
      onCycles[i] = 0;
      break;
    }
  }

  digitalWrite(pin,LOW);
}

void setup()
{
  availableNotes = ARRAY_SIZE(notes);

  MIDI.setHandleNoteOn(handleNoteOn);
  //MIDI.setHandleNoteOff(handleNoteOff);
  MIDI.begin(MIDI_CHANNEL_OMNI);

  for(int i=0; i<availableNotes; i++)
  {
    pinMode(pins[i],OUTPUT);
    digitalWrite(pins[i],LOW);
  }

  reset();
}

void delayCheck()
{
    for(int i=0; i<availableNotes; i++)
    {
      if(onCycles[i] >= 0)
      {
        onCycles[i]--;
      }

      if(onCycles[i] <= 0)
      {
        digitalWrite(pins[i],LOW);
        onCycles[i] = 0;
        return;
      }
    }
}

void loop()
{
    MIDI.read();
    delayCheck();
}
