/*Manages the ATLAS Oxygen Probes*/
#include <SoftwareSerial.h> //we have to include the SoftwareSerial library, or else we can't use it
#define PIN_RX 12               // define what pin PIN_RX is going to be
#define PIN_TX 13

SoftwareSerial myserial(PIN_RX, PIN_TX);

String sensorstring = "";

void atlas_sleep()
{
    myserial.begin(9600);
    sensorstring.reserve(10);
    myserial.print("Sleep\r");
}

void atlas_read()
{
    if (myserial.available() > 0)
    {                                        // if we see that the Atlas Scientific product has sent a character
        char inchar = (char)myserial.read(); // get the char we just received
        sensorstring += inchar;              // add the char to the var called sensorstring
        if (inchar == '\r')
        {                                  // if the incoming character is a <CR>
            bool sensor_string_complete = true; // set the flag
        }
    }
}