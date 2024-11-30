#include "IntruderAlarm.h"
#include "ClientManager.h"
#include <ArduinoJson.h>

// Replace with your network credentials
const char* WIFI_SSID = "Android4444";
const char* WIFI_PASS = "jdp269gaj";

// Replace with your MQTT broker details
const char* MQTT_BROKER = "a2gyx6o3r72s2g-ats.iot.us-east-1.amazonaws.com";
const int MQTT_PORT = 8883;
const char* CLIENT_ID = "ESP-32-INTRUDER-000";

const char AMAZON_ROOT_CA1[] PROGMEM = R"EOF(
-----BEGIN CERTIFICATE-----
MIIDQTCCAimgAwIBAgITBmyfz5m/jAo54vB4ikPmljZbyjANBgkqhkiG9w0BAQsF
ADA5MQswCQYDVQQGEwJVUzEPMA0GA1UEChMGQW1hem9uMRkwFwYDVQQDExBBbWF6
b24gUm9vdCBDQSAxMB4XDTE1MDUyNjAwMDAwMFoXDTM4MDExNzAwMDAwMFowOTEL
MAkGA1UEBhMCVVMxDzANBgNVBAoTBkFtYXpvbjEZMBcGA1UEAxMQQW1hem9uIFJv
b3QgQ0EgMTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBALJ4gHHKeNXj
ca9HgFB0fW7Y14h29Jlo91ghYPl0hAEvrAIthtOgQ3pOsqTQNroBvo3bSMgHFzZM
9O6II8c+6zf1tRn4SWiw3te5djgdYZ6k/oI2peVKVuRF4fn9tBb6dNqcmzU5L/qw
IFAGbHrQgLKm+a/sRxmPUDgH3KKHOVj4utWp+UhnMJbulHheb4mjUcAwhmahRWa6
VOujw5H5SNz/0egwLX0tdHA114gk957EWW67c4cX8jJGKLhD+rcdqsq08p8kDi1L
93FcXmn/6pUCyziKrlA4b9v7LWIbxcceVOF34GfID5yHI9Y/QCB/IIDEgEw+OyQm
jgSubJrIqg0CAwEAAaNCMEAwDwYDVR0TAQH/BAUwAwEB/zAOBgNVHQ8BAf8EBAMC
AYYwHQYDVR0OBBYEFIQYzIU07LwMlJQuCFmcx7IQTgoIMA0GCSqGSIb3DQEBCwUA
A4IBAQCY8jdaQZChGsV2USggNiMOruYou6r4lK5IpDB/G/wkjUu0yKGX9rbxenDI
U5PMCCjjmCXPI6T53iHTfIUJrU6adTrCC2qJeHZERxhlbI1Bjjt/msv0tadQ1wUs
N+gDS63pYaACbvXy8MWy7Vu33PqUXHeeE6V/Uq2V8viTO96LXFvKWlJbYK8U90vv
o/ufQJVtMVT8QtPHRh8jrdkPSHCa2XV4cdFyQzR1bldZwgJcJmApzyMZFo6IQ6XU
5MsI+yMRQ+hDKXJioaldXgjUkK642M4UwtBV8ob2xJNDd2ZhwLnoQdeXeGADbkpy
rqXRfboQnoZsG4q5WTP468SQvvG5
-----END CERTIFICATE-----
)EOF";

const char CERTIFICATE[] PROGMEM = R"KEY(
-----BEGIN CERTIFICATE-----
MIIDWTCCAkGgAwIBAgIUW99M7aX9O7FP7iKNOX7lGx/LeVUwDQYJKoZIhvcNAQEL
BQAwTTFLMEkGA1UECwxCQW1hem9uIFdlYiBTZXJ2aWNlcyBPPUFtYXpvbi5jb20g
SW5jLiBMPVNlYXR0bGUgU1Q9V2FzaGluZ3RvbiBDPVVTMB4XDTI0MTEyOTAyMTAx
M1oXDTQ5MTIzMTIzNTk1OVowHjEcMBoGA1UEAwwTQVdTIElvVCBDZXJ0aWZpY2F0
ZTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBAK8ME/n2c7rq/9sXNeTr
PaxczQWD8BfZ/tXACkEFNKDKRU707DHr5l5zl4ruKO+FQwPbyExNC+OiMGob2JS6
li/sEPDqQseJVSZIfIhtj1boXTQgYPPopXI0mxa2CJLgdGMe3skeXCjARSRm6JrH
5Y4ywOQmoLvutuJ+B4Weuzq17eSzl9hzIZBpKDM2iDvQeqNo2DyhoYkmfPLukngb
HQOHWMDm1Ic0r/rvfskobRwnEYxDdwh0iziDAQaf0sZ6ffBVddnWoZ+75eEUIP4/
K/ovqSNHk9b89tp0uOh95RKwrabG2c9EbHbtVy0rEcYN4y4INLASXMSUK2FEe9ft
WOcCAwEAAaNgMF4wHwYDVR0jBBgwFoAU6a7CaKfzeF/uvvmFOMHEQIXWoc4wHQYD
VR0OBBYEFAHDWKuOgYPmiRZ6wYXOtZMWILfsMAwGA1UdEwEB/wQCMAAwDgYDVR0P
AQH/BAQDAgeAMA0GCSqGSIb3DQEBCwUAA4IBAQCWryg7GjXH6fLYCPSxx15kRC24
F1SFnfZtSbHXYztWOtmiuVHPvRKBd8J3/QMf71RGoucokkLabfSfUYolOghlMcSJ
CN2mEjyVF/BVjQMtQv8dHhvXLy0L58N9T4XlkIHshK5eeRCTZEA1yd3GJv3QqQfz
eNu4xHhhlnKl+zYRE/qPzKp36Kwkpn/hgoG0qyjFreOjhzWQ9YnrXPAhCXQa8uyK
rtiTIOW7jOWgtJd+F3ZTmtGEmSypUg+uDV1MrePUv3wAHvuhR9XpZNSDjylZatQD
cSqdb7SkSp9MN3uiZeMhOSMz5PRyggAb1wC3TPKLSavbtNF92BAq/3R7TOWm
-----END CERTIFICATE-----
)KEY";

const char PRIVATE_KEY[] PROGMEM = R"KEY(
-----BEGIN RSA PRIVATE KEY-----
MIIEpAIBAAKCAQEArwwT+fZzuur/2xc15Os9rFzNBYPwF9n+1cAKQQU0oMpFTvTs
MevmXnOXiu4o74VDA9vITE0L46IwahvYlLqWL+wQ8OpCx4lVJkh8iG2PVuhdNCBg
8+ilcjSbFrYIkuB0Yx7eyR5cKMBFJGbomsfljjLA5Cagu+624n4HhZ67OrXt5LOX
2HMhkGkoMzaIO9B6o2jYPKGhiSZ88u6SeBsdA4dYwObUhzSv+u9+yShtHCcRjEN3
CHSLOIMBBp/Sxnp98FV12dahn7vl4RQg/j8r+i+pI0eT1vz22nS46H3lErCtpsbZ
z0Rsdu1XLSsRxg3jLgg0sBJcxJQrYUR71+1Y5wIDAQABAoIBAQCIlzEjVNdbnLJH
gKYSWinGzKdCzYBU7Es4+eBGekxlx1Ar21xPXBdnzmULCOoydNclNKPoPhqv5shG
lDH9ZFd7T4aspu+zCgvCkr9yKaO3GhxYZ4ef7ALlyfzWzPaFvdWz3svTPfiF5/pL
/G03EhKjUiFIJog3/X8B33v5HTJWDYPWgS5FpkOcXw4KOtYVftSd4RDRGwxCRYLU
w+6exjA44fX8ttIn6eajEgIeYXMZmIiHuwGqAjIsuFBefWuKMD43w8EQg1Kzu28p
64Ekq/TF0VFy71iy8qITbjkjuo6Mf55LusSW+hkfxz97v/bu4ioXdDamz0BrDXpt
soJch+GBAoGBANv6Vm+CSzKtB4QMPVDzdPPkQ15F07eo+7v47hCPIrv28WUmxQK9
ju4h+nDKqKB0E3l7JjjtC8MUjSFhxFPy94g8WfxWKKcYjsjUEyCB4nhiGFw1h99w
WTrZe71+AzJ+pFkCIpb594GLMZ1uf3OACmxttr0362n+dLlzGvkjfFbBAoGBAMu2
NOfdlvRBpTg8vCvxRjIEZT5xOxtaXmZbcwMPdQCp+jOf/ru7lJgRXwj0yOi9Yj35
I1KoDr0VLOYSltVdxHIdwiwYhmaAW3/PyurJ/8YKzqcR4L7ycIIcCIhjnh0GXRAW
97BZlvICiJp1IWpJjWDY2I6d5xDrFl8v5pm9+gGnAoGALHpCbct3buijY2SiJTME
i6e0o2QU+caWF5yubfSy/jRg/KXJIdO4CZgfLh0b2U1+/ooS4WG0hh4qpvxAmQTA
XfBt9WJqV9bUT/DpaSI560NST5qXPWtW71o39cl71gLbMhAHdGVYUP4W6xEDfhP+
h4vrbfjXhZfoe/K1Gni9VEECgYBGrKtEOjOSAGhiz2d1U1rIJZA+jqg0ADE6D8Yi
LK2qEk60tHB7D0QpUfWHtO0zGelImPPxTE74uK0AETj1Q3fRI5PnGocebq/P8vZy
aKxtN/ikK+auOIxiB3nKX81OQpPs7hr2us5MniZod/Sk3GnJTNItAIlW7Eixs3us
oSVohwKBgQCTECi4ycovpg+Y0ptLVOuYtsiH7CukzWGHyZ3cxOqG/u8lkplR9+Jh
2x/X9fMs6a4Dg6g1uHNTKBflPzLy38sNR0pyCaEpgR3JRcgG0SUoh/rkgXt6MU8b
nUAmaV7niv64H1KMwD1mfIQsWSOThG6B3En6OpyJof3k+qykQSg9yQ==
-----END RSA PRIVATE KEY-----
)KEY";

const char* UPDATE_TOPIC = "$aws/things/alarm_intruder_000/shadow/update";              // publish
const char* UPDATE_DELTA_TOPIC = "$aws/things/alarm_intruder_000/shadow/update/delta";  // subscribe

// Pin definitions
const int MOVEMENT_DETECTOR_PIN = 14;
const int BUZZER_PIN = 12;

//objetct instance
ClientManager* networkManager = new ClientManager(WIFI_SSID, WIFI_PASS, MQTT_BROKER, MQTT_PORT, CLIENT_ID, UPDATE_TOPIC, UPDATE_DELTA_TOPIC);
IntruderAlarm intruderAlarm(networkManager, MOVEMENT_DETECTOR_PIN, BUZZER_PIN);
// Callback function to handle messages received from the subscribed topic
byte payloadAlarm = 0;
byte payloadBuzzer = 0;
StaticJsonDocument<JSON_OBJECT_SIZE(64)> inputDoc;

void callback(char* topic, byte* payload, unsigned int length) {
  String message;
  for (int i = 0; i < length; i++) message += String((char) payload[i]);
  Serial.println("Message from topic " + String(topic) + ":" + message);
  DeserializationError err = deserializeJson(inputDoc, payload);
  if (!err) {
    if (String(topic) == UPDATE_DELTA_TOPIC) {
      if (inputDoc["state"].containsKey("alarm_armed")) {
        intruderAlarm.setActivation(inputDoc["state"]["alarm_armed"].as<int8_t>());
      }
      if (inputDoc["state"].containsKey("buzzer_on")) {
        intruderAlarm.setBuzzerActivation(inputDoc["state"]["buzzer_on"].as<int8_t>());
      }
    }
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(MOVEMENT_DETECTOR_PIN, INPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  networkManager->setupWiFi();
  networkManager->setUpCredentials(AMAZON_ROOT_CA1, CERTIFICATE, PRIVATE_KEY);
  networkManager->setMqttConnection(callback);
}

void loop() {
  if (!networkManager->isConeected()) {
    networkManager->reconnect();
    intruderAlarm.reportActivation();
    intruderAlarm.reportBuzzerActivation();
    intruderAlarm.reportDetection();
  }
  intruderAlarm.readDetection();
  networkManager->listenMessages();
}
