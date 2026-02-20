/*
 * Medição de temperatura com LM35 + Controle PWM
 * Arduino Mega 2560
 * Taxa de amostragem: 100ms
 * Conexão: LM35 VCC -> 5V, OUT -> A0, GND -> GND
 */


// =====================================================
// ================= PROJETO DE CONTROLE ===============
// =====================================================

const float ganho_xk = -4771;
const float ganhi_xk1 = 4383;
const float ganho_u = 1;
const float ganho_uk1 = 0.9916;

long x_anterior = 0; // erro anterior
long entrada_anterior = 0;

float SETPOINT = 40.0;   // Temperatura alvo em °C
double x;
double u;

// =====================================================
// ======================= END =========================
// =====================================================


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
unsigned long sampleCount = 0;

// ========== SETUP ==========
void setup() {
  Serial.begin(115200);           
  while (!Serial) {               
    ; 
  }

  startTime = millis();           
  
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
  
  if (currentMillis - previousMillis >= SAMPLE_RATE_MS) {
    previousMillis = currentMillis;
    
    // 1. Ler temperatura
    float rawTemperature = readTemperatureLM35();
    float filteredTemperature = movingAverageFilter(rawTemperature);
    
    /*
    const float ganho_xk = -4771;
    const float ganhi_xk1 = 4383;
    const float ganho_u = 1;
    const float ganho_uk1 = 0.9916;
    */

    // 2. Calcular erro (entrada do sistema)
    x = (SETPOINT - filteredTemperature);
    x_anterior = x; // erro anterior
    u = ((ganho_xk)*x+(ganhi_xk1)*x_anterior+(ganho_uk1)*entrada_anterior) + 127;
  

    // 6. Converter saída para PWM
    pwmValue = constrain((int)u, 0, 255);
    analogWrite(PWM_PIN, pwmValue);
    
    entrada_anterior = pwmValue;
    
    // 7. Exibir resultados
    displayTemperature(rawTemperature, filteredTemperature);
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
  //Serial.print(pwmValue);
  //Serial.print(",");
  //Serial.print(" | Temp Bruta: ");
  //Serial.print(rawTemp, 1);
  //Serial.print(",");
  //Serial.print("°C | Temp Filtrada: ");
  //Serial.print(x);
  //Serial.print(",");
  Serial.print(SETPOINT);
  Serial.print(",");
  Serial.println(filteredTemp, 1);
  //Serial.print(",");
  //Serial.print("°C | Tempo: ");
  //Serial.println(elapsedTime, 1);
  //Serial.println("s");
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
      SETPOINT = novoValor;
    }
  }
}