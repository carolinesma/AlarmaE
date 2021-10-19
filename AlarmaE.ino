#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

//Dados da conexão Wi-fi

const char* ssid     = "SSID";
const char* password = "Password";

HTTPClient http; // Declara variável para para p HttpClient

int ledVerm = 5;   // Pino ligado ao led vermelho
int ledVerde = 4;   // Pino ligado ao led verde
int sensor = 0;  // Pino ligado ao sensor PIR
int valorSensor = 0;   // Variavel para guardar valor do sensor
int httpCode = 0; // Variável para código de retorno do HttpClient
int aux = 0; // Auxiliar para armazenar o estado anterior do sensor

void setup() {

  Serial.begin(115200);
  delay(10);

  pinMode(ledVerm, OUTPUT);  //Define pino D0 como saida
  digitalWrite(ledVerm, LOW);
  pinMode(ledVerde, OUTPUT);  //Define pino D1 como saida
  digitalWrite(ledVerde, LOW);
  pinMode(sensor, INPUT);   //Define pino D3 como entrada


  WiFi.begin(ssid, password); // Inicia conexão Wifi


  while (WiFi.status() != WL_CONNECTED) { // Aguarda a conexão ser estabelecida

    Serial.println("...");
    delay(1000);
  }

  Serial.println("Conectado");
}

void loop() {

  verificaConexao(); //Chama função para verificar a conexão Wifi

  valorSensor = digitalRead(sensor); // Verifica sinal so sensor
  digitalWrite(ledVerm, valorSensor); //Seta o valor do sensor no Led vermelho

  if ((valorSensor == 1) && (aux != 1)) { // Verifica o valor do sensor e da variável auxiliar para não repetir a mesma informação,
                                          // entra no if apenas se o estado anterior do sensor foi 0
    enviaMensagem(); // Chama função para o envio da mensagem para o API

    if (httpCode >= 0) { //httpCode maior > 0 indica que o servidor respondeu corretamente

      aux = 1; // Armazena o estado do sensor
    }

    else { //Caso apresente erro de comunicação com o servidor

      while (httpCode < 0) { //Tentativa de realizar o get até resultado positivo

        verificaConexao(); // Verifica conexão Wifi
        enviaMensagem(); // Repete o envio da mensagem
      }
    }
  }

  else if (valorSensor == 0) {
    aux = 0; // Armazena o estado do sensor
  }
}

void verificaConexao() {

  if (WiFi.status() == WL_CONNECTED) { // Confirma a conexão com a rede Wifi

    digitalWrite(ledVerde, HIGH); // Indica Nodemcu Conectado
  }

  else { // Solicita conexão com a rede Wifi

    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED) { //Enquanto Wifi não conecta
      digitalWrite(ledVerde, LOW); // Desliga Led Verde
      digitalWrite(ledVerm, HIGH); // Liga Led Vermelho
      Serial.println("...");
      delay(1000);
    }

    Serial.println("Conectado"); //Wifi conectado
    digitalWrite(ledVerm, LOW); //Retorna o estado dos Leds
    digitalWrite(ledVerde, HIGH);
  }
}

int enviaMensagem() {

  http.begin("http://iot.wifiaqui.com.br/alarme/?presenca=cadastrar&localidade=Nome-do-Lugar"); //Envia o alerta para a API
  httpCode = http.GET(); //Recebe código para confirmação do get
  http.end(); //Finaliza o Client
  delay(100);
  return httpCode; //Retorna o HttpCode
}



