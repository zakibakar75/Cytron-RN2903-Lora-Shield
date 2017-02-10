/* 5 Feb 2017   Zaki   Support for Single Channel Packet Forwarder */

#include "LoRaWAN.h"
#include <SoftwareSerial.h>
#include<dht.h>

#define ON 1
#define OFF 0
#define MAX_HEX_DIGIT 2
#define DHT11_PIN 8


//************** Device parameters for registration *****************
char DEVICE_ADDR[] = "2602125E";  //4 bytes required 
char NWK_SESSION_KEY[] = "06E635B6E76E02DD8BFBFC0B268F3199"; // 16 bytes required
char APP_SESSION_KEY[] = "745919635797A64960F3C045EF6DB336"; //16 bytes required
char APP_KEY[] = "0AA5AF9F0B74FF519D7FB0FF57D3EB9C"; //16 bytes required
char APP_EUI[] =  "70B3D57EF00038C7";  //8 bytes required

LoRaWAN LoRaWAN(2,3); // Software Serial RX, TX   ** Set to 10, 11 for Mega2560 boards, Set to 5, 4 for Uno 
dht DHT;

/* Function Prototype */
void ConvertResult (uint8_t passedResult, char* buf);
void AssignToCharArray(int startArrayNum, char* txData, char* tempData);


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

  int i;
  String str;
  char tempData[3];
  char txData[7];   //data to be transmitted - must be hexadecimal values
  uint8_t result,chk,humidity,temperature;
  
  //*************** Display received data from LoRaWAN 
  while (LoRaWAN.loraSerial.available()) {
    Serial.write(LoRaWAN.loraSerial.read());
  }
  
  chk = DHT.read11(DHT11_PIN);
  Serial.println("Humidity " );
  Serial.println(DHT.humidity, 1);
  Serial.println("Temperature ");
  Serial.println(DHT.temperature, 1);
  
  /************************** reading 1 **************************/
  result=DHT.temperature;
  ConvertResult(result, tempData);
  AssignToCharArray(0, txData, tempData);   
  /***************************************************************************/
 
  /************************* reading 2 ****************************/
  result=DHT.humidity;
  ConvertResult(result,tempData);
  AssignToCharArray(2, txData, tempData);

  result=0xAA;
  ConvertResult(result,tempData);
  AssignToCharArray(4, txData, tempData);
  /***************************************************************************/
  
  txData[6] = '\0';
              
  //*************** Display then transmit data to LoRaWAN ********************/
  Serial.print("\nSending: ");Serial.println(txData);
  LoRaWAN.LoRaTransmit(1, 30, txData);   //type 0 = Confirmed, 1 = Unconfirmed / port number 1 to 223 / data payload
   
  delay(10000);
  
}

void ConvertResult(uint8_t passedResult, char* buffer)
{
  String str;
  str=String(passedResult,HEX);
  str.toCharArray(buffer,3);
}

void AssignToCharArray(int startArrayNum, char* txData, char* tempData)
{  
  int i;
  
  for(i=startArrayNum; i<(MAX_HEX_DIGIT+startArrayNum); i++)
  {        
    Serial.print("Array ");Serial.print(i);Serial.print(": ");
    
    if(isDigit(tempData[i-startArrayNum]) || isAlpha(tempData[i-startArrayNum]))
    {
      Serial.print("Digit/Aplha : "); 
      Serial.println(tempData[i-startArrayNum]);
      txData[i] = tempData[i-startArrayNum];  // assign tempData to txData respectively
    }
    else
    {
      Serial.print("No Digit/Aplha on 2nd tempData buffer means Need to push '0' to the first txData buffer");
      //Supposedly tempData will always have 2 digits because of HEX, but 
      //in case of 1-F, it is a single digit, since txData expects 2 digit, 
      //we force putting '0' in its first buffer.
      txData[i] = txData[i-1];  //assign the last buffer element to hold the digit
      txData[i-1] = '0';        //hardcode the first element to old a value of '0'
      
    }         
  }
}
