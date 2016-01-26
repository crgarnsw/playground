

/*--------------------------------------------------------------
  Program:      eth_websrv_AJAX_IN

  Description:  Uses Ajax to update the state of two switches
                and an analog input on a web page. The Arduino
                web server hosts the web page.
                Does not use the SD card.
  
  Hardware:     Arduino Uno and official Arduino Ethernet
                shield. Should work with other Arduinos and
                compatible Ethernet shields.
                
  Software:     Developed using Arduino 1.0.3 software
                Should be compatible with Arduino 1.0 +
  
  References:   - WebServer example by David A. Mellis and 
                  modified by Tom Igoe
                - Ethernet library documentation:
                  http://arduino.cc/en/Reference/Ethernet
                - Learning PHP, MySQL & JavaScript by
                  Robin Nixon, O'Reilly publishers

  Date:         20 February 2013
 
  Author:       W.A. Smith, http://startingelectronics.org
--------------------------------------------------------------*/

#include <SPI.h>
#include <Ethernet.h>
#include <SD.h>
#include <RCSwitch.h>
#include "Switch433.h"

#define CODES 6

// MAC address from Ethernet shield sticker under board
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
EthernetServer server(80);  // create a server at port 80

String HTTP_req;            // stores the HTTP request
File webFile;

long codes[CODES * 2] = { 1381716, // 1 off
                          1381719, // 1 on
                          1394004, // 2 off ...
                          1394007,
                          1397076,
                          1397079,
                          4527444, //b1 off
                          4527447,
                          4539732,
                          4539735,
                          4542804,
                          4542807 };

bool lights[CODES] = { false, false, false, false, false, false }; // lights start off
bool lastLights[CODES] = { false, false, false, false, false, false }; // lights start off
String names[CODES] = { "Nicks Tree",
                        "Main Tree",
                        "Maddies Tree",
                        "Dining Tree",
                        "Living Tree",
                        "N/A" };
int indexes[CODES] = { 0, 1, 2, 3, 4, 5 };
Switch433 switch433(codes);
bool fWaitingOutlet = false;
long timeStartWaiting;
EthernetClient waitingClient;

long timeToSend = 99999999;

void setup()
{
    Ethernet.begin(mac);      // initialize Ethernet device
    server.begin();           // start to listen for clients
    Serial.begin(9600);       // for diagnostics

    // initialize SD card
    Serial.println("Initializing SD card...");
    if (!SD.begin(4)) {
        Serial.println("ERROR - SD card initialization failed!");
        return;    // init failed
    }
    Serial.println("SUCCESS - SD card initialized.");
    // check for index.htm file
    if (!SD.exists("index.htm")) {
        Serial.println("ERROR - Can't find index.htm file!");
        return;  // can't find index file
    }
    Serial.println("SUCCESS - Found index.htm file.");

    pinMode( 9, OUTPUT ); digitalWrite( 9, HIGH );
    pinMode( 8, OUTPUT ); digitalWrite( 8, LOW );
    switch433.setup();
}

void loop() {
  bool fChange = false;

  if(fWaitingOutlet) {
    bool fDone = switch433.readValue();
    if(fDone) {
      fWaitingOutlet = false;
      waitingClient.println("HTTP/1.1 200 OK");
      waitingClient.println();
    } else if( (timeStartWaiting + 10000) > millis() ) {
      fWaitingOutlet = false;
      waitingClient.println("HTTP/1.1 200 OK"); // send a bad value
      waitingClient.println();
    } else {
      return;
    }
  }

  EthernetClient client = server.available();  // try to get client

    if (client) {  // got client?
        boolean currentLineIsBlank = true;
        while (client.connected()) {
            if (client.available()) {   // client data available to read
                char c = client.read(); // read 1 byte (character) from client
                HTTP_req += c;  // save the HTTP request 1 char at a time
                // last line of client request is blank and ends with \n
                // respond to client only after last line received

                // print HTTP request character to serial monitor
                Serial.print(c);

                if (c == '\n' && currentLineIsBlank) {   
                if( HTTP_req.indexOf("switch") > -1) {
                  Serial.println("Get switch ***");
                  GetAjaxData(client);
                  fChange = true;
                  client.println("HTTP/1.1 200 OK");
                  client.println();
                } else if( HTTP_req.indexOf("init") > -1 ) {                 
                  init(client);
                } else if( HTTP_req.indexOf("plusOutlet") > -1 ) {
                  fWaitingOutlet = true;
                  timeStartWaiting = millis();
                  waitingClient = client;
                  return;
                } else {
                  int start = HTTP_req.indexOf(' ') + 1;
                  HTTP_req = HTTP_req.substring( start, HTTP_req.indexOf( ' ', start+1 ));

                  if(HTTP_req.equals("/")) {
                    HTTP_req = "index.htm";
                  }

                  webFile = SD.open(HTTP_req);
                }

                if (webFile) {
                  while(webFile.available()) {
                    client.write(webFile.read()); // send web page to client
                  }
                  webFile.close();
               }

                HTTP_req = "";            // finished with request, empty string
                break;
                }

                // every line of text received from the client ends with \r\n
                if (c == '\n') {
                    // last character on line of received text
                    // starting new line with next character read
                    currentLineIsBlank = true;
                } 
                else if (c != '\r') {
                    // a text character was received from client
                    currentLineIsBlank = false;
                }
            } // end if (client.available())
        } // end while (client.connected())
        delay(1);      // give the web browser time to receive the data
        Serial.println("Done");
        client.stop(); // close the connection
    } // end if (client)

    if(fChange) {
      fChange = false;
      for(int i=0; i<CODES; i++) {
        if( lights[i] != lastLights[i] ) {
          switch433.send( i*2 + (lights[i] ? 1:0) );
          lastLights[i] = lights[i];
          delay(100);
        }
      }
    }
}

// update the switch configuration
void GetAjaxData(EthernetClient cl) {
  int idx = HTTP_req.indexOf("switch");

  for(int i=0; i<CODES; i++) {
    lights[i] = HTTP_req[idx + 6 + i] == '1';
  }
}

void init( EthernetClient cl ) {
  cl.print("[");

  for(int i=0; i<CODES; i++) {
    cl.print( "{\"name\":\"" );
    cl.print( names[i] );
    cl.print( "\",\"idx\":\"" );
    cl.print( indexes[i] );
    cl.print( "\",\"val\":" );
    cl.print( lights[i] ? "\"true\"}" : "\"false\"}" );
    if(i<CODES-1) {
      cl.print(",");
    }
  }
  cl.println("]");
}
