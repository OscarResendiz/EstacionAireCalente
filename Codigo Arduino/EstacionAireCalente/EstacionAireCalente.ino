/**
Controlador de la estacion de aire caliente
*/
//libreria para el sensor de temperatura
#include "max6675.h"
//incluyo la libreria para el LCD
#include <LiquidCrystal.h>
//Crear el objeto LCD con los números correspondientes (rs, en, d4, d5, d6, d7)
LiquidCrystal lcd(8, 3, 4, 2, 1, 0); 
//variables para el control de la temperatura
const int PotTemperatura = A0;  // Pin de entrada analogica donde esta conectado el protenciometro que controla la temperatura de la estacion de aire caliente
const int Calefactor = 9; //pin de Salida PWM que controla la resistencia de la estacion 
int Temperatura = 0;        // Almacena la temperatura seleccionada por el usuario por medio del potenciometro
int TemperaturaReal; //almacena la temperatura de la pistola
int Activo=0; //indica si la pistola esta activa o en reposo

//variables para el control del ventilador
const int PotVentilador=A1; //pin de entrada analogica donde esta conectado el potenciometro que controla la velocidad del ventilador
const int Ventilador=5; //pin de salida PWM para controlar el nivel de aire de la estacion
int NivelAire = 50;        // Valor del siclo de trabajo del PWM para el nivel de aire en porcentaje
int PWMNivelAire =100; //vlor equivalete al nivel de air pero del valor real del PWM

const int PinMagnetico=A2; //pin e entrada digital para detctar el sensor magnetico y decdir si la pistola esta en modo de descanso
int Magnetico=1; //variable que va a contener el valor del sensor magnetico. si es 0 significaque la pistola esta colocada en su base y si es 1 el usuario la esta utilizando

// CONFIGURACION DE LOS PINES UTILIZADOS PARA LA COMUNICACIÓN CON EL MAX6675
#define CONFIG_TCGND_PIN      8
#define CONFIG_TCVCC_PIN      9
#define CONFIG_TCSCK_PIN      10
#define CONFIG_TCCS_PIN       11
#define CONFIG_TCDO_PIN       12

// OBJETO UTILIZADO PARA LA COMUNICACION CON EL MAX6675
MAX6675 thermocouple(CONFIG_TCSCK_PIN, CONFIG_TCCS_PIN, CONFIG_TCDO_PIN);

void setup() {
  // PREPARAR LA INTERFAZ //Serial
  //Serial.begin(9600);

  // USAR PINES DE ARDUINO PARA ALIMENTAR EL MODULO MAX6675
  pinMode(CONFIG_TCVCC_PIN, OUTPUT); 
  digitalWrite(CONFIG_TCVCC_PIN, HIGH);
  pinMode(CONFIG_TCGND_PIN, OUTPUT); 
  digitalWrite(CONFIG_TCGND_PIN, LOW);
  //configuramos pines
  pinMode(Calefactor,OUTPUT);
  // Inicializar el LCD con el número de  columnas y filas del LCD
  lcd.begin(16, 2);
  // Escribimos el Mensaje en el LCD.
  lcd.print("Estacion de aire caliente");
  // ESPERAR A QUE SE ESTABILICE LA ENERGIA
  delay(500);
}
//funcion que lee todos los valores de los sensores y controles
void LeeSensores()
{
  // leemos la temperatura de la estacion de calor
  TemperaturaReal=thermocouple.readCelsius()*.59;
  //ahora leo la temperatura del potenciometro
  Temperatura =map(analogRead(PotTemperatura), 0, 1023, 200, 450); 
  //leo la velocidad del potenciometo el ventilador
  NivelAire=map(analogRead(PotVentilador),0,1023,1,99); 
  PWMNivelAire=map(analogRead(PotVentilador),0,1023,0,254); 
  //leo el sensor magnetico
  Magnetico=digitalRead( PinMagnetico);

}
//funcion que activa la pistola porque el usuario la esta ocupando
void PistolaActiva()
{
  Activo=1; //indico que esta activo
  //regulo la temperatura de la pistola
  if(TemperaturaReal<Temperatura)
  {
    //como la teperatura es menor, activo la resistencia
    digitalWrite(Calefactor,HIGH);
  }
  else
  {
    //la temperatura se paso, por lo que dejo que se enfrie
    digitalWrite(Calefactor,LOW);
  }
  //activo el ventilador
  analogWrite(Ventilador,PWMNivelAire);
}
//funcion que pone la pistola en modo de reposo
void PistolaReposo()
{
    //desactivo el calefactor
    digitalWrite(Calefactor,LOW);
  if(TemperaturaReal>100 && Activo==1)
  {
    //como la temperatura es superior a los 100 grados, activo el ventilador al maximo para que enfrie la pistola
    NivelAire=99;
    PWMNivelAire=254;
  }
  else
  {
    //como la temperatura ya bajo, apago el ventilador
    NivelAire=0;
    PWMNivelAire=0;
    Activo=0; //ahora si lo marcamos como en reposo
  }
  analogWrite(Ventilador,PWMNivelAire);
}
void loop() 
{
  //leemos los sensores
  LeeSensores();
  if(Magnetico==1)
  {
    //si el magnetico esta en 1 significa que esta siendo utilizada y hay que activarla resistencia y el ventilador
    PistolaActiva();
  }
  else
  {
    //la pistola esta en su base por lo que hay que enfriar la pistola de calor
    PistolaReposo();
  }
  //muestro los datos por la pantalla
  //limpiamos la pantalla
  lcd.clear();
  // Ubicamos el cursor en la primera posición(columna:0) de la segunda línea(fila:1)
  lcd.setCursor(0, 0);
  //muestro los datos de la temperatura
  lcd.print("Temp: ");
  lcd.print(Temperatura );
  lcd.print(" : ");
  lcd.print(TemperaturaReal );
  //muestro la velocidad del ventilador
  lcd.setCursor(0, 1);
  lcd.print("Aire:");
  lcd.print(NivelAire);
  if(Magnetico==1)
  {
    lcd.print(" Activo");
  }
  else
  {
    lcd.print(" Reposo");
  }
  //me espero un rato para volver a analizar
  delay(500) ;
}
