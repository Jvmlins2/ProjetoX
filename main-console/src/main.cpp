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
const char *mqtt_topic_pub = "senai134/magicduel/console";

// funções

void callback(char *, byte *, unsigned int);
void mqttConnect(void);
void tratamentoMsg(String);
void tratamentoAtaques(void);
void envioMensagem(void);

void playerOneFireAnimation(void);
void playerOneIceAnimation(void);
void playerTwoFireAnimation(void);
void playerTwoIceAnimation(void);
void playerOneThunderAnimation(void);
void playerTwoThunderAnimation(void);
void tratamentoVidaDefesaJogador1(void);
void tratamentoVidaDefesaJogador2(void);
void hpCounterJogador1(void);
void hpCounterJogador2(void);
void gameOver(void);
void resetTelaControle(void);

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
int player1def = 0;
int player2hp = 10;
int player2def = 0;

bool fogoOne = 0;
bool geloOne = 0;
bool fogoTwo = 0;
bool geloTwo = 0;
bool raioOne = 0;
bool raioTwo = 0;

bool sensorPronto1 = 0;
bool sensorPronto2 = 0;

int numRandom = 0;

//! VARIAVEIS DE TESTE DE DANO

int danoFogo = 3;
int danoGelo = 1;
int danoRaio = 2;

int estadoFogoA = 0;
int estadoGeloA = 0;
int estadoFogoB = 0;
int estadoGeloB = 0;

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

    if (!doc["sensorPronto1"].isNull())
      sensorPronto1 = doc["sensorPronto1"];

    if (!doc["sensorPronto2"].isNull())
      sensorPronto2 = doc["sensorPronto2"];

    if (!doc["forcaAtaqueFogo"].isNull())
      estadoFogoB = doc["forcaAtaqueFogo"];

    if (!doc["forcaAtaqueGelo"].isNull())
      estadoGeloB = doc["forcaAtaqueGelo"];

    if (!doc["forcaAtaqueFogo"].isNull())
      estadoFogoA = doc["forcaAtaqueFogoA"];

    if (!doc["forcaAtaqueGelo"].isNull())
      estadoGeloA = doc["forcaAtaqueGeloA"];

    tratamentoAtaques();
  }
}

void tratamentoAtaques()
{
  if (fogoOne and fogoTwo)
  {
    envioMensagem();

    if (sensorPronto1 and sensorPronto2)
    {
      numRandom = rand() % 100;
      Serial.println("fogo x fogo");

      if (numRandom % 2 == 0)
      {
        tratamentoVidaDefesaJogador2();
        playerOneFireAnimation();

        tratamentoVidaDefesaJogador1();
        playerTwoFireAnimation();
      }

      else
      {
        tratamentoVidaDefesaJogador1();
        playerTwoFireAnimation();

        tratamentoVidaDefesaJogador2();
        playerOneFireAnimation();
      }

      sensorPronto1 = 0;
      sensorPronto2 = 0;
      resetTelaControle();
    }
  }

  else if (geloOne and geloTwo)
  {
    envioMensagem();

    if (sensorPronto1 and sensorPronto2)
    {
      numRandom = rand() % 100;
      Serial.println("gelo x gelo");

      if (numRandom % 2 == 0)
      {
        tratamentoVidaDefesaJogador1();
        playerOneIceAnimation();

        tratamentoVidaDefesaJogador2();
        playerTwoIceAnimation();
      }

      else
      {
        tratamentoVidaDefesaJogador2();
        playerTwoIceAnimation();

        tratamentoVidaDefesaJogador1();
        playerOneIceAnimation();
      }

      sensorPronto1 = 0;
      sensorPronto2 = 0;
      resetTelaControle();
    }
  }

  else if (raioOne and raioTwo)
  {
    envioMensagem();

    if (sensorPronto1 and sensorPronto2)
    {
      numRandom = rand() % 100;
      Serial.println("raio x raio");

      if (numRandom % 2 == 0)
      {
        tratamentoVidaDefesaJogador2();
        playerOneThunderAnimation();

        tratamentoVidaDefesaJogador1();
        playerTwoThunderAnimation();
      }

      else
      {
        tratamentoVidaDefesaJogador1();
        playerTwoThunderAnimation();

        tratamentoVidaDefesaJogador2();
        playerOneThunderAnimation();
      }

      sensorPronto1 = 0;
      sensorPronto2 = 0;
      resetTelaControle();
    }
  }

  else if (fogoOne and geloTwo)
  {
    envioMensagem();

    if (sensorPronto1 and sensorPronto2)
    {
      numRandom = rand() % 100;
      Serial.println("fogo x gelo");

      if (numRandom % 2 == 0)
      {
        tratamentoVidaDefesaJogador2();
        playerOneFireAnimation();

        tratamentoVidaDefesaJogador1();
        playerTwoIceAnimation();
      }

      else
      {
        tratamentoVidaDefesaJogador1();
        playerTwoIceAnimation();

        tratamentoVidaDefesaJogador2();
        playerOneFireAnimation();
      }

      sensorPronto1 = 0;
      sensorPronto2 = 0;
      resetTelaControle();
    }
  }

  else if (geloOne and fogoTwo)
  {
    envioMensagem();

    if (sensorPronto1 and sensorPronto2)
    {
      numRandom = rand() % 100;
      Serial.println("gelo x fogo");

      if (numRandom % 2 == 0)
      {
        tratamentoVidaDefesaJogador2();
        playerOneIceAnimation();

        tratamentoVidaDefesaJogador1();
        playerTwoFireAnimation();
      }

      else
      {
        tratamentoVidaDefesaJogador1();
        playerTwoFireAnimation();

        tratamentoVidaDefesaJogador2();
        playerOneIceAnimation();
      }

      sensorPronto1 = 0;
      sensorPronto2 = 0;
      resetTelaControle();
    }
  }

  else if (fogoOne and raioTwo)
  {
    envioMensagem();

    if (sensorPronto1 and sensorPronto2)
    {
      Serial.println("fogo x raio");

      tratamentoVidaDefesaJogador1();
      playerTwoThunderAnimation();

      tratamentoVidaDefesaJogador2();
      playerOneFireAnimation();

      sensorPronto1 = 0;
      sensorPronto2 = 0;
      resetTelaControle();
    }
  }

  else if (geloOne and raioTwo)
  {
    envioMensagem();

    if (sensorPronto1 and sensorPronto2)
    {
      Serial.println("gelo x raio");

      tratamentoVidaDefesaJogador1();
      playerTwoThunderAnimation();

      tratamentoVidaDefesaJogador2();
      playerOneIceAnimation();

      sensorPronto1 = 0;
      sensorPronto2 = 0;
      resetTelaControle();
    }
  }

  else if (raioOne and fogoTwo)
  {
    envioMensagem();

    if (sensorPronto1 and sensorPronto2)
    {
      Serial.println("raio x fogo");

      tratamentoVidaDefesaJogador2();
      playerOneThunderAnimation();

      tratamentoVidaDefesaJogador1();
      playerTwoFireAnimation();

      sensorPronto1 = 0;
      sensorPronto2 = 0;
      resetTelaControle();
    }
  }

  else if (raioOne and geloTwo)
  {
    envioMensagem();

    if (sensorPronto1 and sensorPronto2)
    {
      Serial.println("raio x gelo");

      tratamentoVidaDefesaJogador2();
      playerOneThunderAnimation();

      tratamentoVidaDefesaJogador1();
      playerTwoIceAnimation();

      sensorPronto1 = 0;
      sensorPronto2 = 0;
      resetTelaControle();
    }
  }
}

void playerOneFireAnimation()
{
  for (int i = 3; i < 17; i++)
  {
    lcd.setCursor(i, 3);
    lcd.print("0");
    lcd.setCursor(i - 1, 3);
    lcd.print(" ");
    delay(250);
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

  hpCounterJogador2();

  fogoOne = 0;

  gameOver();
}

void playerTwoFireAnimation()
{
  for (int i = 16; i > 2; i--)
  {
    lcd.setCursor(i, 3);
    lcd.print("0");
    lcd.setCursor(i + 1, 3);
    lcd.print(" ");
    delay(250);
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

  hpCounterJogador1();

  fogoTwo = 0;

  gameOver();
}

void playerOneIceAnimation()
{
  for (int i = 3; i < 17; i++)
  {
    lcd.setCursor(i, 3);
    lcd.print(">");
    lcd.setCursor(i - 1, 3);
    lcd.print(" ");
    delay(250);
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
  hpCounterJogador2();

  gameOver();
}

void playerTwoIceAnimation()
{
  for (int i = 16; i > 2; i--)
  {
    lcd.setCursor(i, 3);
    lcd.print("<");
    lcd.setCursor(i + 1, 3);
    lcd.print(" ");
    delay(250);
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
  hpCounterJogador1();

  gameOver();
}

void playerOneThunderAnimation()
{
  for (int i = 3; i < 17; i++)
  {
    lcd.setCursor(i, 3);
    lcd.write(6);
    lcd.setCursor(i - 1, 3);
    lcd.print(" ");
    delay(250);
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

  hpCounterJogador2();

  gameOver();
}

void playerTwoThunderAnimation()
{
  for (int i = 16; i > 2; i--)
  {
    lcd.setCursor(i, 3);
    lcd.write(6);
    lcd.setCursor(i + 1, 3);
    lcd.print(" ");
    delay(250);
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

  hpCounterJogador1();

  gameOver();
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

void envioMensagem()
{
  bool interacaoSensores = 1;

  JsonDocument doc;
  String mensagem;

  doc["interacao"] = interacaoSensores;

  serializeJson(doc, mensagem);
  client.publish(mqtt_topic_pub, mensagem.c_str());
}

void tratamentoVidaDefesaJogador1()
{
  if (fogoTwo)
  {
    if (estadoFogoB == 1)
    {
      player1hp = player1hp - (danoFogo - player1def);
    }

    else if (estadoFogoB == 2)
    {
      player1hp = player1hp - (danoFogo + 1 - player1def);
    }
  }

  else if (geloTwo)
  {
    if (estadoGeloB == 1)
    {
      player2def = player2def + 1;
      if (player2def > 2)
        player2def = 2;

      player1def = player1def - 1;
      if (player1def < 0)
        player1def = 0;
    }

    else if (estadoGeloB == 2)
    {
      player2def = player2def + 1;
      if (player2def > 2)
        player2def = 2;

      player1def = player1def - 1;
      if (player1def < 0)
        player1def = 0;

      player1hp = player1hp - 1;
    }
  }
  else if (raioTwo)
  {
    player1hp = player1hp - (danoRaio - player1def);
  }
}

void tratamentoVidaDefesaJogador2()
{
  if (fogoOne)
  {
    if (estadoFogoA)
    {
      player2hp = player2hp - (danoFogo - player2def);
    }

    else if (estadoFogoB)
    {
      player2hp = player2hp - (danoFogo + 1 - player2def);
    }
  }

  else if (geloOne)
  {
    if (estadoGeloA == 1)
    {
      player1def = player1def + 1;
      if (player1def > 2)
        player1def = 2;

      player2def = player2def - 1;
      if (player2def < 0)
        player2def = 0;
    }

    else if(estadoGeloA == 2)
    {
      player1def = player1def + 1;
      if (player1def > 2)
        player1def = 2;

      player2def = player2def - 1;
      if (player2def < 0)
        player2def = 0;

      player2hp = player1hp - 1;
    }
  }

  else if (raioOne)
  {
    player2hp = player2hp - (danoRaio - player2def);
  }
}

void hpCounterJogador1()
{
  if (player1hp != 10)
  {
    lcd.setCursor(5, 0);
    lcd.print(" ");
    lcd.setCursor(6, 0);

    if (player1hp >= 0)
      lcd.print(player1hp);

    else
      lcd.print("0");
  }
}

void hpCounterJogador2()

{
  if (player2hp != 10)
  {
    lcd.setCursor(14, 0);
    lcd.print(" ");
    lcd.setCursor(13, 0);

    if (player2hp >= 0)
      lcd.print(player2hp);

    else
      lcd.print("0");
  }
}

void gameOver()
{
  if (player1hp <= 0)
  {
    delay(1500);
    lcd.clear();
    lcd.setCursor(0, 1);
    lcd.print("Jogador 2 venceu");
    while (1)
      ;
  }

  else if (player2hp <= 0)
  {
    delay(1500);
    lcd.clear();
    lcd.setCursor(0, 1);
    lcd.print("Jogador 1 venceu");
    while (1)
      ;
  }
}

void resetTelaControle()
{
  JsonDocument doc;
  String mensagem;

  bool resetTela = 1;

  doc["voltaTela"] = resetTela;

  serializeJson(doc, mensagem);
  client.publish(mqtt_topic_pub, mensagem.c_str());
}