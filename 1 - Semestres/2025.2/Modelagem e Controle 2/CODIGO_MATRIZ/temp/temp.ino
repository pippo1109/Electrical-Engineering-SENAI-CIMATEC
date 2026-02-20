
/*
 * Medidor de Temperatura com LM35 - Alta Velocidade de Amostragem
 * 
 * Conexões:
 * LM35 VCC -> Arduino 5V
 * LM35 OUT -> Arduino A0
 * LM35 GND -> Arduino GND
 */

const int sensorPin = A0;        // Pino analógico para o LM35
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

void setup() {
  Serial.begin(9600);          // Aumentado para 115200 baud para maior velocidade
  
  // Inicializa o array de leituras para média móvel
  for (int i = 0; i < numLeituras; i++) {
    leituras[i] = analogRead(sensorPin); // Lê valor inicial
    total += leituras[i];
  }
  
  // Inicializa temperatura filtrada
  temperaturaFiltrada = (total / numLeituras * 5.0 / 1023.0) * 100.0;
  
  ultimoTempo = millis();
  ultimoResetStats = millis();
}

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