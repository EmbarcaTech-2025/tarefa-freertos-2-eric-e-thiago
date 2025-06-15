// Definições e protótipos para controle do LED RGB da BitDogLab
#ifndef LED_H
#define LED_H

#include <stdbool.h>

// Definições dos pinos GPIO conectados aos LEDs RGB
#define LED_R 13   // Pino do LED vermelho
#define LED_G 11   // Pino do LED verde
#define LED_B 12   // Pino do LED azul

/**
 * Inicializa os pinos do LED RGB como saída.
 * Deve ser chamada antes de usar o LED.
 */
void setup_led_rgb(void);

/**
 * Define a cor do LED RGB.
 * 
 * @param r Estado do LED vermelho (true = ligado, false = desligado)
 * @param g Estado do LED verde   (true = ligado, false = desligado)
 * @param b Estado do LED azul    (true = ligado, false = desligado)
 */
void set_led_color(bool r, bool g, bool b);

#endif