  /*============================================================
  Proyecto: Radar de presencia LD2410 sobre ESP32
  Profesor: Yan Romero
  Estudiantes: Cristofer Medina, Fabián Sánchez, Gerald Villarruel, Daniel Paz, Luis Canelón, Eduar Márquez, Miguel Barrios
  ============================================================*/

#include <ld2410.h>

// DEFINICIONES ESPECÍFICAS PARA ESP32-WROOM-32
#define MONITOR_SERIAL Serial   // Serial es para la comunicación con la PC (Processing) a 115200
#define RADAR_SERIAL   Serial1    // Serial1 es para la comunicación con el radar a 256000 baudios
#define RADAR_RX_PIN 32 // GPIO del ESP32 conectado al TX del RADAR 
#define RADAR_TX_PIN 33 // GPIO del ESP32 conectado al RX del RADAR 

ld2410 radar;
uint32_t lastReading = 0;

void setup(void)
{
  // 1. Inicializa la comunicación con la PC para enviar datos a Processing
  MONITOR_SERIAL.begin(115200); 
  
  // 2. Inicializa la comunicación con el radar LD2410
  // Usa Serial1 con los pines 32 (RX) y 33 (TX)
  RADAR_SERIAL.begin(256000, SERIAL_8N1, RADAR_RX_PIN, RADAR_TX_PIN); 
  delay(500);

  MONITOR_SERIAL.println(F("\n--- INICIALIZACIÓN DE HARDWARE ---"));
  MONITOR_SERIAL.print(F("Conectar TX del radar LD2410 al GPIO: "));
  MONITOR_SERIAL.println(RADAR_RX_PIN); 
  MONITOR_SERIAL.print(F("Conectar RX del radar LD2410 al GPIO: "));
  MONITOR_SERIAL.println(RADAR_TX_PIN);

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
    MONITOR_SERIAL.println(F("Enviando datos a Processing."));
  }
  else
  {
    MONITOR_SERIAL.println(F("Error de conexión con radar. Revisar cableado/energía"));
  }
}

void loop()
{
  radar.read();

  // Envía datos cada 100 milisegundos para una gráfica fluida
  if(radar.isConnected() && millis() - lastReading > 100) 
  {
    lastReading = millis();
    
    // El formato de salida es: [distancia_movil] [espacio] [distancia_estacionario] [salto de línea]
    
    // 1. Distancia del objetivo en movimiento
    if(radar.movingTargetDetected())
    {
      MONITOR_SERIAL.print(radar.movingTargetDistance());
    } 
    else 
    {
      MONITOR_SERIAL.print(0); // Si no hay movimiento, envía 0
    }
    
    MONITOR_SERIAL.print(' '); // Añade el ESPACIO como separador

    // 2. Distancia del objetivo estacionario
    if(radar.stationaryTargetDetected())
    {
      MONITOR_SERIAL.println(radar.stationaryTargetDistance());
    }
    else
    {
      MONITOR_SERIAL.println(0); // Si no hay estático, envía 0
    }
    
    // La combinación anterior asegura que siempre se envía algo como "250 150\n" o "0 0\n"
  }
}
