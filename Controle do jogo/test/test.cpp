#include <Arduino.h>
#include <Bounce2.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 20, 4);

#define PIN_A 5
#define PIN_B 3
#define PIN_C 4
#define PIN_D 9
#define PIN_E 7

void leituraBotoes(bool leitura[5]);

void setup()
{
  Serial.begin(9600);

  lcd.init(); 
  lcd.backlight();
  lcd.setCursor(0,1);
  lcd.print("boca");
}

  void loop()
{
}
void leitura()
{
unsigned int tempo = millis();
bool estadoBotao = digitalRead(PIN_A);
static bool estadoAnteriorBotao = 1;
unsigned int tempoAnterior;
static bool acao = 1;
  
  if(estadoBotao != estadoAnteriorBotao 
     && estadoBotao == 1)
    tempoAnterior = tempo;
  
  if(tempo - tempoAnterior > 20)
  {
 
    if(estadoBotao != acao && estadoBotao == 1)
    {
    Serial.println("Pressionado");
    }
    acao = estadoBotao;
  
   estadoAnteriorBotao = estadoBotao;
}
}