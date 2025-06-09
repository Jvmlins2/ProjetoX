#include <Arduino.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <WiFi.h>
#include <ezTime.h>
#include "internet.h"
#include <LiquidCrystal_I2C.h>

WiFiClient espClient;
PubSubClient client(espClient);

const char *mqtt_server = "broker.hivemq.com";
const int mqtt_port = 1883;
const char *mqtt_id = "esp32-senai134-magicduelcontrole1";
const char *mqtt_topic_sub = "senai134/magicduel/console";
const char *mqtt_topic_pub = "senai134/magicduel/controles";

void callback(char *, byte *, unsigned int);
void mqttConnect(void);

void setup()
{
  conectaWiFi();

  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
}

void loop()
{
  checkWiFi();

  if (!client.connected())
    mqttConnect();

  client.loop();

  static unsigned long tempoAnterior = 0;
  unsigned long agora = millis();
  static bool fogo = 0;

  JsonDocument doc;
  String mensagem;


  if(agora - tempoAnterior > 3000)
  {
    fogo = !fogo;

    doc["fogo"] = fogo;

    serializeJson(doc, mensagem);
    client.publish(mqtt_topic_pub, mensagem.c_str());

    tempoAnterior = agora;
  }



}

void callback(char *topic, byte *payload, unsigned int length)
{
  Serial.printf("mensagem recebida em %s: ", topic);

  String mensagem = "";
  for (unsigned int i = 0; i < length; i++)
  {
    char c = (char)payload[i];
    mensagem += c;
  }
  Serial.println(mensagem);
}

void mqttConnect()
{
  while (!client.connected())
  {
    Serial.println("Conectando ao MQTT...");

    if (client.connect(mqtt_id))
    {
      Serial.println("Conectado com sucesso");
      client.subscribe(mqtt_topic_sub);
    }

    else
    {
      Serial.print("falha, rc=");
      Serial.println(client.state());
      Serial.println("tentando novamente em 5 segundos");
      delay(5000);
    }
  }
}