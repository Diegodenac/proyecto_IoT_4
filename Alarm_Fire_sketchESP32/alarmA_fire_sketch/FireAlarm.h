#ifndef FIRE_ALARM_H
#define FIRE_ALARM_H

#include "ClientManager.h"
#include <ArduinoJson.h>

class FireAlarm
{
private:
    ClientManager* networkManager;
    unsigned char valvePin;
    unsigned char typePin;
    unsigned char ppmPin;
    unsigned char valveState;
    const char* typeGas;
    int ppmConcetration;
    StaticJsonDocument<JSON_OBJECT_SIZE(4)> outputDoc;
    char outputBuffer[128];
public:
    FireAlarm(ClientManager* manager, unsigned char attachValvePin, unsigned char attachTypePin, unsigned char attachPpmPin)
      : networkManager(manager),
        valvePin(attachValvePin),
        typePin(attachTypePin),
        ppmPin(attachPpmPin),
        valveState(0),
        typeGas("air"),
        ppmConcetration(0) {
    }
    void reportTypeAndPPM(){
        outputDoc["state"]["reported"]["detected_type"] = typeGas;
        outputDoc["state"]["reported"]["ppm_level"] = ppmConcetration;
        serializeJson(outputDoc, outputBuffer);
        networkManager->publish(outputBuffer);
    }
    void reportValveActivation(){
        outputDoc["state"]["reported"]["valve_state"] = valveState;
        serializeJson(outputDoc, outputBuffer);
        networkManager->publish(outputBuffer);
    }
    void setValveActivation(byte payloadValve){
        if(valveState != payloadValve){
            valveState = payloadValve;
            valveState ? digitalWrite(valvePin, HIGH) : digitalWrite(valvePin, LOW);
            Serial.println(valveState);
            reportValveActivation();
        }
    }
    void determineType(){
      int valueType = analogRead(typePin);
      typeGas = valueType < 300 ? "smoke" : "air";
      typeGas = valueType < 170 ? "gas" : typeGas;
    }
    void readDetection(){
      determineType();
      ppmConcetration = analogRead(ppmPin);
      Serial.println("Type: " + String(typeGas));
      Serial.println("PPM: " + String(ppmConcetration));
    }
};

#endif
