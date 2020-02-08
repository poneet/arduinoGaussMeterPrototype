/*

  HelloWorld.ino
  
  "Hello World" version for U8x8 API

  Universal 8bit Graphics Library (https://github.com/olikraus/u8g2/)

  Copyright (c) 2016, olikraus@gmail.com
  All rights reserved.

  Redistribution and use in source and binary forms, with or without modification, 
  are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this list 
    of conditions and the following disclaimer.
    
  * Redistributions in binary form must reproduce the above copyright notice, this 
    list of conditions and the following disclaimer in the documentation and/or other 
    materials provided with the distribution.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND 
  CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, 
  INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF 
  MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE 
  DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR 
  CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT 
  NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; 
  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER 
  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, 
  STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF 
  ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.  

*/

#include <Arduino.h>
#include <U8x8lib.h>

#ifdef U8X8_HAVE_HW_SPI
#include <SPI.h>
#endif

// U8x8 Contructor List 
// The complete list is available here: https://github.com/olikraus/u8g2/wiki/u8x8setupcpp
// Please update the pin numbers according to your setup. Use U8X8_PIN_NONE if the reset pin is not connected
U8X8_SSD1306_128X32_UNIVISION_SW_I2C u8x8(/* clock=*/ 21, /* data=*/ 20, /* reset=*/ U8X8_PIN_NONE);   // Adafruit Feather M0 Basic Proto + FeatherWing OLED


// Macro used for determining a mag field polarity
#define sign(x) ((x) < 0 ? -1 : ((x) > 0 ? 1 : 0)) // https://forum.arduino.cc/index.php?topic=627179.0


// Analog Inputs
#define vma313_S 0   // A0 analog input pin <--- vma313_S-pin
#define a1302_DATA 1 // A1 analog input pin <--- a1302_DATA pin


#define a1302_quiescentVOLTAGE 2.48 // Manually read from sensor and placed here
float a1302_Voltage = 0;             // init
float a1302_deltaVoltage = 0;        // from quiescent voltage

// Determined by specifications provided by Allegro Microsystems (likely not accurate for all a1302 sensors).
// REFERENCE: http://robotics.itee.uq.edu.au/~elec3004/laboratories/LeviLab/Hall%20Effect%20Sensor%20A1301-2-Datasheet.pdf
#define a1302_allegroSENSITIVITY (1.3/1000)*0.2  // Volts per Gauss (p.4) with a 0.2 eyeball correction for this version using Velleman vma313 sensor (30G trigger)
int a1302_allegroGauss = 0;                      // Calculated Below
int a1302_allegroPolarity = 0;                   // init determined below
String a1302_allegroPolarityString = "";         // init determined below


// Determined by Arduino documentation.
// Voltage Scale: +5V
// MAX:1023 units of resolution
// REFERENCE: https://www.arduino.cc/en/Tutorial/AnalogInputPins
#define analogVoltageScale 5
#define analogVoltageResolution 1023


void setup(void)
{
  u8x8.begin();
  u8x8.setPowerSave(0);

 //Serial.begin(9600); // used for debugging
}

void loop(void)
{
  //Serial.println( analogRead(vma313_S) );   // used for debugging
  //Serial.println( analogRead(a1302_DATA) ); // used for debugging
  //
  //   _______
  //  |       |============
  //  | a1302 |============
  //  |_______|============
  //
  //
  //
  //
  // a1302: Read Voltage from the a1302 sensor
  //
  //
  a1302_Voltage = (float)( analogRead(a1302_DATA) ); // float casted analog pin voltage

  //
  //
  // a1302: Voltage value processing
  //
  //
  a1302_Voltage = a1302_Voltage * analogVoltageScale / analogVoltageResolution;
  a1302_deltaVoltage = (a1302_Voltage-a1302_quiescentVOLTAGE);
  a1302_deltaVoltage = ((int)(a1302_deltaVoltage * 100)/100.0); // truncate float after 2 decimal places (https://forum.arduino.cc/index.php?topic=488560.0)

  //
  //
  // a1302: Magnetic Field quantities
  //
  //
  // Determines the magnitude of the magnetic field based on Allegro Microsystems datasheet (see above)
  a1302_allegroGauss = (int)( (abs(a1302_deltaVoltage)) / a1302_allegroSENSITIVITY );

  // Determines North/South Polarity of the magnetic field
  a1302_allegroPolarity = sign(a1302_deltaVoltage);

  switch (a1302_allegroPolarity) {
    case 0:
    a1302_allegroPolarityString = " ";
    break;
    case 1:
    a1302_allegroPolarityString = "S";
    break;
    case -1:
    a1302_allegroPolarityString = "N";
    break;
    }

  //
  //          +---+---------------------+
  //  ========| o |                     |
  //  ========| o |  ssd1306(128x32)    |
  //  ========| o |  OLED Display       |
  //  ========| o |                     |
  //          +-------------------------+
  //
  //
  //
  // ssd1306(128x32): OLED Display
  //
  //
  u8x8.setFont(u8x8_font_chroma48medium8_r);
  u8x8.drawString(0,0,"a1302 Sensor");

  //
  //
  // ssd1306(128x32): Voltage
  //
  //
  // Voltage (value)
  u8x8.setCursor(0,2);
  u8x8.print( a1302_Voltage );

  // Voltage (units)
  u8x8.setCursor(8,2);
  u8x8.print( "Volts" );

  //
  //
  // ssd1306(128x32): Magnetic Field
  //
  //
  // Magnetic Field (magnitude)
  u8x8.setCursor(0,3);
  u8x8.print ( a1302_allegroGauss );

  // Magnetic Field (direction)
  u8x8.setCursor(5,3);
  u8x8.print( a1302_allegroPolarityString );

  // Magnetic Field (unit)
  u8x8.setCursor(8,3);
  u8x8.print( "Gauss" );
  
  //
  //
  // 
  //
  //
  delay(1000);

  // Clear the Magnetic Field magnitude so digits don't stick around, Gauss reading blinks
  u8x8.setCursor(0,3);
  u8x8.print( "     " );
}
