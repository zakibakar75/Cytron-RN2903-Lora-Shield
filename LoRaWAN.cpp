/*! \file LoRaWAN.cpp
    \brief Library for managing the Haxiot Arduino LoRaWAN shield v.1.0
    
    Copyright (C) 2016 Haxiot t/a Iotek Systems, LLC 
    http://www.haxiot.com
 
    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.
   
    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even an implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.
  
    You should have received a copy of the GNU Lesser General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
  
    Version:		1.0
    Authors:		Jim Wallace, Nik Kitson

    Update 5 February    Zaki   Modification and Addition for SCPF

 */

#include <Arduino.h>
#include "LoRaWAN.h"
#include <SoftwareSerial.h>

byte printResponse;

LoRaWAN::LoRaWAN(byte RX, byte TX):loraSerial(RX,TX)         // RX, TX   ** Set to 10, 11 for Mega2560 boards
{
 loraSerial.begin(57600);
}

//***************************************************************************
void LoRaWAN::LoRaDeviceEUI()
{
  loraSerial.println(F("sys get hweui"));      //get module version
  waitCommandResponse();
  loraSerial.print(F("mac set deveui "));loraSerial.println(_rxBuffer);
  waitCommandResponse();
  loraSerial.println(F("mac get deveui"));
  waitCommandResponse();
  Serial.print(F("Device EUI Set To: "));Serial.print(_rxBuffer);
}

//***************************************************************************
void LoRaWAN::LoRaDevAddr(char* address)
{
  loraSerial.print(F("mac set devaddr "));loraSerial.println(address);
  waitCommandResponse();
  loraSerial.println(F("mac get devaddr"));
  waitCommandResponse();
  Serial.print(F("Device Address Set To: "));Serial.print(_rxBuffer);
}

//****************************************************************************
void LoRaWAN::LoraApplicationEUI(char* EUI)
{
  loraSerial.print(F("mac set appeui "));loraSerial.println(EUI);
  waitCommandResponse();
  loraSerial.println(F("mac get appeui"));
  waitCommandResponse();
  Serial.print(F("Application EUI Set To: "));Serial.print(_rxBuffer);
}

//****************************************************************************
void LoRaWAN::LoRaNetworkSessionKey(char* nKey)
{
  loraSerial.print(F("mac set nwkskey "));loraSerial.println(nKey);
  waitCommandResponse(); 
  Serial.print(F("Network Session Key Set To: "));Serial.println(nKey);

}

//*****************************************************************************
void LoRaWAN::LoRaApplicationSessionKey(char* aKey)
{
  loraSerial.print(F("mac set appskey "));loraSerial.println(aKey);
  waitCommandResponse();
  Serial.print(F("Application Session Key Set To: "));Serial.println(aKey);
}

//******************************************************************************
void LoRaWAN::LoRaApplicationKey(char* aKey)
{
  loraSerial.print(F("mac set appkey "));loraSerial.println(aKey);
  waitCommandResponse();
  Serial.print(F("Application Key Set To: "));Serial.println(aKey);
}

//********************* configure ABP ************************************************
void LoRaWAN::LoRaJoinABP()
{
  loraSerial.println(F("mac join abp"));
  Serial.println(F("Joined ABP"));
  waitCommandResponse();
}

//************************ configure OTAA ************************************
void LoRaWAN::LoRaJoinOTAA()
{
  loraSerial.println(F("mac join otaa"));
  Serial.println(F("Joined OTAA"));
  waitCommandResponse();
}
//********************************************************************************
void LoRaWAN::LoRaSaveConfig()
{
  loraSerial.println(F("mac save"));
  waitCommandResponse();
}

//*********************************************************************************
void LoRaWAN::LoRaAdaptiveDataRate(byte mode)
{
  if (mode)
    {
      loraSerial.println(F("mac set adr on"));
      Serial.println(F("ADR is enabled"));
    }
  else
    {
      loraSerial.println(F("mac set adr off"));
      Serial.println(F("ADR is disabled"));
    }
  waitCommandResponse();
}

//*********************************************************************************
void LoRaWAN::LoRaTXPower(byte power)
{
  byte index;
  switch (power){
    case 10:
          index = 10;
          break;
    case 12:
          index = 9;
          break;
    case 14:
          index = 8;
          break;
    case 16:
          index = 7;
          break;
    case 18:
          index = 6;
          break;
    case 20:
          index = 5;
          break;
    default:
          index = 8;
          break;                
  }
  loraSerial.print(F("mac set pwridx "));loraSerial.println(index);
  waitCommandResponse();
  loraSerial.println(F("mac get pwridx"));
  waitCommandResponse();
  byte pwrIndex =atoi(_rxBuffer);
  switch (pwrIndex){
    case 10:
          index = 10;
          break;
    case 9:
          index = 12;
          break;
    case 8:
          index = 14;
          break;
    case 7:
          index = 16;
          break;
    case 6:
          index = 18;
          break;
    case 5:
          index = 20;
          break;
    default:
          index = 0;
          break;                
  }
  Serial.print(F("TX Power Set To: "));Serial.print(index);Serial.println(F("dBM"));
 }

//********************* configure channels on or off ******************************
void LoRaWAN::LoRaChannelPlan(byte st_ch, byte end_ch)
{
  
  for (int x=0; x<72; x++)    //turn all channels off
  {
    loraSerial.print(F("mac set ch status "));loraSerial.print(x);loraSerial.println(F(" off"));
    waitCommandResponse();
  }

  for (int x=st_ch; x<end_ch+1; x++)  //turn on channels between start and end range
  {
    loraSerial.print(F("mac set ch status "));loraSerial.print(x);loraSerial.println(F(" on"));
    waitCommandResponse();
  }
  
} 

//****************************************************************************************
void LoRaWAN::LoRaResetModule()
{
  loraSerial.println(F("sys factoryRESET"));
  waitCommandResponse();
}

//******************** wait for response from module *************************************
void LoRaWAN::waitCommandResponse()
{
  unsigned long currentTime,startTime;
  byte c=0;
  memset(_rxBuffer,0x00,64);    //clear buffer
  startTime = millis(); 
  while (loraSerial.available() == 0)
  {
    currentTime = millis();
    if (currentTime > (startTime + 3000))       //exit wait routine if nor response in 3 seconds
    {
      Serial.println(F("No Resposnse from Lora Module"));
      return;
    }
  }
  
  while (loraSerial.available()) 
    {
      _rxBuffer[c] = loraSerial.read();
      c++;
    }
   if (_rxBuffer[0]=='o' && _rxBuffer[1]=='k')     //check if response is ok
    {
      return; 
    }
   if (printResponse)
   {
      Serial.print(_rxBuffer);
   }
}

//************************ get configuration information from module *********************
void LoRaWAN::LoRaShowConfig()
{
  float ch_freq;
  long freq;
  int dRange;
  int sFactor;
  byte index;
  
  loraSerial.println(F("sys get ver"));      //get module version
  waitCommandResponse();
  Serial.print(F("Module Version: "));Serial.print(_rxBuffer);
   
  loraSerial.println(F("mac get deveui"));
  waitCommandResponse();
  Serial.print(F("Device EUI Set To: "));Serial.print(_rxBuffer);

  loraSerial.println(F("mac get appeui"));
  waitCommandResponse();
  Serial.print(F("Application EUI Set To: "));Serial.print(_rxBuffer);

  loraSerial.println(F("mac get devaddr"));
  waitCommandResponse();
  Serial.print(F("Device Address Set To: "));Serial.print(_rxBuffer);
  
  loraSerial.println(F("mac get pwridx"));
  waitCommandResponse();
  byte pwrIndex =atoi(_rxBuffer);
  switch (pwrIndex){
    case 10:
          index = 10;
          break;
    case 9:
          index = 12;
          break;
    case 8:
          index = 14;
          break;
    case 7:
          index = 16;
          break;
    case 6:
          index = 18;
          break;
    case 5:
          index = 20;
          break;
    default:
          index = 0;
          break;                
  }
  Serial.print(F("TX Power Set To: "));Serial.print(index);Serial.println(F("dBM"));
  
  for (int x=0; x<72; x++)   //cover all available channel in AU band
  {
    loraSerial.print(F("mac get ch status "));loraSerial.println(x);      //get channel status
    waitCommandResponse();

    if (_rxBuffer[0]=='o' && _rxBuffer[1]=='n')     //check if channel is on, acquire rest of info
    {
        /************Enable this if using Real LoraWAN Gateway**************************************************/
        //loraSerial.print(F("mac get ch drrange "));loraSerial.println(x);      //get channel data range
        //waitCommandResponse();
        //dRange=0;
        //if (_rxBuffer[0]=='0' && _rxBuffer[2]=='3')
        //{
            //dRange = 125;
        //}
        //else if (_rxBuffer[0]=='4' && _rxBuffer[2]=='4')
        //{
             //dRange = 500;
        //}
        /*******************************************************************************************************/

        /*************Enable this if using Single Channel Forwarder where we force to use SF7*******************/
        loraSerial.print(F("mac set ch drrange "));loraSerial.print(x);loraSerial.print(" 3");loraSerial.println(" 3");  //Force using DR3 (SF7)
        waitCommandResponse();

        loraSerial.println(F("mac save"));
        waitCommandResponse();

        loraSerial.print(F("mac get ch drrange "));loraSerial.println(x);  //read back to confirm
        waitCommandResponse();
        
        dRange = 0;
        sFactor=0;
        if (_rxBuffer[0]=='3' && _rxBuffer[2]=='3')  //check here
        {
          sFactor=7;
          dRange=125;
        }
        /**************************************************************************************************************/
        
        loraSerial.print(F("mac get ch freq "));loraSerial.println(x);  //get channel frequency
        waitCommandResponse();
        ch_freq = atol(_rxBuffer)/1000000.0;
        Serial.print(F("Channel "));Serial.print(x);Serial.print(F(": Frequency = "));Serial.print(ch_freq);Serial.print(F("Mhz, DataRate = "));Serial.print(dRange);Serial.print(F("kHz"));Serial.print(F(": SF = "));Serial.println(sFactor);
    }
   }

  Serial.println();
}

void LoRaWAN::LoRaTransmit(byte _type, byte portno, char* data)
{
  printResponse = 1;
    
  loraSerial.print(F("mac tx "));
  if (_type == 1)
    {
      loraSerial.print(F("uncnf "));
    }
  else
    {
      loraSerial.print(F("cnf "));
    }
  loraSerial.print(portno);loraSerial.print(" ");loraSerial.println(data);      //tx data
  waitCommandResponse();
 
  loraSerial.println(F("radio get freq"));  //get channel frequency (we expect 1 channel only for SCPF case)
  waitCommandResponse();
  loraSerial.println(F("radio get sf"));    //get SF (we expect SF7 only for SCPF case)
  waitCommandResponse();
  
  printResponse = 0;
}
