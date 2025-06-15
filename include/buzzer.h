#ifndef BUZZER_H
#define BUZZER_H

#include <stdint.h>
#include <stddef.h>
#include "pico/types.h"

#define BUZZER_A 21                       // Pino do primeiro buzzer
#define BUZZER_B 10                       // Pino do segundo buzzer

#define BUZZER_FREQ 2000 // Hz

/**
 * Inicializa o PWM para um buzzer em um pino específico.
 * 
 * @param pin      Pino GPIO conectado ao buzzer
 * @param freq_hz  Frequência do PWM em Hz (deve ser igual à taxa de amostragem do áudio)
 * 
 * Configura o pino para função PWM, define a frequência desejada e inicializa o PWM desligado.
 */
void pwm_init_buzzer(uint pin, uint32_t freq_hz);

#endif