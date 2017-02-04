/*
  LoRaWAN - Library for controlling RN2903
  Created by Iotek Systems, LLC t/a Haxiot
  May 9, 2016.
*/
#ifndef LoRaWAN_h
#define LoRaWAN_h

#include <Arduino.h>
#include <SoftwareSerial.h>

class LoRaWAN
{
  public:
    LoRaWAN(byte RX, byte TX);
    void LoRaJoinABP();
    void LoRaJoinOTAA();
    void LoRaChannelPlan(byte st_ch, byte end_ch);
    void waitCommandResponse();
    void LoRaShowConfig();
    void LoRaDeviceEUI();
    void LoRaDevAddr(char* address);
    void LoraApplicationEUI(char* EUI);
    void LoRaNetworkSessionKey(char* nKey);
    void LoRaApplicationSessionKey(char* aKey);
    void LoRaApplicationKey(char* aKey);
    void LoRaSaveConfig();
    void LoRaAdaptiveDataRate(byte mode);
    void LoRaTXPower(byte power);
    void LoRaResetModule();
    void LoRaTransmit(byte _type, byte portno, char* data);
            
    SoftwareSerial loraSerial;
  private:
    char _rxBuffer[100];
    
};


#endif
