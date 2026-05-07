#include <Servo.h>
#include <LiquidCrystal_I2C.h>
#include "DHT.h"

// --- CONFIGURACIÓN DE PINES (Según tu esquema de colores) ---
#define DHTPIN 7
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);
Servo ascensor;
LiquidCrystal_I2C lcd(0x27, 16, 2);

const int botones[] = {2, 3, 4, 5, 6}; // Naranja, Amarillo, Azul, Rosa, Marrón
const int ledsIlum[] = {10, 11, 12, 13, A1}; // Los 5 LEDs verdes
const int ledFrio = A2;  // LED Azul (Enfriar)
const int ledCalor = A3; // LED Rojo (Calentar)
const int pinLDR = A0;   // Sensor de luz
const int pinPIR = 8;    // Presencia

// --- VARIABLES DE CONTROL (Según PDF ACME S.A.) ---
float tempActual = 0;
float tempAnterior = 0;
int plantaActual = 0;
const int SETPOINT_TEMP = 25; // Temperatura deseada
const int MARGEN = 3;         // Zona muerta +/- 3 grados
const int angulos[] = {0, 45, 90, 135, 180};
unsigned long ultimoRefresco = 0;

void setup() {
  lcd.init();
  lcd.backlight();
  dht.begin();
  ascensor.attach(9);
  ascensor.write(angulos[0]);

  for(int i=0; i<5; i++) {
    pinMode(botones[i], INPUT_PULLUP);
    pinMode(ledsIlum[i], OUTPUT);
  }
  pinMode(ledFrio, OUTPUT);
  pinMode(ledCalor, OUTPUT);
  pinMode(pinPIR, INPUT);

  lcd.print("ACME S.A. READY");
  delay(1500);
}

void loop() {
  // 1. GESTIÓN DE LLAMADAS (MOVIMIENTO)
  for (int i = 0; i < 5; i++) {
    if (digitalRead(botones[i]) == LOW) {
      viajar(i);
    }
  }

  // 2. CONTROL DE ILUMINACIÓN (PROPORCIONAL INVERSO)
  // El PDF pide mantener 80% de luz. Si baja, encendemos LEDs.
  int nivelLuz = analogRead(pinLDR); 
  int numLedsEncendidos = map(nivelLuz, 0, 1023, 5, 0); 
  for(int i=0; i<5; i++) {
    digitalWrite(ledsIlum[i], i < numLedsEncendidos ? HIGH : LOW);
  }

  // 3. CONTROL DE TEMPERATURA (ALGORITMO ZONA MUERTA +/- 3)
  if (millis() - ultimoRefresco > 2000) { // Leer cada 2 seg para ver cambios
    tempAnterior = tempActual;
    tempActual = dht.readTemperature();
    ultimoRefresco = millis();
  }

  controlClima();
  mostrarHMI();
}

void viajar(int destino) {
  if (destino == plantaActual) return;
  
  lcd.clear();
  if (destino > plantaActual) lcd.print("SUBIENDO A P");
  else lcd.print("BAJANDO A P");
  
  lcd.print(destino == 0 ? "G" : String(destino));
  ascensor.write(angulos[destino]);
  delay(2000); // Tiempo de viaje
  plantaActual = destino;
}

void controlClima() {
  // Lógica Figura 3 del PDF: Tres posiciones sin histéresis
  if (tempActual > (SETPOINT_TEMP + MARGEN)) {
    digitalWrite(ledFrio, HIGH);  // Activamos enfriamiento (>28°C)
    digitalWrite(ledCalor, LOW);
  } 
  else if (tempActual < (SETPOINT_TEMP - MARGEN)) {
    digitalWrite(ledFrio, LOW);
    digitalWrite(ledCalor, HIGH); // Activamos calentamiento (<22°C)
  } 
  else {
    digitalWrite(ledFrio, LOW);   // Zona muerta: ahorro energético
    digitalWrite(ledCalor, LOW);
  }
}

void mostrarHMI() {
  lcd.setCursor(0, 0);
  // Requisito PDF: Mostrar Temperatura Anterior y Actual
  lcd.print("Ant:"); lcd.print(tempAnterior, 0); 
  lcd.print("C Act:"); lcd.print(tempActual, 0); lcd.print("C ");

  lcd.setCursor(0, 1);
  // Mostrar estado de control
  if (digitalRead(ledFrio)) lcd.print("ESTADO: ENFRIAR ");
  else if (digitalRead(ledCalor)) lcd.print("ESTADO: CALENTAR");
  else lcd.print("ESTADO: OPTIMO  ");
}
