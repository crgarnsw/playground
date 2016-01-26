/**
 * Switch433.h
 *  Interface to the control the 433 Mhz switches
 */
#ifndef __SWITCH_433__
#define __SWITCH_433__

#include <RCSwitch.h>

namespace {
  RCSwitch mySwitch = RCSwitch();
}

class Switch433 {
  private:
  	long* m_codes;

  public:
    Switch433( long* a_codes ) :
      m_codes( a_codes ) {};

 	void setup() {
      mySwitch.enableReceive(0);   // Receiver on inerrupt 0 => that is pin #2
      mySwitch.enableTransmit(10); // Transmitter is connected to Arduino Pin #10  
    };

    bool readValue() {
      bool ret = false;

      if(mySwitch.available()) {
        int value = mySwitch.getReceivedValue();

        Serial.print("Received Value: ");
        Serial.println(value);
    
        if (value == 0) {
          Serial.print("Unknown encoding");
        } else {
          Serial.print("Received ");
          Serial.print( mySwitch.getReceivedValue() );
          Serial.print(" / ");
          Serial.print( mySwitch.getReceivedBitlength() );
          Serial.print("bit ");
          Serial.print("Protocol: ");
          Serial.println( mySwitch.getReceivedProtocol() );
        }
        mySwitch.resetAvailable();
        ret = true;
      }

      return ret;
    };

    void send( int a_idx ) {
      mySwitch.send( m_codes[a_idx], 24);
    };
 };

 #endif