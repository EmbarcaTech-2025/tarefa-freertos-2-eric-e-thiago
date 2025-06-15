// Funções de controle e reprodução de áudio nos buzzers via PWM
#include <stdio.h>
#include "include/buzzer.h"
#include "hardware/pwm.h"
#include "hardware/clocks.h"
#include "pico/stdlib.h"
#include "pico/types.h"

/**
 * Inicializa o PWM para um buzzer em um pino específico.
 * 
 * @param pin      Pino GPIO conectado ao buzzer
 * @param freq_hz  Frequência do PWM em Hz (deve ser igual à taxa de amostragem do áudio)
 * 
 * Configura o pino para função PWM, define a frequência desejada e inicializa o PWM desligado.
 */
void pwm_init_buzzer(uint pin, uint32_t freq_hz) 
{
    gpio_set_function(pin, GPIO_FUNC_PWM);                  // Configura o pino como saída PWM
    uint slice_num = pwm_gpio_to_slice_num(pin);            // Obtém o slice PWM correspondente ao pino

    uint32_t clock = clock_get_hz(clk_sys);                 // Obtém a frequência do clock do sistema
    uint32_t top = clock / freq_hz - 1;                     // Calcula o valor de "top (wrap)" para a frequência desejada
    pwm_config config = pwm_get_default_config();            // Obtém a configuração padrão do PWM
    pwm_config_set_wrap(&config, top);                      // Define o valor de "top" (resolução do PWM)
    pwm_init(slice_num, &config, true);                     // Inicializa o PWM com a configuração definida
    pwm_set_gpio_level(pin, 0);                             // Garante que o PWM inicia desligado
}