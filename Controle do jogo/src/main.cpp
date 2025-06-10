# include <Arduino.h>

# define PIN_A 4
# define PIN_B 2
# define PIN_C 12
# define PIN_D 13
# define PIN_E 25

void leituraBotoes(bool leitura[5]);

void setup()
{
  pinMode(PIN_A, INPUT_PULLUP);
  pinMode(PIN_B, INPUT_PULLUP);
  pinMode(PIN_C, INPUT_PULLUP);
  pinMode(PIN_D, INPUT_PULLUP);
  pinMode(PIN_E, INPUT_PULLUP);
  Serial.begin(9600);
}

void loop()
{
  bool botao[5];
  leituraBotoes(botao);

  if(botao[0] == 1)
  Serial.println("A");
  if(botao[1] == 1)
  Serial.println("B");
  if(botao[2] == 1)
  Serial.println("C");
  if(botao[3] == 1)
  Serial.println("D");
  if(botao[4] == 1)
  Serial.println("E");

}

void leituraBotoes(bool leitura[5])
{
  unsigned int tempo = millis();
static bool verdadeiro = 1;

if( tempo > 100)
{
  //? Botão A
  bool estadoA = digitalRead(PIN_A);
  static bool estadoAnteriorA = 1;
  static unsigned int tempoAnteriorA;
  static bool acaoA =1;

  if (estadoA != estadoAnteriorA && estadoA == 0)
    tempoAnteriorA = tempo;
  if (tempo - tempoAnteriorA > 100)
  {
    //Serial.println(acaoD);
    if (acaoA != estadoA)
    {
    if(estadoA == 0)
    leitura[0] = 1;
    }
    else
    {
    leitura[0]=0;
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
    //Serial.println(acaoD);
    if (acaoB != estadoB)
    {
    if(estadoB == 0)
    leitura[1] = 1;
    }
    else
    {
    leitura[1]=0;
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
    if (tempo - tempoAnteriorC > 100)
  {
    //Serial.println(acaoD);
    if (acaoC != estadoC)
    {
    if(estadoC == 0)
    leitura[2] = 1;
    }
    else
    {
    leitura[2]=0;
    }
    acaoC = estadoC;
  }
    estadoAnteriorC = estadoC;

  //? Botão D
  bool estadoD = digitalRead(PIN_D);
  static bool estadoAnteriorD = 1;
  static unsigned int tempoAnteriorD;
  static bool acaoD =1;
  if (estadoD != estadoAnteriorD)
    tempoAnteriorD = tempo;
  if (tempo - tempoAnteriorD > 100)
  {
    //Serial.println(acaoD);
    if (acaoD != estadoD)
    {
    if(estadoD == 0)
    leitura[3] = 1;
    }
    else
    {
    leitura[3]=0;
    }
    acaoD = estadoD;
  }
    estadoAnteriorD = estadoD;


  //? Botão E
  bool estadoE = digitalRead(PIN_E);
  static bool estadoAnteriorE = 1;
  static unsigned int tempoAnteriorE;
  static bool acaoE =1;
  if (estadoE != estadoAnteriorE )
    tempoAnteriorE = tempo;
 if (tempo - tempoAnteriorE > 100)
  {
    //Serial.println(acaoD);
    if (acaoE != estadoD)
    {
    if(estadoE == 0)
    leitura[4] = 1;
    }
    else
    {
    leitura[4]=0;
    }
    acaoE = estadoE;
  }
    estadoAnteriorE = estadoE;
}
}