//Librerías
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <VL53L0X.h>
#include <esp_sleep.h>

//Pines
#define PinCelda 6
#define PinBateria 11
#define SDA_PIN 8  // Pin SDA para I2C
#define SCL_PIN 9  // Pin SCL para I2C

//Constantes
const float Vref1 = 3.3;
const float Vref2 = 6.0;
#define TIME_TO_SLEEP 5
const uint32_t SleepTime = TIME_TO_SLEEP * 1000000; // Tiempo que va a dormir el ESP32 (us)
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET     -1 // Reset pin (using Arduino pin numbering), or -1 if not used (some displays might be wired to share the microcontroller's reset pin).
#define SCREEN_ADDRESS 0x3C // Dirección de esclavo de la pantalla OLED

//Variables generalesu
float Celda_value = 0.0;
float Battery_value = 0.0;
float Masa_Celda = 0.0;
uint16_t Distance_measur = 0;

RTC_DATA_ATTR int wakeups = 0;

// Inicialización sensores y pantalla
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
VL53L0X Sensor_dist;


void setup() {
  Serial.begin(115200);
  Wire.begin(SDA_PIN, SCL_PIN);
  Serial.println("Iniciando...");

  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    //Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }
  
  if (Sensor_dist.init() != true){
    //Serial.println("No se pudo inicializar el sensor de distancia");
  }
  
  ++wakeups;
  Serial.println("wakeup number: " + String(wakeups));

  //Lectura analógica
  Celda_value = analogRead(PinCelda);
  Battery_value = analogRead(PinBateria);
  
  //Conversión de voltaje
  Celda_value = Celda_value * (Vref1 / 8191.0);
  Battery_value = Battery_value * (Vref2 / 8191.0);

  //Conversión de masa y voltaje batería
  Masa_Celda = (Celda_value + 0.8564175) / (0.00204);

  //Sensor de distancia
  Distance_measur = Sensor_dist.readRangeSingleMillimeters();

  //Rectificación del valor medido
  Serial.print("Voltaje medido: ");
  Serial.println(Celda_value);
  Serial.print("Masa calculada: ");
  Serial.println(Masa_Celda);
  Serial.print("Voltaje batería: ");
  Serial.println(Battery_value);
  Serial.print("Distancia de ubicación del objeto: ");
  Serial.println(Distance_measur);
  Serial.print("\n");

  // Mostrar en la pantalla OLED
  displayValues();

  // MODO DEEP SLEEP
  esp_sleep_enable_timer_wakeup(SleepTime);
  esp_deep_sleep_start();
}

void loop() {

}


//Funciones extra
void displayValues() {
  // Mostrar en la pantalla OLED
  display.clearDisplay();
  display.setTextColor(WHITE);

  // Mostrar voltaje sensado
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.print("Volt. sensado: ");
  display.print(Celda_value, 2); // Mostrar el valor con 3 decimales
  display.println(" V");

  // Mostrar masa calculada
  display.setTextSize(1);
  display.setCursor(0, 15);
  display.print("Masa: ");
  display.print(Masa_Celda, 2); // Mostrar el valor con 3 decimales
  display.println(" Kg");

  // Mostrar nivel de batería
  display.setTextSize(1);
  display.setCursor(0, 30);
  display.print("Volt. Bateria: ");
  display.print(Battery_value, 2); // Mostrar el valor con 2 decimales
  display.println(" V");

  // Mostrar distancia de ubicación del objeto
  display.setTextSize(1);
  display.setCursor(0, 45);
  display.print("Distancia: ");
  display.print(Distance_measur); // Mostrar el valor con 2 decimales
  display.println(" mm");
  
  display.display();           // Mostrar en la pantalla
}
