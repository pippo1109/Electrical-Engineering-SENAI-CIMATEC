#include <Arduino.h>
#include <pippo.h>

/**
 * @file main.cpp
 * @brief multimetro
 * @author Felippo e Raul
 * @version 1.0.0

 */

const int analog_pin_adc1 = A2; // Pino analógico do Arduino conectado ao Vout do divisor de tensão.

unsigned int i = 0;
unsigned int a = 0;

constexpr int size = 256; // Tamanho do array de amostras
double amostras[size]; // Array para armazenar as amostras coletadas

unsigned long ultimo_us = 0;
const unsigned long Ts_us = 16; // ~6kHz

void setup() {
  Serial.begin(1024000);
  pinMode(analog_pin_adc1, INPUT);
}

void loop() {

  double sinal = analogRead(analog_pin_adc1)*(5.0/1023.0); // Lê o valor do pino analógico e converte para tensão (0-5V)
  double sinal_dc_off = sinal - 0; // Remove o offset DC (assumindo que o sinal está centrado em 2.5V)
  unsigned long agora = micros();

  if (agora - ultimo_us >= Ts_us) {
    amostras[i] = sinal_dc_off;
    a = i;

    ultimo_us = agora;
    i++;
    if (i == size) {
      i = 0;
    }
  }

  double rms = calcularRMS(amostras, size); // Chama a função RMS para processar o sinal do pino analógico
  double media_valor = media(amostras, size); // Chama a função média para processar o sinal do pino analógico
  Serial.println(rms);
  //Serial.print(",");
  //Serial.print(media_valor);
  //Serial.print(",");
  //Serial.println(amostras[a]);
 
  
}