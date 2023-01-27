/*
  Using polyphony principles from the following project:
  https://diyelectromusic.wordpress.com/2021/02/05/arduino-tone-polyphony/
  Be sure to check it!
*/

#include <TimerOne.h>
#include <EEPROM.h>
#include <MIDI.h>
struct MySettings : public MIDI_NAMESPACE::DefaultSettings {
  static const bool Use1ByteParsing = false;
  static const long BaudRate = 31250;
};
MIDI_CREATE_CUSTOM_INSTANCE(HardwareSerial, Serial, MIDI, MySettings);

#define NUM_TONES 6
#define PORTDMASK  0b11111100; // Pins 2-7
#define PORTBMASK  0b00000000; // Pins 2-7
#define TICK 40    // 20uS
#define BTN_A 8
#define BTN_B 9

int outputSelect = 1;
uint16_t triggerEnable;
uint8_t notesToTrigger[NUM_TONES];

bool settingsEnable = false;


void handleNoteOn(byte channel, byte pitch, byte velocity){
  if(!settingsEnable){
    if (velocity == 0) {
      handleNoteOff(channel, pitch, velocity);
      return;
    }
    for (int i=0; i < NUM_TONES; i++) {
      if(pitch == notesToTrigger[i]){
        triggerEnable |= (1<<i);
        return;
      }
    }
  }else{
    notesToTrigger[outputSelect-1] = pitch;
  }
  return;
  
}

void handleNoteOff(byte channel, byte pitch, byte velocity){
  for (int i=0; i < NUM_TONES; i++) {
    if(pitch == notesToTrigger[i]){
      triggerEnable &= ~(1<<i);
      return;
    }
  }
  return;
}

void setup(){
  // Set the pin directions using direct port accesses
  DDRD |= PORTDMASK;
  DDRB |= PORTBMASK;

  // Start with all tones disabled
  triggerEnable = 0;

  // Use an Arduino Timer1 to generate a regular "tick"
  // to run our code every TICK microseconds.
  Timer1.initialize(TICK);
  Timer1.attachInterrupt(interrupt);

  // Set up the functions to be called on specific MIDI events
  MIDI.setHandleNoteOn(handleNoteOn);
  MIDI.setHandleNoteOff(handleNoteOff);

  // This listens to the specified MIDI channel
  MIDI.begin(((PINB & 0x3C) >> 2) + 1);
  
  for(int i=0; i < NUM_TONES; i++){
    notesToTrigger[i] = EEPROM.read(i);
  }
}

void loop(){
  MIDI.read();
  mode();
}
void trigger(){
    uint8_t pd = ((uint16_t) triggerEnable << 2) & 0xFF;  // PORTD sets 0b11111100
    // pins 0 and 1 are the RX/TX pins, we don't want mess with them
    PORTD = (PORTD & 0x03) | pd; 
}
void mode(){
  if(PINB & 1<<0){
    settingsEnable = !settingsEnable;
    if(settingsEnable){
      blinkBlink();
    }else{
      outputSelect =1;
      for(int i=0; i < NUM_TONES; i++){
        EEPROM.update(i, notesToTrigger[i]);
      }
    }
    delay(250);
  }
  if(settingsEnable){
    if(PINB & 1<<1){
      outputSelect++;
      if(outputSelect > NUM_TONES){
        outputSelect = 1;
       }
      PORTD = (PORTD & 0x03) | (1<<(1+outputSelect));
      delay(250);
    }
  }
}

void blinkBlink(){
  for (int i=0; i < NUM_TONES; i++) {
    PORTD = (PORTD & 0x03) | ((1<<i)<<2);
    delay(150);
  }
  PORTD = (PORTD & 0x03) | (outputSelect<<2);
  
}
void interrupt(){
  if(!settingsEnable){
    trigger();
  }
}
