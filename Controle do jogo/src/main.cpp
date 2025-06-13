#include <Arduino.h>
#include <LiquidCrystal_I2C.h>
#include <WiFi.h>
#include "internet.h"
#include <ArduinoJson.h>
#include <PubSubClient.h>


WiFiClient espClient;
PubSubClient client(espClient);

const char *mqtt_server = "broker.hivemq.com";
const int mqtt_port = 1883;
const char *mqtt_id = "esp32-senai134-magicduelcontrole1";
const char *mqtt_topic_sub = "senai134/magicduel/console";
const char *mqtt_topic_pub = "senai134/magicduel/controles";

void callback(char *, byte *, unsigned int);
void mqttConnect(void);


LiquidCrystal_I2C lcd(0x27, 20, 4);

#define PIN_A 2
#define PIN_B 4
#define PIN_C 18
#define PIN_D 19
#define PIN_E 25

void leituraBotoes(bool leitura[5]);
int selecaoSerial(bool botaoA, bool botaoB, bool botaoC, bool botaoD);
void ataques(int magia, bool botaoB, bool D);

void setup()
{
  pinMode(PIN_A, INPUT_PULLUP);
  pinMode(PIN_B, INPUT_PULLUP);
  pinMode(PIN_C, INPUT_PULLUP);
  pinMode(PIN_D, INPUT_PULLUP);
  pinMode(PIN_E, INPUT_PULLUP);

  Serial.begin(9600);

  lcd.init();
  lcd.backlight();
  lcd.home();
  lcd.print(">");

  conectaWiFi();

  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
}

void loop()
{
  heckWiFi();

  if (!client.connected())
    mqttConnect();

  client.loop();


  bool botao[5];
  static int golpes;
  leituraBotoes(botao);

  bool A = botao[0];  if(A) Serial.println("A");//! sobe
  bool B = botao[1];  if(B) Serial.println("B");//! voltar
  bool C = botao[2];  if(C) Serial.println("C");//! descer
  bool D = botao[3];  if(D) Serial.println("D");//! avancar
  bool E = botao[4];  if(E) Serial.println("E");//* inderterminado

  //* As magias e a selecao sao dividas de 0 a 3
  golpes = selecaoSerial(A, B, C, D);
  ataques(golpes, B, D);
}



void leituraBotoes(bool leitura[5])
{
  unsigned int tempo = millis();
  static bool verdadeiro = 1;

  if (tempo > 100)
  {
    //? Botão A
    bool estadoA = digitalRead(PIN_A);
    static bool estadoAnteriorA = 1;
    static unsigned int tempoAnteriorA;
    static bool acaoA = 1;

    if (estadoA != estadoAnteriorA && estadoA == 0)
      tempoAnteriorA = tempo;
    if (tempo - tempoAnteriorA > 100)
    {
      if (acaoA != estadoA)
      {
        if (estadoA == 0)
          leitura[0] = 1;
      }
      else
      {
        leitura[0] = 0;
      }
      acaoA = estadoA;
    }
    estadoAnteriorA = estadoA;

    //? Botão B
    bool estadoB = digitalRead(PIN_B);
    static bool estadoAnteriorB = 1;
    static unsigned int tempoAnteriorB;
    static bool acaoB = 1;
    if (estadoB != estadoAnteriorB && estadoB == 0)
      tempoAnteriorB = tempo;
    if (tempo - tempoAnteriorB > 100)
    {
      if (acaoB != estadoB)
      {
        if (estadoB == 0)
          leitura[1] = 1;
      }
      else
      {
        leitura[1] = 0;
      }
      acaoB = estadoB;
    }
    estadoAnteriorB = estadoB;

    //? Botão C
    bool estadoC = digitalRead(PIN_C);
    static bool estadoAnteriorC = 1;
    static unsigned int tempoAnteriorC;
    static bool acaoC = 1;
    if (estadoC != estadoAnteriorC && estadoC == 0)
        tempoAnteriorC = tempo;
      if (tempo - tempoAnteriorC > 100)
      {
        if (acaoC != estadoC)
        {
          if (estadoC == 0)
            leitura[2] = 1;
        }
        else
        {
          leitura[2] = 0;
        }
        acaoC = estadoC;
      }
    estadoAnteriorC = estadoC;

    //? Botão D
    bool estadoD = digitalRead(PIN_D);
    static bool estadoAnteriorD = 1;
    static unsigned int tempoAnteriorD;
    static bool acaoD = 1;
    if (estadoD != estadoAnteriorD)
      tempoAnteriorD = tempo;
    if (tempo - tempoAnteriorD > 100)
    {
      if (acaoD != estadoD)
      {
        if (estadoD == 0)
          leitura[3] = 1;
      }
      else
      {
        leitura[3] = 0;
      }
      acaoD = estadoD;
    }
    estadoAnteriorD = estadoD;

    //? Botão E
    bool estadoE = digitalRead(PIN_E);
    static bool estadoAnteriorE = 1;
    static unsigned int tempoAnteriorE;
    static bool acaoE = 1;
    if (estadoE != estadoAnteriorE)
      tempoAnteriorE = tempo;
    if (tempo - tempoAnteriorE > 100)
    {
      if (acaoE != estadoD)
      {
        if (estadoE == 0)
          leitura[4] = 1;
      }
      else
      {
        leitura[4] = 0;
      }
      acaoE = estadoE;
    }
    estadoAnteriorE = estadoE;
  }
}

int selecaoSerial(bool botaoA, bool botaoB, bool botaoC, bool botaoD)
{
  //? Selecao
  static int selecao;
  static int selecaoAnterior;
  static bool agir;

  if(botaoD)
  agir = true;
  if(botaoB)
  agir = false;

  if(!agir)
{
  lcd.clear();
  if (botaoA)
    selecao--;
  if (botaoC)
    selecao++;

  if (selecao > 3)
    selecao = 0;
  if (selecao < 0)
    selecao = 3;

    //* Impedir que pule 3 a 1
    if(selecao == 1 && selecaoAnterior == 3)
    selecao = 2;
    
    selecaoAnterior = selecao;

  if (selecao == 0)
  {
    lcd.home();
    lcd.print(">");
    lcd.setCursor(0, 3);
    lcd.print(" ");
    lcd.setCursor(0, 1);
    lcd.print(" ");
  }
  if (selecao == 1)
  {
    lcd.setCursor(0, 1);
    lcd.print(">");
    lcd.setCursor(0, 0);
    lcd.print(" ");
    lcd.setCursor(0, 2);
    lcd.print(" ");
  }
  if (selecao == 2)
  {
    lcd.setCursor(0, 2);
    lcd.print(">");
    lcd.setCursor(0, 1);
    lcd.print(" ");
    lcd.setCursor(0, 3);
    lcd.print(" ");
  }
  if (selecao == 3)
  {
    lcd.setCursor(0, 3);
    lcd.print(">");
    lcd.setCursor(0, 2);
    lcd.print(" ");
    lcd.setCursor(0, 0);
    lcd.print(" ");
  }
}
  return selecao;
}

void ataques(int magia, bool botaoB, bool botaoD)
{
  if(magia == 0)
  {
    if(botaoD == 1)
    {
    Serial.println("Magia 1");//*golpe
    lcd.clear();
    }
    if(botaoB);
    //volta a selecao
  }
  if(magia == 1)
  {
    if(botaoD)
    {
       Serial.println("Magia 2");//*golpe
       lcd.clear();
    }

    if(botaoB);
    //volta a selecao
  }
  if(magia == 2)
  {
    if(botaoD)
    {
       Serial.println("Magia 3");//*golpe
       lcd.clear();
    }

    if(botaoB);
    //volta a selecao
  }
  if(magia == 3)
  {
    if(botaoD)
    {
       Serial.println("Magia 4");//*golpe
       lcd.clear();
    }

    if(botaoB);
    //volta a selecao
  }
}
//*===========================================================================================================

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