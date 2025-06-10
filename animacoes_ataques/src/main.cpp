#include <Arduino.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <WiFi.h>
#include <ezTime.h>
#include "internet.h"
#include <LiquidCrystal_I2C.h>

WiFiClient espClient;
PubSubClient client(espClient);
LiquidCrystal_I2C lcd(0x27, 20, 4);

const char *mqtt_server = "broker.hivemq.com";
const int mqtt_port = 1883;
const char *mqtt_id = "esp32-senai134-magicduelconsole";
const char *mqtt_topic_sub = "senai134/magicduel/controles";
const char *mqtt_topic_pub = "";

// funções

void callback(char *, byte *, unsigned int);
void mqttConnect(void);
void tratamentoMsg(String);
void recebimentoMSG(bool fogoOne, bool geloOne, bool fogoTwo, bool geloTwo, bool raioOne, bool raioTwo);

void playerOneFireAnimation(void);
void playerOneIceAnimation(void);
void playerTwoFireAnimation(void);
void playerTwoIceAnimation(void);
void playerOneThunderAnimation(void);
void playerTwoThunderAnimation(void);

void cenariolcd(void);

// CRIAÇÃO DOS CARACTERES ESPECIAIS DO LCD

byte leftMageTop[] = {B00000, B00000, B01000, B01100, B00100, B01110, B11111, B01110};
byte leftMageDown[] = {B01110, B00100, B01110, B01111, B01110, B01110, B11111, B11111};
byte leftCloud[] = {B00000, B00000, B01101, B10010, B10000, B10010, B01101, B00000};
byte rightCloud[] = {B00000, B00000, B10110, B01001, B00001, B01001, B10110, B00000};
byte rightMageTop[] = {B00000, B00000, B00010, B00110, B00100, B01110, B11111, B01110};
byte rightMageDown[] = {B01110, B00100, B01110, B11110, B01110, B01110, B11111, B11111};
byte raio[] = {B00000, B00010, B10000, B00111, B11100, B00001, B01000, B00000};

// fim criação caracteres

int player1hp = 10;
int player1def = 2;
int player2hp = 10;
int player2def = 2;

bool fogoOne = 0;
bool geloOne = 0;
bool fogoTwo = 0;
bool geloTwo = 0;
bool raioOne = 0;
bool raioTwo = 0;

void setup()
{
  Serial.begin(9600);
  lcd.init();
  lcd.backlight();

  // Definindo os caracteres especiais
  lcd.createChar(0, leftMageTop);
  lcd.createChar(1, leftMageDown);
  lcd.createChar(2, leftCloud);
  lcd.createChar(3, rightCloud);
  lcd.createChar(4, rightMageTop);
  lcd.createChar(5, rightMageDown);
  lcd.createChar(6, raio);

  conectaWiFi();
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);

  cenariolcd();
}

void loop()
{
  checkWiFi();
  if (!client.connected())
    mqttConnect();

  client.loop();
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

  tratamentoMsg(mensagem);
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

void tratamentoMsg(String msg)
{
  String mensagem = msg;
  JsonDocument doc;
  DeserializationError erro = deserializeJson(doc, mensagem);

  if (erro)
  {
    Serial.println("Mensagem Recebida não esta no formato Json");
    return;
  }

  else
  {

    if (!doc["fogoOne"].isNull())
      fogoOne = doc["fogoOne"];

    if (!doc["geloOne"].isNull())
      geloOne = doc["geloOne"];

    if (!doc["fogoTwo"].isNull())
      fogoTwo = doc["fogoTwo"];

    if (!doc["geloTwo"].isNull())
      geloTwo = doc["geloTwo"];

    if (!doc["raioOne"].isNull())
      raioOne = doc["raioOne"];

    if (!doc["raioTwo"].isNull())
      raioTwo = doc["raioTwo"];

    recebimentoMSG(fogoOne, geloOne, fogoTwo, geloTwo, raioOne, raioTwo);
  }
}

void recebimentoMSG(bool fogoOne, bool geloOne, bool fogoTwo, bool geloTwo, bool raioOne, bool raioTwo)
{
  if (fogoOne == 1 and fogoTwo == 1)
  {
    Serial.println("fogo x fogo");
    playerOneFireAnimation();
    playerTwoFireAnimation();
  }

  else if (geloOne == 1 and geloTwo == 1)
  {
    Serial.println("gelo x gelo");
    playerOneIceAnimation();
    playerTwoIceAnimation();
  }

  else if (raioOne == 1 and raioTwo == 1)
  {
    Serial.println("raio x raio");
    playerOneThunderAnimation();
    playerTwoThunderAnimation();
  }

  else if (fogoOne == 1 and geloTwo == 1)
  {
    Serial.println("fogo x gelo");
    playerOneFireAnimation();
    playerTwoIceAnimation();
  }

  else if (geloOne == 1 and fogoTwo == 1)
  {
    Serial.println("gelo x fogo");
    playerOneIceAnimation();
    playerTwoFireAnimation();
  }

  else if (fogoOne == 1 and raioTwo == 1)
  {
    Serial.println("fogo x raio");
    playerOneFireAnimation();
    playerTwoThunderAnimation();
  }

  else if (geloOne == 1 and raioTwo == 1)
  {
    Serial.println("gelo x raio");
    playerOneIceAnimation();
    playerTwoThunderAnimation();
  }

  else if (raioOne == 1 and fogoTwo == 1)
  {
    Serial.println("raio x fogo");
    playerOneThunderAnimation();
    playerTwoFireAnimation();
  }

  else if (raioOne == 1 and geloTwo == 1)
  {
    Serial.println("raio x gelo");
    playerOneThunderAnimation();
    playerTwoIceAnimation();
  }
}

void playerOneFireAnimation()
{
  for (int i = 3; i < 17; i++)
  {
    lcd.setCursor(i, 3);
    lcd.print("f");
    lcd.setCursor(i - 1, 3);
    lcd.print(" ");
    delay(150);
  }

  lcd.setCursor(16, 3);
  lcd.print(" ");
  lcd.noBacklight();
  delay(200);
  lcd.backlight();
  delay(200);
  lcd.noBacklight();
  delay(200);
  lcd.backlight();

  fogoOne = 0;
    
}

void playerTwoFireAnimation()
{
  for (int i = 16; i > 2; i--)
  {
    lcd.setCursor(i, 3);
    lcd.print("f");
    lcd.setCursor(i + 1, 3);
    lcd.print(" ");
    delay(150);
  }

  lcd.setCursor(3, 3);
  lcd.print(" ");
  lcd.noBacklight();
  delay(200);
  lcd.backlight();
  delay(200);
  lcd.noBacklight();
  delay(200);
  lcd.backlight();

  fogoTwo = 0;
}

void playerOneIceAnimation()
{
  for (int i = 3; i < 17; i++)
  {
    lcd.setCursor(i, 3);
    lcd.print("I");
    lcd.setCursor(i - 1, 3);
    lcd.print(" ");
    delay(150);
  }

  lcd.setCursor(16, 3);
  lcd.print(" ");
  lcd.noBacklight();
  delay(200);
  lcd.backlight();
  delay(200);
  lcd.noBacklight();
  delay(200);
  lcd.backlight();

  geloOne = 0;
}

void playerTwoIceAnimation()
{
  for (int i = 16; i > 2; i--)
  {
    lcd.setCursor(i, 3);
    lcd.print("I");
    lcd.setCursor(i + 1, 3);
    lcd.print(" ");
    delay(150);
  }

  lcd.setCursor(3, 3);
  lcd.print(" ");
  lcd.noBacklight();
  delay(200);
  lcd.backlight();
  delay(200);
  lcd.noBacklight();
  delay(200);
  lcd.backlight();

  geloTwo = 0;
}

void playerOneThunderAnimation()
{
  for (int i = 3; i < 17; i++)
  {
    lcd.setCursor(i, 3);
    lcd.write(6);
    lcd.setCursor(i - 1, 3);
    lcd.print(" ");
    delay(130);
  }

  lcd.setCursor(16, 3);
  lcd.print(" ");
  lcd.noBacklight();
  delay(200);
  lcd.backlight();
  delay(200);
  lcd.noBacklight();
  delay(200);
  lcd.backlight();

    raioOne = 0;
}

void playerTwoThunderAnimation()
{
  for (int i = 16; i > 2; i--)
  {
    lcd.setCursor(i, 3);
    lcd.write(6);
    lcd.setCursor(i + 1, 3);
    lcd.print(" ");
    delay(130);
  }

  lcd.setCursor(3, 3);
  lcd.print(" ");
  lcd.noBacklight();
  delay(200);
  lcd.backlight();
  delay(200);
  lcd.noBacklight();
  delay(200);
  lcd.backlight();

    raioTwo = 0;
}

void cenariolcd()
{
  // barras de hp esquerda
  lcd.setCursor(0, 0);
  lcd.write(255);
  lcd.write(255);
  lcd.write(255);
  lcd.write(255);
  lcd.write(255);
  lcd.setCursor(5, 0);
  lcd.print(player1hp);

  // barra de hp direita
  lcd.setCursor(19, 0);
  lcd.write(255);
  lcd.setCursor(18, 0);
  lcd.write(255);
  lcd.setCursor(17, 0);
  lcd.write(255);
  lcd.setCursor(16, 0);
  lcd.write(255);
  lcd.setCursor(15, 0);
  lcd.write(255);
  lcd.setCursor(13, 0);
  lcd.print(player2hp);

  // mago esquerda
  lcd.setCursor(1, 2);
  lcd.write(0);
  lcd.setCursor(1, 3);
  lcd.write(1);

  // mago direita
  lcd.setCursor(18, 2);
  lcd.write(4);
  lcd.setCursor(18, 3);
  lcd.write(5);

  // nuvens
  lcd.setCursor(2, 1);
  lcd.write(2);
  lcd.setCursor(3, 1);
  lcd.write(3);

  lcd.setCursor(7, 1);
  lcd.write(2);
  lcd.setCursor(8, 1);
  lcd.write(3);

  lcd.setCursor(16, 1);
  lcd.write(2);
  lcd.setCursor(17, 1);
  lcd.write(3);

  lcd.setCursor(11, 1);
  lcd.write(2);
  lcd.setCursor(12, 1);
  lcd.write(3);
}
