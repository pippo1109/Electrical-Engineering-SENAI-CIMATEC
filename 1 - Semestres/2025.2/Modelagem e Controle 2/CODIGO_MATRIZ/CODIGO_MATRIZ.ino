/*
 * Medição de temperatura com LM35 + Controle PWM
 * Arduino Mega 2560
 * Taxa de amostragem: 100ms
 * Conexão: LM35 VCC -> 5V, OUT -> A0, GND -> GND
 */

// ========== CONFIGURAÇÕES ==========
const int PWM_PIN = 3;             // Pino para saída PWM (0-255)
const int PWM_PIN_OFF = 9; 
int pwmValue = 255;                  // Valor atual do PWM

const int LM35_PIN = A0;           // Pino analógico para LM35
const float VREF = 5.0;            // Tensão de referência do Arduino (5V)
const int ADC_RESOLUTION = 1023;   // Resolução ADC 10 bits (0-1023)
const int SAMPLE_RATE_MS = 100;    // Taxa de amostragem de 100ms

// ========== FILTRO DE MÉDIA MÓVEL ==========
const int NUM_SAMPLES = 10;        // Número de amostras para filtro
float samples[NUM_SAMPLES];        // Array para armazenar amostras
int sampleIndex = 0;               // Índice atual no array

// ========== VARIÁVEIS DE TEMPO ==========
unsigned long previousMillis = 0;  // Armazena último tempo de leitura
unsigned long startTime = 0;       // Tempo de início

// ========== VARIÁVEIS PARA CONTROLE ALEATÓRIO ==========
const int AMOSTRAS_PARA_MUDANCA = 50;  // Muda PWM a cada 10k amostras
unsigned int contadorAmostras = 0;        // Contador de amostras
bool valoresUsados[256] = {false};        // Marca quais valores já foram usados
int valoresDisponiveis[256];              // Array de valores disponíveis
int totalValoresDisponiveis = 0;          // Quantidade de valores disponíveis
bool modoAutomatico = false;              // Modo automático ativo


long sampleCount = 0;
int contador = 0;
// ========== SETUP ==========
void setup() {
  Serial.begin(115200);           // Inicia comunicação serial
  while (!Serial) {               // Aguarda conexão serial
    ; 
  }
  
  startTime = millis();           // Marca tempo de início
  
  // Configuração do pino analógico
  pinMode(LM35_PIN, INPUT);
  
  // Configura referência analógica para padrão (5V)
  analogReference(DEFAULT);
  
  pinMode(PWM_PIN, OUTPUT);
  pinMode(PWM_PIN_OFF, OUTPUT);
  analogWrite(PWM_PIN_OFF, 255);
  analogWrite(PWM_PIN, pwmValue);
  
  // Inicializa array de amostras
  for (int i = 0; i < NUM_SAMPLES; i++) {
    samples[i] = 0.0;
  }
}

// ========== LOOP PRINCIPAL ==========
void loop() {
  unsigned long currentMillis = millis();
  
  // Executa a cada 100ms exatos
  if (currentMillis - previousMillis >= SAMPLE_RATE_MS) {
    previousMillis = currentMillis;
    
    // Lê temperatura
    float rawTemperature = readTemperatureLM35();
    
    // Aplica filtro de média móvel
    float filteredTemperature = movingAverageFilter(rawTemperature);
    
    // Exibe resultados
    displayTemperature(rawTemperature, filteredTemperature);
  }
    // Verifica se deve mudar o PWM (a cada 10k amostras)
    if (contador == AMOSTRAS_PARA_MUDANCA) {
      gerarNovoValorPWM();
      contador = 0;
    }
  
  // Verifica entrada serial para PWM
  checkSerialForPWM();
}

// ========== FUNÇÃO PARA LER TEMPERATURA ==========
float readTemperatureLM35() {
  // Realiza várias leituras para média
  int totalReadings = 0;
  float sum = 0.0;
  
  for (int i = 0; i < 5; i++) {
    int adcValue = analogRead(LM35_PIN);
    
    // Verifica leituras inválidas
    if (adcValue < 0 || adcValue > 1023) {
      continue;
    }
    
    // Converte para tensão
    float voltage = (adcValue * VREF) / ADC_RESOLUTION;
    
    // LM35: 10mV por °C, então °C = tensão * 100
    float temperature = voltage * 100.0;
    
    sum += temperature;
    totalReadings++;

  }
  
  // Retorna média ou 0 se não houver leituras válidas
  return (totalReadings > 0) ? (sum / totalReadings) : 0.0;
}

// ========== FILTRO DE MÉDIA MÓVEL ==========
float movingAverageFilter(float newSample) {
  // Adiciona nova amostra ao array
  samples[sampleIndex] = newSample;
  
  // Atualiza índice circular
  sampleIndex = (sampleIndex + 1) % NUM_SAMPLES;
  
  // Calcula média das amostras
  float sum = 0.0;
  int validSamples = 0;
  
  for (int i = 0; i < NUM_SAMPLES; i++) {
    if (samples[i] > -50 && samples[i] < 150) {  // Filtro de valores plausíveis
      sum += samples[i];
      validSamples++;
    }
  }
  
  // Retorna média ou nova amostra se não houver amostras válidas
  return (validSamples > 0) ? (sum / validSamples) : newSample;
}

// ========== EXIBIÇÃO DOS RESULTADOS ==========
void displayTemperature(float rawTemp, float filteredTemp) {
  
  sampleCount++;
  
  // Calcula tempo decorrido
  float elapsedTime = (millis() - startTime) / 1000.0;
  
  // Formata saída
  //Serial.print("Amostra #");
  Serial.print(sampleCount);
  Serial.print(",");
  //Serial.print(" | PWM: ");
  Serial.print(pwmValue);
  Serial.print(",");
  //Serial.print(" | Temp Bruta: ");
  //Serial.print(rawTemp, 1);
  //Serial.print(",");
  //Serial.print("°C | Temp Filtrada: ");
  Serial.print(filteredTemp, 1);
  Serial.print(",");
  //Serial.print("°C | Tempo: ");
  Serial.println(elapsedTime, 1);
  //Serial.println("s");
  contador += 1;
}

// ========== CONTROLE PWM VIA SERIAL ==========
void checkSerialForPWM() {
  if (Serial.available() > 0) {
    String input = Serial.readStringUntil('\n');
    input.trim();
    
    // Tenta converter para número
    int novoValor = input.toInt();
    
    // Verifica se é um número válido (0-255)
    if (input.length() > 0 && novoValor >= 0 && novoValor <= 255) {
      pwmValue = novoValor;
      analogWrite(PWM_PIN, pwmValue);
    }
  }
}

// ========== FUNÇÃO AUXILIAR: GERAR NOVO VALOR PWM SEM REPETIÇÃO ==========
void gerarNovoValorPWM() {
  // Se todos os valores já foram usados, reinicia
  if (totalValoresDisponiveis == 0) {
    inicializarValoresAleatorios();
  }
  
  // Seleciona um índice aleatório dos valores disponíveis
  int indiceAleatorio = random(0, totalValoresDisponiveis);
  int novoValor = valoresDisponiveis[indiceAleatorio];
  
  // Remove o valor selecionado do array de disponíveis
  for (int i = indiceAleatorio; i < totalValoresDisponiveis - 1; i++) {
    valoresDisponiveis[i] = valoresDisponiveis[i + 1];
  }
  totalValoresDisponiveis--;
  
  // Marca o valor como usado
  valoresUsados[novoValor] = true;
  
  // Atualiza o PWM
  pwmValue = novoValor;
  analogWrite(PWM_PIN, pwmValue);
}

// ========== INICIALIZA SISTEMA DE VALORES ALEATÓRIOS ==========
void inicializarValoresAleatorios() {
  // Limpa array de valores usados
  for (int i = 0; i < 256; i++) {
    valoresUsados[i] = false;
  }
  
  // Preenche array de valores disponíveis (0-255)
  totalValoresDisponiveis = 0;
  for (int i = 0; i < 256; i++) {
    valoresDisponiveis[totalValoresDisponiveis] = i;
    totalValoresDisponiveis++;
  }

}
