// Importação de bibliotecas
#include <WiFi.h>       // Biblioteca WiFi para ESP32
#include <Wire.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <Adafruit_GFX.h> // Biblioteca para o display oled
#include <Adafruit_SSD1306.h> // Biblioteca para o display oled

// Configurações Wi-Fi
const char* ssid = "Guto Rapido";
const char* pass = "familiarg_33";

// Configurações do cliente NTP
WiFiUDP ntpUDP;
NTPClient timeclient(ntpUDP, "pool.ntp.org", -3 * 3600); // Fuso horário (-3h UTC)

// Definições do Display
#define width_display 128
#define height_display 64
#define reset_display -1
Adafruit_SSD1306 display(width_display, height_display, &Wire, reset_display);

// Configurações HTTP e API
const char* apiEndpoint = "http://api.weatherapi.com/v1/current.json?key=410b58086b6b4128a82175631242811&q=Sao%20Paulo&aqi=no";
String getTemperature(String api){
  String temperature; 
  if (WiFi.status() == WL_CONNECTED){
    HTTPClient http;
    http.begin(api);
    http.addHeader("Content-Type", "application/json");
    int httpResponseCode = http.GET();

    if (httpResponseCode > 0){
      String response = http.getString();
      Serial.println("Response: " + response); // retorno do json
      Serial.println(httpResponseCode);
      // Fazendo o parsing do JSON
      StaticJsonDocument<2048> doc;
      DeserializationError error = deserializeJson(doc, response);

      if (!error){
        JsonObject obj = doc["current"];  // Obtendo o objeto correto
        temperature = String(obj["temp_c"].as<float>());  // Convertendo para float e depois para String
      }
    }
  }
  return temperature;
}

void setup() {
  // Inicializa a comunicação serial
  Serial.begin(115200);

  // Inicializando o Display
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)){
    Serial.println(F("Falha ao inicializar o display"));
    while(1);
  } else {
    Serial.println("Display Inicializado");
  }
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(WHITE);

  // Conecta ao Wi-Fi
  Serial.println("Conectando ao WiFi...");
  WiFi.begin(ssid, pass);

  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
  }

  Serial.println("\nConectado ao WiFi: " + String(ssid));
  Serial.print("Endereço IP: ");
  Serial.println(WiFi.localIP());

  // Inicializa o cliente NTP
  timeclient.begin();
}

void loop() {
  String temperature = getTemperature(apiEndpoint);
  Serial.println(temperature);

  // Atualiza o cliente NTP
  timeclient.update();

  // Limpar display
  display.clearDisplay();
  
  // Exibir informações no display
  display.setCursor(48,0);
  display.println("Time");
  display.println(timeclient.getFormattedTime());
  display.println(temperature + " C");
  // Atualizar o display
  display.display();
  
  delay(1000); // Atualiza a cada segundo
}
