// - - - - -
// DmxSerial - A hardware supported interface to DMX.
// DmxSerialRecv.ino: Sample DMX application for retrieving 3 DMX values:
// address 1 (red) -> PWM Port 9
// address 2 (green) -> PWM Port 6
// address 3 (blue) -> PWM Port 5
// 
// Copyright (c) 2011 by Matthias Hertel, http://www.mathertel.de
// This work is licensed under a BSD style license. See http://www.mathertel.de/License.aspx
// 
// Documentation and samples are available at http://www.mathertel.de/Arduino
// 25.07.2011 creation of the DmxSerial library.
// 10.09.2011 fully control the serial hardware register
//            without using the Arduino Serial (HardwareSerial) class to avoid ISR implementation conflicts.
// 01.12.2011 include file and extension changed to work with the Arduino 1.0 environment
// 28.12.2011 changed to channels 1..3 (RGB) for compatibility with the DmxSerialSend sample.
// 10.05.2012 added some lines to loop to show how to fall back to a default color when no data was received since some time.
// - - - - -

#include <DMXSerial.h>

// Constants for demo program

struct Channel {
  // output pin
  uint8_t pin;
  // initial value output when in setup
  uint8_t setup;
  // value to use when no messages have been received
  uint8_t idle;
  // written value
  uint8_t value;
};

Channel channel[] = {
//{pin, setup, idle, value},
  { 0,   0,   0, 0}, // channel 0 not used
  { 9,  80, 100, 0}, // PWM output pin for DMX channel 1, Red Light.
  { 6,   0, 200, 0}, // PWM output pin for DMX channel 2, Green Light.
  { 5,   0, 255, 0}, // PWM output pin for DMX channel 3, Blue Light.
};

#define channel_count (sizeof(channel)/sizeof(*channel))

// calls analogWrite if the value changed since the last write
static void setOutput(int i, uint8_t value)
{
  if(channel[i].value != value)
  {
    analogWrite(channel[i].pin, value);
    channel[i].value = value;
  }
}

void setup () {
  DMXSerial.init(DMXReceiver);
  
  // set some default values
  for(unsigned int i=1; i<channel_count; ++i)
  {
    DMXSerial.write(i, channel[i].setup);
    // set to a value other than setup, so the setup write will go out
    channel[i].value = !channel[i].setup;
    setOutput(i, channel[i].setup);
  }
}

void loop() {
  // Calculate how long no data backet was received
  unsigned long lastPacket = DMXSerial.noDataSince();
  
  if (lastPacket < 5000) {
    // read recent DMX values and set pwm levels 
    for(unsigned int i=1; i<channel_count; ++i) {
      setOutput(i, DMXSerial.read(i));
    }

  } else {
    // Show pure red color, when no data was received since 5 seconds or more.
    for(unsigned int i=1; i<channel_count; ++i) {
      setOutput(i, channel[i].idle);
    }
  } // if
}

// End.
