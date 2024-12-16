#ifndef CLIENT_MANAGER_H
#define CLIENT_MANAGER_H

#include <WiFiClientSecure.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <WiFiManager.h>
class ClientManager
{
private:
    const char* MQTT_BROKER; 
    int MQTT_PORT;
    const char* CLIENT_ID;
    PubSubClient client;
    WiFiClientSecure wiFiClient;
    const char* PUBLISH_TOPIC;
    const char* SUBSCRIBE_TOPIC;
public:
    ClientManager(const char* brokerDomain, int portBroker, const char* clientId, const char* publishTopic, const char* subscribeTopic) : client(wiFiClient){
      this->MQTT_BROKER = brokerDomain;
      this->MQTT_PORT = portBroker;
      this->CLIENT_ID = clientId;
      this->PUBLISH_TOPIC = publishTopic;
      this->SUBSCRIBE_TOPIC = subscribeTopic;
    };

    void setUpCredentials(const char* AMAZON_ROOT_CA1, const char* CERTIFICATE, const char* PRIVATE_KEY){
        wiFiClient.setCACert(AMAZON_ROOT_CA1);
        wiFiClient.setCertificate(CERTIFICATE);
        wiFiClient.setPrivateKey(PRIVATE_KEY);
    }

    void setupWiFi() {
        WiFiManager wm;
        const char* apName = "IntruderAlarmAP";
        const char* apPassword = "admin123";
        if (!wm.autoConnect(apName, apPassword)) {
            Serial.println("No se pudo conectar a WiFi. Reiniciando...");
            ESP.restart();
        }
        Serial.print("Connected to WiFi. IP address: ");
        Serial.println(WiFi.localIP());
    }

    void reconnect() {
        while (!client.connected()) {
            Serial.print("Attempting MQTT connection...");
            if (client.connect(CLIENT_ID)) {
            Serial.println("connected");
            client.subscribe(SUBSCRIBE_TOPIC);
            Serial.println("Subscribed to " + String(SUBSCRIBE_TOPIC));
            delay(100);
            } else {
            Serial.print("failed, rc=");
            Serial.print(client.state());
            Serial.println(" trying again in 5 seconds");
            delay(5000);
            }
        }
    }

    bool isConeected(){
        return client.connected();
    }

    void listenMessages(){
        client.loop();
    }

    void setMqttConnection(MQTT_CALLBACK_SIGNATURE){
        setupWiFi();
        client.setServer(MQTT_BROKER, MQTT_PORT);
        client.setCallback(callback);
    }

    void publish(char* message){
      client.publish(PUBLISH_TOPIC, message);
    }
};

#endif
