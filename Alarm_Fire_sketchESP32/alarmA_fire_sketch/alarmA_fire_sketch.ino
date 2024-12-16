#include "FireAlarm.h"
#include "ClientManager.h"
#include <ArduinoJson.h>

// Replace with your MQTT broker details
const char* MQTT_BROKER = "a2gyx6o3r72s2g-ats.iot.us-east-1.amazonaws.com";
const int MQTT_PORT = 8883;
const char* CLIENT_ID = "ESP-32-FIRE-000";

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
MIIDWTCCAkGgAwIBAgIUKu5X3B9z7qZV8T4iXdPsmGF8O6swDQYJKoZIhvcNAQEL
BQAwTTFLMEkGA1UECwxCQW1hem9uIFdlYiBTZXJ2aWNlcyBPPUFtYXpvbi5jb20g
SW5jLiBMPVNlYXR0bGUgU1Q9V2FzaGluZ3RvbiBDPVVTMB4XDTI0MTEyOTA0MDkw
OVoXDTQ5MTIzMTIzNTk1OVowHjEcMBoGA1UEAwwTQVdTIElvVCBDZXJ0aWZpY2F0
ZTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBAMTjmRqv/3mhv7MWgpX3
vQSxKebjS+rxztGz9/YAoK23YwlgpgTFwxDEvHwWyj+Xz9Q4r3endAfb4SfqbqZc
jD6te/OogK40IRbTd9v2DPoq7vCGfBALrGp+v+GGOB3qgC5OeSJ8cgqgd/l/ho7/
L8ze58WKOOFFrq2EDdffB5+RZkY2sChF572UtPNUzrTD4NSE1m5ppiyui5gppPRn
yTMLv/DxXxCS0UoaGXCS3uMlhKTn2pVNs/7acEkNSmcs7+d3ON+g0vIEiJpn9bDf
a9/73Rvtvl/suJPslKcLYz1uuFoj+U8PZLIT5RCcGMCYbtfBYu0cAe3r5OtZJzhP
0CECAwEAAaNgMF4wHwYDVR0jBBgwFoAU6a7CaKfzeF/uvvmFOMHEQIXWoc4wHQYD
VR0OBBYEFKYsawXxdqDq4IldDGc1H5JmblT9MAwGA1UdEwEB/wQCMAAwDgYDVR0P
AQH/BAQDAgeAMA0GCSqGSIb3DQEBCwUAA4IBAQAhyeOsDp/81gTuZGfJLc7yznhC
C6DEDY5MsXNoZelaRgVR2KI+wsq5pCsCKktzlTJrEWNEy5gYIoUrzQj6Aysgvq30
G/PxkjLEJtwcEZHLyyrXR2uMhCJhfkuq2T/Bl6M6QCDxnYu/2Md78xI8dgkn2swh
Y2yQyv1LhzYnJUVhuzpjSqWgLgVwiL9w+IACMTWBC2+CvI12Y2+uh3V3rdnIVMXZ
LOx3EElSlsitbpo+DRPn2sOJUt9CA3HS/m2Gfbqlk7ux7iBk5QCojZjw65iYzWqI
8+4LWJGot3NXeZcRFdkVCX8VIGLcW4NnlCqqegwEF/2AXITW4YvnXBSCzS4s
-----END CERTIFICATE-----
)KEY";

const char PRIVATE_KEY[] PROGMEM = R"KEY(
-----BEGIN RSA PRIVATE KEY-----
MIIEpgIBAAKCAQEAxOOZGq//eaG/sxaClfe9BLEp5uNL6vHO0bP39gCgrbdjCWCm
BMXDEMS8fBbKP5fP1Divd6d0B9vhJ+puplyMPq1786iArjQhFtN32/YM+iru8IZ8
EAusan6/4YY4HeqALk55InxyCqB3+X+Gjv8vzN7nxYo44UWurYQN198Hn5FmRjaw
KEXnvZS081TOtMPg1ITWbmmmLK6LmCmk9GfJMwu/8PFfEJLRShoZcJLe4yWEpOfa
lU2z/tpwSQ1KZyzv53c436DS8gSImmf1sN9r3/vdG+2+X+y4k+yUpwtjPW64WiP5
Tw9kshPlEJwYwJhu18Fi7RwB7evk61knOE/QIQIDAQABAoIBAQCFRjL7F6ttYAnt
Zg1bCmlOZcYhXjajgK8o0BPG18HsFTjavitjC6LxFNspfBvHVDIAS6aq7hTixPny
FQR1ZCwiuTgwRR1lzi+fzeFSaYl7Rdibw8zy55P8OnQUatBNn9DIh6IDsA1aGCle
H+6rmR14JmLpGplKE+sb1UmxmG0GmypPyjzLAwyBmV/ku65qAYoIFo9Z4mPMb59a
w546O/WlX/Fwp9Tdme4JIrnCBuDj/baGtN+eq6FEfkjWeXOr/+HaY5qMpkSfVpFg
Qlfn/ZfAs8OdimW1jwPvziaHFZrATmPfC+cMdYXhMSpZpGH2NutP41voIxgrXwXh
ZV8KujABAoGBAPYdAYA9kWHeIC1+E4TW2ds3XJm3wy/ZeO2zBOz1oXKAnGNh13x4
YfwUMxC1I/56adLmcRnDpzvoXBpyihTfBZ6NyHcey6pbbwyf7C+jQoe+dnGwSKex
Y+Lum6lmXFlWUPjvsx7jDv6zgHglIQo1KWmqXaNA2eHGh40oZkPjimZBAoGBAMzM
YIWU3G+YHlFW+QtTZPjrpZpqZcwqT13rOFtCxpH3Sx3opMEO9XRTaNLaPrh1k0X3
yC///kyxAzE8Wi21vqZLJMMo637Q/PXfwVUwFBG3wiv4RuFiQM66TEEaInoQDvw4
HCxgFmfq8BsKWYgDuYXTIcd6UBbEnRuX/vgmubHhAoGBAIXy8X8vgzHl6FN6O/tf
yECUmBSdvGENltCgT3z8jzs3nmZEuThoeMPu5oAgZo+5ZmE/xKzGHP3xEDfgwSBs
5a0O891JaIW2r/EKFU00KSCiW+qyjEigtoGrZhNDv91rqjRimXWlFOMxJSm+44Wf
u4NkEwSoZi2IlPwR0d7/N7NBAoGBAIgVll2tGP0eO6cY/8vC0mmFTDlbUeFki/CA
iKY7rO75P/TTzgZrBjcR5amWU+8GUaJAj8k1dalIRjLFQ7bz+YdOj1MPMczG2hAX
asRMekl7rY7CH3dMbnvv1xO7ZgVt/kcikbX682iB+KI/MlbrArAmJ2ABqo42f58C
VO5X7TSBAoGBAO3gN6GEKD8B6M6gLKKZvXNEDyZV5nM3cPS1br1/K8ewJ+dI8Vv6
8qWs/I12gsjPHlp6NZcVXtz/Qh4LMgRkGbRepZTaRq526V+v+Yr8KNvMmJuYMN7c
GDMOBErKuR6hTJZPRDEZEI7K7nVctVc8BgPxwZQHay64YBKBH54tzDM4
-----END RSA PRIVATE KEY-----
)KEY";

const char* UPDATE_TOPIC = "$aws/things/alarm_fire_000/shadow/update";              // publish
const char* UPDATE_DELTA_TOPIC = "$aws/things/alarm_fire_000/shadow/update/delta";  // subscribe

// Pin definitions
const int VALVE_PIN = 27;
const int MQ2_ppm_PIN = 35; // analogic pin MQ2
const int MQ2_type_PIN = 32; // digital pin MQ2

//objetct instance
ClientManager* networkManager = new ClientManager(MQTT_BROKER, MQTT_PORT, CLIENT_ID, UPDATE_TOPIC, UPDATE_DELTA_TOPIC);
FireAlarm fireAlarm(networkManager, VALVE_PIN, MQ2_type_PIN, MQ2_ppm_PIN);

// Callback function to handle messages received from the subscribed topic
byte payloadValve = 0;

StaticJsonDocument<JSON_OBJECT_SIZE(64)> inputDoc;

void callback(char* topic, byte* payload, unsigned int length) {
  String message;
  for (int i = 0; i < length; i++) message += String((char) payload[i]);
  Serial.println("Message from topic " + String(topic) + ":" + message);
  DeserializationError err = deserializeJson(inputDoc, payload);
  if (!err) {
    if (String(topic) == UPDATE_DELTA_TOPIC) {
      if (inputDoc["state"].containsKey("valve_state")) {
        fireAlarm.setValveActivation(inputDoc["state"]["valve_state"].as<int8_t>());
      }
      if (inputDoc["state"].containsKey("fire_umbral_ppm")) {
        fireAlarm.setFireUmbralPpm(inputDoc["state"]["fire_umbral_ppm"].as<int8_t>());
      }
      if (inputDoc["state"].containsKey("reports_umbral_ppm")) {
        fireAlarm.setReportsUmbralPpm(inputDoc["state"]["reports_umbral_ppm"].as<int8_t>());
      }
    }
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(VALVE_PIN, OUTPUT);
  pinMode(MQ2_ppm_PIN, INPUT);
  pinMode(MQ2_type_PIN, INPUT);
  networkManager->setupWiFi();
  networkManager->setUpCredentials(AMAZON_ROOT_CA1, CERTIFICATE, PRIVATE_KEY);
  networkManager->setMqttConnection(callback);
  fireAlarm.reportValveActivation();
  fireAlarm.reportFireUmbralPpm();
  fireAlarm.reportReportsUmbralPpm();
  fireAlarm.reportFireDetection();
}

void loop() {
  if (!networkManager->isConeected()) {
    networkManager->reconnect();
    fireAlarm.reportValveActivation();
  }

  fireAlarm.readDetection();
  if(fireAlarm.reportValid()) fireAlarm.reportTypeAndPPM();
  networkManager->listenMessages();
  delay(1000);
}
