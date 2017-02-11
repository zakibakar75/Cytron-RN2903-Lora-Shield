/* 5 Feb 2017   Zaki   Support for Single Channel Packet Forwarder */

#include "LoRaWAN.h"
#include <SoftwareSerial.h>
#include<dht.h>

#define ON 1
#define OFF 0
#define MAX_HEX_DIGIT 2
#define DHT11_PIN 8  //change as needed

//Replace all the Info below with the infos you got from your own TTN App
//************** Device parameters for registration *****************
char DEVICE_ADDR[] = "26XXXXXX";  //4 bytes required 
char NWK_SESSION_KEY[] = "06E635B6E76E02DD8BFBFC0B268FXXXX"; // 16 bytes required
char APP_SESSION_KEY[] = "745919635797A64960F3C045EF6DXXXX"; //16 bytes required
char APP_KEY[] = "0AA5AF9F0B74FF519D7FB0FF57D3EB9C"; //16 bytes required
char APP_EUI[] =  "70B3D57EF000XXXX";  //8 bytes required

LoRaWAN LoRaWAN(2,3); // Software Serial RX,TX. Cytron board uses these ports. 
dht DHT;

/* Function Prototype */
void ConvertResult (uint8_t passedResult, char* tempData);
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
  char tempData[3]; //buffer to hold char that represent the Hex number
  char txData[7];   //data to be transmitted - must be hexadecimal values.  Adjust the number as needed.
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
  result=DHT.temperature;  //Get the temp data
  ConvertResult(result, tempData);
  AssignToCharArray(0, txData, tempData);   //note the position of the txData array element. Here is 0.
  /***************************************************************************/
 
  /************************* reading 2 ****************************/
  result=DHT.humidity; //Get the humidity data
  ConvertResult(result,tempData);
  AssignToCharArray(2, txData, tempData);  //note the position of the txData array element. Here is 2.
  
  //result below is just a dummy
  result=0xAA;
  ConvertResult(result,tempData);
  AssignToCharArray(4, txData, tempData);  //note the position of the txData array element. Here is 4.
  /***************************************************************************/
  
  txData[6] = '\0';  //to say that we are done
              
  //*************** Display then transmit data to LoRaWAN ********************/
  Serial.print("\nSending: ");Serial.println(txData);
  LoRaWAN.LoRaTransmit(1, 30, txData);   //type 0 = Confirmed, 1 = Unconfirmed / port number 1 to 223 / data payload
   
  delay(10000);
  
}

void ConvertResult(uint8_t passedResult, char* tempData)
{
  String str;
  str=String(passedResult,HEX); //convert uint8_t into a String in HEX format (eg 255 becomes FF)
  str.toCharArray(tempData,3); //put FF (example) into the char array so it will occupy 2 array elements. 
                               //the 3rd array element is just a dummy.
}

void AssignToCharArray(int startArrayNum, char* txData, char* tempData)
{  
  int i;
  //we keep track which array element of txData that we want to touch
  //here, the startArrayNum is passed from the calling function
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
      Serial.print("No Digit/Aplha on 2nd tempData buffer means need to push '0' to the first txData buffer");
      //Supposedly tempData will always have 2 digits because of HEX, but 
      //in case of 1-F, it is a single digit, since txData expects 2 digit, 
      //we force putting '0' in txData first element passed.
      txData[i] = txData[i-1];  //assign the last array element to hold the digit
      txData[i-1] = '0';        //hardcode the first element to hold a value of '0'
      
    }         
  }
}
