#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

const char* ssid = "################";// 
const char* password = "################";

//WiFiClient client;
char server[] = "################";   //eg: 192.168.0.222

int PinoAnalogico = 13; // Define o pino 13 como Pino Analógico do sensor
int PinoDigital = 12; // Define pino D14 como Pino Digital do Sensor 

int Rele = 14; // Pino Digital D1 como Relé

int EstadoSensor = 0;
int UltimoEstSensor = 0;

int  ValAnalogIn; // Valor analógico no código

int contador = 0;
int fluxo_agua = 4;

long currentMillis = 0;
long previousMillis = 0;
int interval = 1000;
float calibrationFactor = 4.5;
volatile byte pulseCount;
byte pulse1Sec = 0;
float flowRate;
unsigned int flowMilliLitres;
unsigned long totalMilliLitres;

WiFiClient client;    

void IRAM_ATTR pulseCounter()
{
  pulseCount++;
}

void setup() {

  wifiConnection();
  Serial.begin(115200);

  pinMode(Rele, OUTPUT); // Declara o Rele como Saída Digital 
  pinMode(PinoDigital, INPUT);
  pinMode(fluxo_agua, INPUT_PULLUP);

  attachInterrupt(digitalPinToInterrupt(fluxo_agua), pulseCounter, FALLING);

}


void loop() 
{
  sensor_fluxo_agua();
  umidade_solo();
  enviardados(); 
  delay(30000); // interval

}


int wifiConnection() 
{
  Serial.begin(115200);
  WiFi.mode(WIFI_OFF);        //Prevents reconnection issue (taking too long to connect)
  delay(1000);
  WiFi.mode(WIFI_STA);        //This line hides the viewing of ESP as wifi hotspot

  WiFi.begin(ssid, password);     //Connect to your WiFi router
  Serial.println("");

  Serial.print("Connecting");
  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
  }

  //If connection successful show IP address in serial monitor
  Serial.println("");
  Serial.println("Connected to Network/SSID");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());  //IP address assigned to your ESP
  Serial.begin(115200);
  return (WiFi.localIP());
}

void sensor_fluxo_agua(){

  pulseCount = 0;
  flowRate = 0.0;
  flowMilliLitres = 0;
  totalMilliLitres = 0;
  previousMillis = 0;

  currentMillis = millis();
  if (currentMillis - previousMillis > interval) 
  {
    
    pulse1Sec = pulseCount;
    pulseCount = 0;

    // Because this loop may not complete in exactly 1 second intervals we calculate
    // the number of milliseconds that have passed since the last execution and use
    // that to scale the output. We also apply the calibrationFactor to scale the output
    // based on the number of pulses per second per units of measure (litres/minute in
    // this case) coming from the sensor.
    flowRate = ((1000.0 / (millis() - previousMillis)) * pulse1Sec) / calibrationFactor;
    previousMillis = millis();

    // Divide the flow rate in litres/minute by 60 to determine how many litres have
    // passed through the sensor in this 1 second interval, then multiply by 1000 to
    // convert to millilitres.
    flowMilliLitres = (flowRate / 60) * 1000;

    // Add the millilitres passed in this second to the cumulative total    
    totalMilliLitres += flowMilliLitres;
    
    // Print the flow rate for this second in litres / minute
    Serial.print("Flow rate: ");
    Serial.print(int(flowRate));  // Print the integer part of the variable
    Serial.print("L/min");
    Serial.print("\t");       // Print tab space

    // Print the cumulative total of litres flowed since starting
    Serial.print("Output Liquid Quantity: ");
    Serial.print(totalMilliLitres);
    Serial.print("mL / ");
    Serial.print(totalMilliLitres / 1000);
    Serial.println("L");
  }

}

void umidade_solo(){
  ValAnalogIn = analogRead(PinoAnalogico);
  int Porcento = map(ValAnalogIn, 1023, 0, 0, 100); // Traforma o valor analógico em porcentagem

  Serial.print("Umidade: "); // Imprime o símbolo no valor
  Serial.print(Porcento); // Imprime o valor em Porcentagem no monitor Serial
  Serial.println("%");

  if (Porcento <= 76) { // Se a porcentagem for menor ou igual à 76%. OBS: Você pode alterar essa porcentagem
    
    Serial.println("Irrigando Planta"); // Imprime no monitor serial
    digitalWrite(Rele, LOW); // Aciona Relé

  }else { // Caso contrario 
  
    Serial.println("Planta Irrigada"); // Imprime a no monitor serial
    digitalWrite(Rele, HIGH); // Desliga Relé
    delay (1000);
  }
}


void enviardados()
{
  if(client.connect(server, 3306)) {
  Serial.println("connected");
  // Make a HTTP request:
  Serial.print("GET /testcode/dht.php?humidity=");
  client.print("GET /testcode/dht.php?humidity=");     //YOUR URL
  //Serial.println(humidityData);
  //client.print(humidityData);
  client.print("&temperature=");
  Serial.println("&temperature=");
  //client.print(temperatureData);
  //Serial.println(temperatureData);
  client.print(" ");      //SPACE BEFORE HTTP/1.1
  client.print("HTTP/1.1");
  client.println();
  client.println("Connection: close");
  client.println();
  } else {
    // if you didn't get a connection to the server:
    Serial.println("connection failed");
  }
 }