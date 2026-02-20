// ----------------------------------------------------------------------------------------------------------------------------------

/*
 * Medidor de Alta Precisão para Miliamperes com ACS712 (1ms amostragem)
 * 
 * Conexões:
 * ACS712 VCC → Arduino 5V
 * ACS712 GND → Arduino GND  
 * ACS712 OUT → Arduino A1
 * 
 * Adicionar capacitor de 100nF entre OUT e GND do ACS712
 * para reduzir ruído.
 */

const int sensorPinC = 15;
const int numLeiturasC = 1000;  // Número alto de leituras para melhor precisão
const float vRef = 5.0;        // Tensão de referência do Arduino
const int adcResolution = 1023; // Resolução do ADC (10 bits)
const int intervaloAmostragem = 1; // Amostragem a cada 1ms

// Parâmetros do ACS712 (ESCOLHA O MODELO CORRETO)
//#define MODELO 5    // 185mV/A = 0.185V/A
//#define MODELO 20   // 100mV/A = 0.100V/A  
#define MODELO 30     // 66mV/A = 0.066V/A

float zeroVoltage;    // Tensão de zero corrente (deve ser ~2.5V)
float sensitivity;    // Sensibilidade em V/A

// ----------------------------------------------------------------------------------------------------------------------------------

/*
 * Medidor de Temperatura com LM35 - Alta Velocidade de Amostragem
 * 
 * Conexões:
 * LM35 VCC -> Arduino 5V
 * LM35 OUT -> Arduino A0
 * LM35 GND -> Arduino GND
 */

const int sensorPin = 15;        // Pino analógico para o LM35
const int numLeituras = 50;      // Aumentado para 50 leituras (mais suavização)
const int janelaTemporal = 114;   // Reduzido para 20ms (mais atualizações por segundo)
// Variáveis para média móvel
int leituras[numLeituras];
int indice = 0;
long total = 0;

// Variáveis para estatísticas
float temperaturaMin = 100.0;
float temperaturaMax = -100.0;
unsigned long ultimoResetStats = 0;
const unsigned long intervaloResetStats = 30000; // Reset stats a cada 30 segundos

// Variáveis para cálculo de taxa de amostragem
unsigned long ultimoTempo = 0;
unsigned int contadorAmostras = 0;
float taxaAmostragem = 0;

// Filtro IIR (Infinite Impulse Response) para suavização adicional
float temperaturaFiltrada = 0;
const float alpha = 0.1; // Fator de suavização (0.1 = 10% de cada nova amostra)

// ----------------------------------------------------------------------------------------------------------------------------------

void setup() {
  Serial.begin(115200);          // Aumentado para 115200 baud para maior velocidade
  
  // Inicializa o array de leituras para média móvel
  for (int i = 0; i < numLeituras; i++) {
    leituras[i] = analogRead(sensorPin); // Lê valor inicial
    total += leituras[i];
  }
  
  // Inicializa temperatura filtrada
  temperaturaFiltrada = (total / numLeituras * 5.0 / 1023.0) * 100.0;
  
  ultimoTempo = millis();
  ultimoResetStats = millis();

    // Configura sensibilidade conforme modelo
  #if MODELO == 5
    sensitivity = 0.185; // 185mV/A = 0.185V/A
  #elif MODELO == 20
    sensitivity = 0.100; // 100mV/A = 0.100V/A  
  #elif MODELO == 30
    sensitivity = 0.136; // 66mV/A = 0.066V/A
  #endif
  calibrarSensor();  // Calibração crucial para miliamperes
}

// ----------------------------------------------------------------------------------------------------------------------------------

void loop() {
  
  unsigned long tempoAtual = millis();
  
  // Lê o valor analógico do sensor
  int leitura = analogRead(sensorPin);
  
  // Atualiza média móvel
  total = total - leituras[indice];
  leituras[indice] = leitura;
  total = total + leituras[indice];
  indice = (indice + 1) % numLeituras;
  
  // Calcula temperaturas
  float temperaturaMedia = (total / numLeituras * 5.0 / 1023.0) * 100.0;
  float temperaturaAtual = (leitura * 5.0 / 1023.0) * 100.0;
  
  // Aplica filtro IIR
  temperaturaFiltrada = (1 - alpha) * temperaturaFiltrada + alpha * temperaturaAtual;
  
  // Atualiza estatísticas
  if (temperaturaAtual < temperaturaMin) temperaturaMin = temperaturaAtual;
  if (temperaturaAtual > temperaturaMax) temperaturaMax = temperaturaAtual;
  
  // Calcula taxa de amostragem
  contadorAmostras++;
  if (tempoAtual - ultimoTempo >= 1000) {
    taxaAmostragem = contadorAmostras * 1000.0 / (tempoAtual - ultimoTempo);
    ultimoTempo = tempoAtual;
    contadorAmostras = 0;
  }
  
  // Reset estatísticas periodicamente
  if (tempoAtual - ultimoResetStats >= intervaloResetStats) {
    temperaturaMin = temperaturaAtual;
    temperaturaMax = temperaturaAtual;
    ultimoResetStats = tempoAtual;
  }
  
  // Envia os dados formatados para o Serial Plotter
  Serial.println(temperaturaFiltrada, 2);
  
  // Aguarda o intervalo definido (janelamento rápido)
  delay(janelaTemporal);
}

// ----------------------------------------------------------------------------------------------------------------------------------

void calibrarSensor() {
  
  // Calibração de alta precisão
  double soma = 0;
  for (int i = 0; i < 1000; i++) {
    soma += analogRead(sensorPinC);
    delay(1);
  }
  
  zeroVoltage = (soma / 1000.0) * (vRef / adcResolution);
  
  if (zeroVoltage < 2.4 || zeroVoltage > 2.6) {
    Serial.println("AVISO: Tensão de zero fuera do normal (2.4V-2.6V)");
    Serial.println("Verifique alimentação do sensor!");
  }
}

float medirCorrentePrecisa() {
  // Técnica de oversampling e filtragem digital
  long somaADC = 0;
  
  for (int i = 0; i < numLeiturasC; i++) {
    somaADC += analogRead(sensorPinC);
    delayMicroseconds(1); // Pequeno delay entre leituras
  }
  
  float voltage = (somaADC / (float)numLeiturasC) * (vRef / adcResolution);
  float corrente = (voltage - zeroVoltage) / sensitivity;
  
  // Filtro passa-baixa digital suave
  static float filteredCurrent = 0;
  filteredCurrent = 0.9 * filteredCurrent + 0.1 * corrente;
  
  return filteredCurrent;
}

float lerTensaoSensor() {
  int adcValue = analogRead(sensorPin);
  return adcValue * (vRef / adcResolution);
}