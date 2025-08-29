#include <Arduino.h>
#include <LiquidCrystal_I2C.h>
#include <WiFi.h>
#include "internet.h"
#include <ArduinoJson.h>
#include <PubSubClient.h>
#include <Bounce2.h>
#include <DHT.h>

WiFiClient espClient;
PubSubClient client(espClient);
Bounce botaoA = Bounce();
Bounce botaoB = Bounce();
Bounce botaoC = Bounce();
Bounce botaoD = Bounce();

const char *mqtt_server = "broker.hivemq.com";
const int mqtt_port = 1883;
const char *mqtt_id = "esp32-senai134-magicduelcontrole12";
const char *mqtt_topic_sub = "senai134/magicduel/console";
const char *mqtt_topic_pub = "senai134/magicduel/controles";

#define pinled 27

void callback(char *, byte *, unsigned int);
void mqttConnect(void);

void telaSelecao(void);
void botoesCimaBaixo(void);
void botaoConfirma(void);
void telaEspera(void);
void enviaAtaque(void);
void telaInteracao(void);
void tratamentoMsg(String);
void interacaoSensor(void);
void ataqueGelo(void);
void ataqueFogo(void);
void enviaForcaAtaque(void);
void setupDHT(void);
void resetTela(void);

LiquidCrystal_I2C lcd(0x27, 20, 4);

#define DHTPIN 5
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);

// variaveis

int telas = 0;
int posicaoSeletor = 0;

bool fogo = 0;
bool gelo = 0;
bool raio = 0;

bool interacao = 0;
bool voltaTela = 0;

float temperatura = 0;
float umidade = 0;
float setupUmidade = 35.0;
float setupTemperatura = 26.0;

int forcaAtaqueFogo = 0;
int forcaAtaqueGelo = 0;

void setup()
{
  Serial.begin(9600);
  dht.begin();
  lcd.init();
  pinMode(pinled, OUTPUT);

  botaoA.attach(2, INPUT_PULLUP);
  botaoB.attach(4, INPUT_PULLUP);
  botaoC.attach(23, INPUT_PULLUP);
  botaoD.attach(18, INPUT_PULLUP);

  telaSelecao();

  conectaWiFi();

  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);

  setupUmidade = dht.readHumidity();
  setupTemperatura = dht.readTemperature();

  Serial.println(setupTemperatura);
  Serial.println(setupUmidade);

  // setupDHT();
}

void loop()
{
  checkWiFi();

  if (!client.connected())
    mqttConnect();

  client.loop();

  botaoA.update();
  botaoB.update();
  botaoC.update();
  botaoD.update();

  botoesCimaBaixo();
  botaoConfirma();
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
    if (!doc["interacao"].isNull())
      interacao = doc["interacao"];

    if (!doc["voltaTela"].isNull())
      voltaTela = doc["voltaTela"];
  }

  telaInteracao();
  resetTela();
}

void telaSelecao() // tela 0
{
  digitalWrite(pinled, 0);
  lcd.clear();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print(">");

  lcd.setCursor(1, 0);
  lcd.print("Fogo");
  lcd.setCursor(1, 1);
  lcd.print("Gelo");
  lcd.setCursor(1, 2);
  lcd.print("Raio");
}

void botoesCimaBaixo()
{
  if (telas == 0)
  {
    if (posicaoSeletor == 0)
    {
      if (botaoA.fell())
      {
        lcd.setCursor(0, 0);
        lcd.print(" ");

        lcd.setCursor(0, 2);
        lcd.print(">");

        posicaoSeletor = 2;
      }

      if (botaoC.fell())
      {
        lcd.setCursor(0, 0);
        lcd.print(" ");

        lcd.setCursor(0, 1);
        lcd.print(">");

        posicaoSeletor = 1;
      }
    }

    else if (posicaoSeletor == 1)
    {
      if (botaoA.fell())
      {
        lcd.setCursor(0, 1);
        lcd.print(" ");

        lcd.setCursor(0, 0);
        lcd.print(">");

        posicaoSeletor = 0;
      }

      if (botaoC.fell())
      {
        lcd.setCursor(0, 1);
        lcd.print(" ");

        lcd.setCursor(0, 2);
        lcd.print(">");

        posicaoSeletor = 2;
      }
    }

    else if (posicaoSeletor == 2)
    {
      if (botaoA.fell())
      {
        lcd.setCursor(0, 2);
        lcd.print(" ");

        lcd.setCursor(0, 1);
        lcd.print(">");

        posicaoSeletor = 1;
      }

      if (botaoC.fell())
      {
        lcd.setCursor(0, 2);
        lcd.print(" ");

        lcd.setCursor(0, 0);
        lcd.print(">");

        posicaoSeletor = 0;
      }
    }
  }
}

void botaoConfirma()
{
  if (telas == 0)
  {
    if (posicaoSeletor == 0)
    {
      if (botaoD.fell())
      {
        telaEspera();
        enviaAtaque();
      }
    }

    else if (posicaoSeletor == 1)
    {
      if (botaoD.fell())
      {
        telaEspera();
        enviaAtaque();
      }
    }

    else if (posicaoSeletor == 2)
    {
      if (botaoD.fell())
      {
        telaEspera();
        enviaAtaque();
      }
    }
  }
}

void telaEspera() // tela 1
{
  telas = 1;

  lcd.clear();
  lcd.setCursor(1, 1);
  lcd.print("Aguarde o outro");
  lcd.setCursor(1, 2);
  lcd.print("jogador");
}

void enviaAtaque()
{
  JsonDocument doc;
  String mensagem;

  if (posicaoSeletor == 0)
  {
    fogo = 1;

    doc["fogoOne"] = fogo;

    fogo = 0;
  }

  else if (posicaoSeletor == 1)
  {
    gelo = 1;

    doc["geloOne"] = gelo;

    gelo = 0;
  }

  else if (posicaoSeletor == 2)
  {
    raio = 1;

    doc["raioOne"] = raio;

    raio = 1;
  }

  serializeJson(doc, mensagem);
  client.publish(mqtt_topic_pub, mensagem.c_str());
}

void telaInteracao() // tela 2
{
  if (telas == 1)
  {
    if (interacao == 1)
    {
      digitalWrite(pinled, 1);
      telas = 2;

      lcd.clear();
      lcd.setCursor(1, 1);
      lcd.print("INTERAJA COM");
      lcd.setCursor(1, 2);
      lcd.print("O SENSOR");

      interacao = 0;

      interacaoSensor();
    }
  }
}

void interacaoSensor()
{
  delay(6000);
  temperatura = dht.readTemperature();
  umidade = dht.readHumidity();

  ataqueFogo();
  ataqueGelo();

  enviaForcaAtaque();
}

void setupDHT()
{
  delay(2000);
  setupTemperatura = dht.readTemperature();
  setupUmidade = dht.readHumidity();
}

void ataqueFogo()
{
  if (temperatura > (setupTemperatura * 1.08))
  {
    forcaAtaqueFogo = 2;
  }
  else if (temperatura >= (setupTemperatura * 1.02) && temperatura <= (setupTemperatura * 1.08))
  {
    forcaAtaqueFogo = 1;
  }
  else
  {
    forcaAtaqueFogo = 1;
  }
  Serial.println(temperatura);
}

void ataqueGelo()
{
  if (umidade >= (setupUmidade * 1.95))
  {
    forcaAtaqueGelo = 2;
  }
  else if (umidade < (setupUmidade * 1.89) && umidade >= (setupUmidade * 1.35))
  {
    forcaAtaqueGelo = 1;
  }
  else
  {
    forcaAtaqueGelo = 1;
  }
  Serial.println(umidade);
}

void enviaForcaAtaque()
{
  JsonDocument doc;
  String mensagem;

  bool sensorPronto = 0;

  sensorPronto = 1;

  doc["forcaAtaqueFogoA"] = forcaAtaqueFogo;
  doc["forcaAtaqueGeloA"] = forcaAtaqueGelo;
  doc["sensorPronto1"] = sensorPronto;

  sensorPronto = 0;

  serializeJson(doc, mensagem);
  client.publish(mqtt_topic_pub, mensagem.c_str());
}

void resetTela()
{
  if (telas == 2)
  {
    if (voltaTela == 1)
    {
      telaSelecao();
      telas = 0;
      voltaTela = 0;
    }
  }
}