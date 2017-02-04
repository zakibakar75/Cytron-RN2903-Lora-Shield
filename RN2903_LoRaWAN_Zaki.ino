/* 5 Feb 2017   Zaki   Support for Single Channel Packet Forwarder */

#include "LoRaWAN.h"
#include <SoftwareSerial.h>

#define ON 1
#define OFF 0

//************** Device parameters for registration *****************
char DEVICE_ADDR[] = "26021BF2";  //4 bytes required
char NWK_SESSION_KEY[] = "C66C9E1F3D7D6E7DE9A84A7E94704C3F"; // 16 bytes required
char APP_SESSION_KEY[] = "CAA5C87D0A6A7E82CBA09E37F09EA117"; //16 bytes required
char APP_KEY[] = "0AA5AF9F0B74FF519D7FB0FF57D3EB9C"; //16 bytes required
char APP_EUI[] =  "70B3D57EF0001D5E";  //8 bytes required

char txData[]="01020304050";      //sample data to be transmitted - must be hexadecimal values

LoRaWAN LoRaWAN(2,3); // Software Serial RX, TX   ** Set to 10, 11 for Mega2560 boards, Set to 5, 4 for Uno 


void setup() {

delay(1000);               //startup delay - gives Lora module time to reset if cold start
Serial.begin(9600);        //terminal serial port, 9600 = Default, 57600 = Haxiot shield

//LoRaWAN.LoRaResetModule();                          //restore module to factory default settings and reset device
LoRaWAN.LoRaDeviceEUI();                            //retrieves EUI from module and auto sets, not needed for ABP
LoRaWAN.LoRaDevAddr(DEVICE_ADDR);                   //sets device addresss
LoRaWAN.LoraApplicationEUI(APP_EUI);                //sets application EUI
LoRaWAN.LoRaNetworkSessionKey(NWK_SESSION_KEY);     //sets network session key
LoRaWAN.LoRaApplicationSessionKey(APP_SESSION_KEY); //sets application session key
//LoRaWAN.LoRaApplicationKey(APP_KEY);                //sets application key, not needed for ABP
LoRaWAN.LoRaAdaptiveDataRate(OFF);      //set to ON to enable, OFF to disable
LoRaWAN.LoRaTXPower(20);                //dBM 10(Min), 12, 14, 16, 18, 20(Max)
LoRaWAN.LoRaChannelPlan(24,24);         //(start channel, end channel) - channel range 0 to 71
LoRaWAN.LoRaSaveConfig();               //save configuration settings to module eeprom

//************* Connect to LoRaWAN Network Server****************//
LoRaWAN.LoRaShowConfig();    //display current module configuration for debugging only
// Comment out 
LoRaWAN.LoRaJoinABP ();       //Connect with Access By Personalization (ABP) networks
//LoRaWAN.LoRaJoinOTAA ();    //Connect with Over The Air Activation (OTAA) networks 
delay(1000);
}

void loop() {
 
  //*************** Display received data from LoRaWAN 
  while (LoRaWAN.loraSerial.available()) {
    Serial.write(LoRaWAN.loraSerial.read());
  }
            
  //*************** Display then transmit data to LoRaWAN
  Serial.print("\nSending: ");Serial.println(txData);
  LoRaWAN.LoRaTransmit(1, 30, txData);   //type 0 = Confirmed, 1 = Unconfirmed / port number 1 to 223 / data payload
  delay(10000);

}

