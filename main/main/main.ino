//#include <ESP8266WiFi.h>
#include <WiFiClient.h>
//#include <ESP8266WebServer.h>
//#include <ESP8266mDNS.h>
#include <WiFi.h>
#include <MQTT.h>
#include <MQTTClient.h>

const char topic[]  = "####";
  #define BROKER_IP    "####"
  #define DEV_NAME     "####"
  #define MQTT_USER    "####"
  #define MQTT_PW      "####"
  #define TOPIC_SUBSCRIBE "####"   //Informe um Tópico único. Caso sejam usados tópicos em duplicidade, o último irá eliminar o anterior.

  WiFiClient wifiClient;
  MQTTClient mqttClient;

const char* ssid = "####";// 
const char* password = "####";
int status = WL_IDLE_STATUS;     // the Wifi radio's status

//WiFiClient client;
char server[] = "####";   //eg: 192.168.0.222

int Porcento;
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

void IRAM_ATTR pulseCounter()
{
  pulseCount++;
}

void setup() {

  //wifiConnection();
  Serial.begin(115200);

  pinMode(Rele, OUTPUT); // Declara o Rele como Saída Digital 
  pinMode(PinoDigital, INPUT);
  pinMode(fluxo_agua, INPUT_PULLUP);

  attachInterrupt(digitalPinToInterrupt(fluxo_agua), pulseCounter, FALLING);

  mqttClient.begin(BROKER_IP, 1883, wifiClient);

  conectawifi();  
  conectamqtt();

}

int cont = 0;

void loop() 
{
  
  sensor_fluxo_agua();
  umidade_solo();
  //enviardados(); 
  //mqtt

  mqttClient.loop();
    if (!mqttClient.connected()) {
      conectawifi();
      conectamqtt();
    } 
    enviaValores();
    cont++;
  delay(5000); // interval

}


/*int wifiConnection() 
{
  Serial.begin(115200);
  WiFi.mode(WIFI_OFF);        //Preventa a questão da reconexão (demora demasiado tempo a conectar)
  delay(1000);
  WiFi.mode(WIFI_STA);        //Esta linha oculta a visualização do ESP como hotspot wifi

  WiFi.begin(ssid, password);     //Conectar ao seu router WiFi
  Serial.println("");

  Serial.print("Connecting");
  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
  }


  //Se a ligação for bem sucedida mostrar o endereço IP no monitor de série
  Serial.println("");
  Serial.println("Connected to Network/SSID");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());  // Endereço IP atribuído ao seu ESP
  Serial.begin(115200);
  return (WiFi.localIP());
}
*/

void conectawifi()
  {
     // attempt to connect to Wifi network:
    while (status != WL_CONNECTED) {
      Serial.print("Attempting to connect to network: ");
      Serial.println(ssid);
      // Connect to WPA/WPA2 network:
      status = WiFi.begin(ssid, password);
  
      // wait 10 seconds for connection:
      delay(10000);
    }
  
    // you're connected now, so print out the data:
    Serial.println("You're connected to the network");
    
    Serial.println("----------------------------------------");
    //printData();
    Serial.println("----------------------------------------");
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

    // Porque este ciclo pode não se completar em intervalos de exactamente 1 segundo, calculamos
    // o número de milissegundos que passaram desde a última execução e utilização
    // que escalar a produção. Aplicamos também o factor de calibração para escalar a produção
    // com base no número de impulsos por segundo por unidade de medida (litros/minuto em
    // neste caso) proveniente do sensor.
    flowRate = ((1000.0 / (millis() - previousMillis)) * pulse1Sec) / calibrationFactor;
    previousMillis = millis();

    // Dividir o caudal em litros/minuto por 60 para determinar quantos litros têm
    // passou pelo sensor neste intervalo de 1 segundo, depois multiplicar por 1000 até
    // converter para mililitros.
    flowMilliLitres = (flowRate / 60) * 1000;

    // Adicionar os mililitros passados neste segundo ao total acumulado
    totalMilliLitres += flowMilliLitres;
    
    // Imprimir o caudal para este segundo em litros / minuto
    Serial.print("Flow rate: ");
    Serial.print(int(flowRate));  // Imprimir a parte inteira da variável
    Serial.print("L/min");
    Serial.print("\t");       // Imprimir espaço na aba

    // Imprimir o total acumulado de litros fluidos desde o início
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

/*
void enviardados()
{
  if(client.connect(server, 80)) {
  Serial.println("connected");
  // Fazer um pedido HTTP:
  Serial.print("GET /salvar.php?humidity=");
  client.print("GET /testcode/dht.php?humidity=");     //SUA URL
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
    // se você não conseguiu uma conexão com o servidor:
    Serial.println("connection failed");
  }
 }
*/
   void conectamqtt()
  {
    Serial.print("\nconnectando MQTT...");
    while (!mqttClient.connect(DEV_NAME));//, MQTT_USER, MQTT_PW)) 
    {
      Serial.print(".");
      delay(1000);
    }
    Serial.println("\nMQTT connected!");
    Serial.print("BROKER_IP=");
    Serial.println(BROKER_IP);
    Serial.print("DEV_NAM=");
    Serial.println(DEV_NAME);
    Serial.print("MQTT_USER=");
    Serial.println(MQTT_USER);
    Serial.print("MQTT_PW=");
    Serial.println(MQTT_PW);
    
  }

void enviaValores() {

  //Ler a umidade 
  mqttClient.publish(topic, "Fluxo de Agua: " + String(flowRate) + " Umidade do Solo: " + String(Porcento) + "%");

}