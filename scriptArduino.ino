#if defined(ESP32)
    #ifdef ESP_IDF_VERSION_MAJOR // IDF 4+
      #if CONFIG_IDF_TARGET_ESP32 // ESP32/PICO-D4
        #define MONITOR_SERIAL Serial
        #define RADAR_SERIAL Serial1
        #define RADAR_RX_PIN 32
        #define RADAR_TX_PIN 33
      #elif CONFIG_IDF_TARGET_ESP32S2
        #define MONITOR_SERIAL Serial
        #define RADAR_SERIAL Serial1
        #define RADAR_RX_PIN 9
        #define RADAR_TX_PIN 8
      #elif CONFIG_IDF_TARGET_ESP32C3
        #define MONITOR_SERIAL Serial
        #define RADAR_SERIAL Serial1
        #define RADAR_RX_PIN 4
        #define RADAR_TX_PIN 5
      #else 
        #error Target CONFIG_IDF_TARGET is not supported
      #endif
    #else // ESP32 antes IDF 4.0 - Fabián Sánchez
      #define MONITOR_SERIAL Serial
      #define RADAR_SERIAL Serial1
      #define RADAR_RX_PIN 32
      #define RADAR_TX_PIN 33
    #endif
#endif

#include <ld2410.h>

ld2410 radar;

uint32_t lastReading = 0;
bool radarConnected = false;

void setup(void)
{
  MONITOR_SERIAL.begin(115200); //Retroalimentación por Monitor Serial
  //radar.debug(MONITOR_SERIAL); //Descomenta para mostrar información de depuración de la librería en el Monitor Serial. Por defecto no muestra lecturas del sensor ya que son muy frecuentes. - Eduar Marquez
  
  RADAR_SERIAL.begin(256000, SERIAL_8N1, RADAR_RX_PIN, RADAR_TX_PIN); //UART para monitorear el radar (Receptor/tranmisor asíncrono universal, su función es convertir los datos en formato serie para su transmisión y volverlos a convertir al llegar al receptor) - Luis Canelon
  
  delay(500);
  MONITOR_SERIAL.print(F("\nConectar TX del radar LD2410 al GPIO:"));
  MONITOR_SERIAL.println(RADAR_RX_PIN); //Que pines son Exactamente
  MONITOR_SERIAL.print(F("Conectar RX del radar LD2410 al GPIO:"));
  MONITOR_SERIAL.println(RADAR_TX_PIN);//Que pines son Exactamente
  MONITOR_SERIAL.print(F("Inicializando sensor radar LD2410: ")); 
  if(radar.begin(RADAR_SERIAL))
  {
    MONITOR_SERIAL.println(F("OK"));
    MONITOR_SERIAL.print(F("Versión de firmware LD2410: "));
    MONITOR_SERIAL.print(radar.firmware_major_version);
    MONITOR_SERIAL.print('.');
    MONITOR_SERIAL.print(radar.firmware_minor_version);
    MONITOR_SERIAL.print('.');
    MONITOR_SERIAL.println(radar.firmware_bugfix_version, HEX);
  }
  else
  {
    MONITOR_SERIAL.println(F("no conectado"));
  }
}

void loop()
{
  radar.read();
  if(radar.isConnected() && millis() - lastReading > 1000)  //Reporte cada 1000 ms (Envia un reporte cada 1000 milisegundos) - Luis Canelon
  {
    lastReading = millis();
    if(radar.presenceDetected())
    { 
      if(radar.stationaryTargetDetected())
      {
        Serial.print(F("Objetivo estacionario: ")); // Objetivo estacionario (ej. una persona quieta) - Fabián Sánchez 
        Serial.print(radar.stationaryTargetDistance()); //Distancia al objetivo estacionario - Fabián Sánchez 
        Serial.print(F("cm energía:"));
        Serial.print(radar.stationaryTargetEnergy()); //Nivel de energía/reflectividad del objetivo - Eduar Marquez
        Serial.print(' ');
      }
      if(radar.movingTargetDetected())
      {
        Serial.print(F("Objetivo en movimiento: ")); // Objetivo en movimiento (ej. una persona andando) - Fabián Sánchez
        Serial.print(radar.movingTargetDistance()); //Distancia al objetivo en movimiento - Eduar Marquez
        Serial.print(F("cm energía:"));
        Serial.print(radar.movingTargetEnergy()); //Nivel de energía del objetivo en movimiento - Eduar Marquez
      }
      Serial.println();
    }
    else
    {
      Serial.println(F("Sin objetivos"));
    }
  }
}
