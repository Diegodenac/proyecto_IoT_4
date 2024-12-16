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
    const char* typeGasReported;
    int ppmConcetration;
    int ppmConcetrationReported;
    unsigned char fireUmbralPpm;
    unsigned char reportsUmbralPpm;
    int fireDetected;
    StaticJsonDocument<JSON_OBJECT_SIZE(4)> outputDoc;
    char outputBuffer[512];
public:
    FireAlarm(ClientManager* manager, unsigned char attachValvePin, unsigned char attachTypePin, unsigned char attachPpmPin)
      : networkManager(manager),
        valvePin(attachValvePin),
        typePin(attachTypePin),
        ppmPin(attachPpmPin),
        valveState(0),
        typeGas("air"),
        ppmConcetration(0),
        typeGasReported(""),
        ppmConcetrationReported(0),
        fireUmbralPpm(120),
        reportsUmbralPpm(15),
        fireDetected(0) {
    }
    void reportTypeAndPPM(){
        memset(outputBuffer, 0, sizeof(outputBuffer));
        outputDoc["state"]["reported"]["detected_type"] = typeGas;
        outputDoc["state"]["reported"]["ppm_level"] = ppmConcetration;
        serializeJson(outputDoc, outputBuffer);
        networkManager->publish(outputBuffer);
        typeGasReported = typeGas;
        ppmConcetrationReported = ppmConcetration;
    }
    bool reportValid(){
      int diferencePpmSendAndRead = (ppmConcetration-ppmConcetrationReported) < 0 ? -(ppmConcetration-ppmConcetrationReported) : (ppmConcetration-ppmConcetrationReported);
      //if(typeGas != typeGasReported || diferencePpmSendAndRead > reportsUmbralPpm) Serial.println("REPORTADO");
      //else Serial.println("NO REPORTADO");
      return typeGas != typeGasReported || diferencePpmSendAndRead > reportsUmbralPpm;
      //Serial.println("GasDetected: " + String(typeGas));
      //Serial.println("GasReported: " + String(typeGasReported));
      //Serial.println("ppmConcetration: " + String(ppmConcetration));
      //Serial.println("ppmConcetrationReported: " + String(ppmConcetrationReported));
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
            //Serial.println(valveState);
            reportValveActivation();
        }
    }
    void reportFireUmbralPpm(){
        outputDoc["state"]["reported"]["fire_umbral_ppm"] = fireUmbralPpm;
        serializeJson(outputDoc, outputBuffer);
        networkManager->publish(outputBuffer);
    }
    void setFireUmbralPpm(byte payloadFireUmbral){
        if(fireUmbralPpm != payloadFireUmbral){
              fireUmbralPpm = payloadFireUmbral;
              //Serial.println(fireUmbralPpm);
              reportValveActivation();
        }
    }
    void reportReportsUmbralPpm(){
        outputDoc["state"]["reported"]["reports_umbral_ppm"] = reportsUmbralPpm;
        serializeJson(outputDoc, outputBuffer);
        networkManager->publish(outputBuffer);
    }
    void setReportsUmbralPpm(byte payloadReportsUmbral){
        if(reportsUmbralPpm != payloadReportsUmbral){
              reportsUmbralPpm = payloadReportsUmbral;
              //Serial.println(reportsUmbralPpm);
              reportReportsUmbralPpm();
        }
    }
    void reportFireDetection(){
      outputDoc["state"]["reported"]["fire_detected"] = fireDetected;
      serializeJson(outputDoc, outputBuffer);
      networkManager->publish(outputBuffer);
    }
    void determineType(){
      int valueType = analogRead(typePin);
      Serial.println("Type Value: " + String(valueType));
      typeGas = valueType < 400 ? "smoke" : "air";
    }
    bool isFireDetected(){
      return typeGas == "smoke" && ppmConcetration > fireUmbralPpm;
    }
    void readDetection(){
      determineType();
      ppmConcetration = analogRead(ppmPin);
      unsigned char fireDetectedStatus = isFireDetected();
      if(fireDetectedStatus != fireDetected){
        fireDetected = fireDetectedStatus;
        Serial.println("Fire Detected Reportado: " + String(fireDetected));
        reportFireDetection();
      }
      //Serial.println("Type: " + String(typeGas));
      Serial.println("PPM: " + String(ppmConcetration));
    }
};

#endif
