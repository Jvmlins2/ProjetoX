#include <Arduino.h>
#include <WiFi.h>
#include <LiquidCrystal_I2C.h>

WiFiClient espClient;
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

//**************************************Variaveis importantes****************************************
int player1hp = 10;
int player1def = 0;
int player2hp = 10;
int player2def = 0;


uint8_t vidaLcd1 = 5;//Variavel inteira sem sinal, representando vida, com o valor de 5 
uint8_t vidaLcd2 = 5;
//********************************************Variaveis importantes***********************************

void removeVida2();
void removeVida1();

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

int danoFogo = 4;
int danoRaio = 2;



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

  cenariolcd();
}

void loop()
{
//*******************Condição(Eventos) para executar cada função****************************************
  // Para teste: Simula um evento a cada 2 segundos para cada barra
  static uint32_t tempoAnterior = millis();
  static bool turn = false;

  if (millis() - tempoAnterior >= 1000/*Tempo apenas*/)//Se (condição, no caso intervalo de tempo)
  {                                   /*para teste  */
    tempoAnterior = millis(); //Atualisa tempo anterior


//******************************Trecho principal*********************************************************
    /*if (turn) //Se vez da barra 1
    {*/
      removeVida1(); //Chama função que diminui barra
    /*}
    else  //Se não é vez da barra 1
    {*/
      removeVida2(); //Diminui barra 2
    /*}
//******************************************************************************************************
    turn = !turn; //Inverte vez*/
//*****************************************************************************************************
  }
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

//!                           Falta implementar Condicional pra cada função removeVida.

//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
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
    }
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
    lcd.print("f");
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
    lcd.print("I");
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

  gameOver();
}

void playerTwoIceAnimation()
{
  for (int i = 16; i > 2; i--)
  {
    lcd.setCursor(i, 3);
    lcd.print("I");
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

void tratamentoVidaDefesaJogador1()
{
  if (fogoTwo)
  {
    player1hp = player1hp - (danoFogo - player1def);
  }

  else if (geloTwo)
  {
    player2def = player2def + 1;
    if (player2def > 2)
      player2def = 2;

    player1def = player1def - 1;
    if (player1def < 0)
      player1def = 0;
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
    player2hp = player2hp - (danoFogo - player2def);
  }

  else if (geloOne)
  {
    player1def = player1def + 1;
    if (player1def > 2)
      player1def = 2;

    player2def = player2def - 1;
    if (player2def < 0)
      player2def = 0;
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

//***************************************Principais Funções************************************
// ————— Remove um bloco da Barra 1 —————
void removeVida1() {
  if (vidaLcd1 == 0) //Se vida é zero, não faz nada 
  {
    return;
  }
  lcd.setCursor(vidaLcd1 - 1, 0); //Posiciona cursor de acordo com valor da vida 1
  lcd.print(' '); //Apaga ponto de vida printando espaço vazio
  vidaLcd1--; //Valor de vida reduz 1       
}

// ————— Remove um bloco da Barra 2 —————
void removeVida2() {
  if (vidaLcd2 == 0)
  {
    return;
  }
  lcd.setCursor(21 - (vidaLcd2 + 1), 0);
  lcd.print(' ');
  vidaLcd2--;
}              
//**************************************Principais Funções************************************

//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

//!     Preciso chamar função toda vez que `variavel player1hp ou player2hp` reduzir dois pontos

//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!