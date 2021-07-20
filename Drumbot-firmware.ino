#include <MIDI.h>
#include <EEPROM.h>
#define ARRAY_SIZE(array) (sizeof((array))/sizeof((array[0])))

#define EXTRA_POWER_PIN 2
#define BUTTON_DATA_PIN 3
#define RED_LED 4
#define GREEN_LED 5

MIDI_CREATE_DEFAULT_INSTANCE();

int pins [] =     {13, 8, 10, 12, 11,  7, 6};
int notes [] =    {60, 61, 62, 63, 64, 65, 66};
int onCycles [] = {0,  0,  0,  0,  0,  0, 0};
int securityLimit = 90000;
int availableNotes = 0;
bool programmingMode = false;
int maxDuration = 2000;
int minDuration = 1000;

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
    if(programmingMode)
    {
      programNotes(pitch);
      setProgrammingMode(false);
    }
    else{
      int pin = -1;

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
  
        if (pin > 0) {
          digitalWrite(pin,HIGH);
        }
      }
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

  if (pin > 0) {
     digitalWrite(pin, LOW);
  }
}

void setup()
{
  availableNotes = ARRAY_SIZE(notes);

  MIDI.setHandleNoteOn(handleNoteOn);
  //MIDI.setHandleNoteOff(handleNoteOff);
  MIDI.begin(MIDI_CHANNEL_OMNI);

  pinMode(EXTRA_POWER_PIN,OUTPUT);
  pinMode(BUTTON_DATA_PIN,INPUT);
  pinMode(RED_LED, OUTPUT);
  pinMode(GREEN_LED, OUTPUT);
  digitalWrite(EXTRA_POWER_PIN, HIGH);

  for(int i=0; i<availableNotes; i++)
  {
    pinMode(pins[i],OUTPUT);
  }

  programNotes(EEPROM.read(0));

  reset();
}

void setProgrammingMode(bool value){
  programmingMode = value;
  if(value){
    digitalWrite(RED_LED, HIGH);
  }
  else {
    digitalWrite(RED_LED, LOW);
  }
}

void programNotes(int base_note){
  for(int i=0; i<availableNotes; i++){
      notes[i] = base_note + i;
  }

  if(EEPROM.read(0) != base_note){
    EEPROM.write(0, base_note);
  }
}

void delayCheck()
{
    bool playingNote = false;
    
    for(int i=0; i<availableNotes; i++)
    {
      if(onCycles[i] > 0)
      {
        onCycles[i]--;
        playingNote = true;
      }
      else
      {
        digitalWrite(pins[i],LOW);
        onCycles[i] = 0;
      }
    }
    digitalWrite(GREEN_LED, playingNote);
}

void buttonCheck(){
  if(digitalRead(BUTTON_DATA_PIN) == HIGH){
    setProgrammingMode(true); 
  }
}

void loop()
{
    MIDI.read();
    delayCheck();
    buttonCheck();
}
