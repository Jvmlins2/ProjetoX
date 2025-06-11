#include <Arduino.h>
#include <DHT.h>

#define DHTPIN 5
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);


//*variáveis para funcois DHTtemperatura 
float temperatura = 0;

//*variáveis para funcois DHTumidade
float umidade = 0;

//*variáveis de tempo
int tempoInteracao = 4000;

//*variáveis para funcois setupDHT
float setupUmidade = 0;
float setupTemperatura = 0;

//*variáveis para funcois ataqueFogo
int forcaAtaqueFogo = 0;

//*variáveis para funcois ataqueGelo
int forcaAtaqueGelo = 0;

//!-------Funcois

void DHTtemperatura();
void DHTumidade ();
void setupDHT();
void ataqueGelo ();
void ataqueFogo ();

void setup()
{
 Serial.begin(9600);
 dht.begin();
 setupDHT();
 Serial.print("temp");
 Serial.println(setupTemperatura);
 Serial.print("umidade");
 Serial.println(setupUmidade);
}

void loop() 
{
 DHTtemperatura();
 DHTumidade ();

  //Serial.println(temperatura);
  Serial.println(umidade);
  Serial.println("----------------");
  ataqueFogo ();
  ataqueGelo ();
  delay(1000);
}


void setupDHT ()
{
  delay(2000);
  setupTemperatura = dht.readTemperature();
  setupUmidade = dht.readHumidity();
}




void DHTtemperatura ()
{
  unsigned long ultimoEnvio = 0;
  unsigned long agora = millis();
  if (agora-ultimoEnvio >= tempoInteracao)
{
  temperatura = dht.readTemperature();
  ultimoEnvio = agora;
}
}

void DHTumidade ()
{
  unsigned long ultimoEnvio = 0;
  unsigned long agora = millis();
  if (agora-ultimoEnvio >= tempoInteracao)
{
  umidade = dht.readHumidity();
  ultimoEnvio = agora;
}
}

void ataqueFogo ()
{
  if (temperatura>= (setupTemperatura* 1.25))
  {
    forcaAtaqueFogo = 2;
  }
  else if (temperatura <= (setupTemperatura*1.24) && temperatura >= (setupTemperatura*1.15) )
  {
    forcaAtaqueFogo = 1;
  }
  else 
  {
    forcaAtaqueFogo = 0;
  }
  Serial.println(forcaAtaqueFogo);
}

void ataqueGelo ()
{
  if (umidade>= (setupUmidade* 1.52))
  {
    forcaAtaqueGelo = 2;
  }
  else if (umidade <= (setupUmidade*1.57) && umidade >= (setupUmidade*1.35))
  {
    forcaAtaqueGelo = 1;
  }
  else
  {
    forcaAtaqueGelo = 0;
  }
  Serial.println(forcaAtaqueGelo);
}
