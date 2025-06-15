// Controle do LED RGB da BitDogLab
#include "include/led.h"
#include "hardware/gpio.h"
#include "pico/stdlib.h"

/**
 * Inicializa os pinos do LED RGB como saída.
 * 
 * Configura os pinos definidos em led.h para uso como saída digital,
 * permitindo o controle individual das cores vermelho, verde e azul.
 */
void setup_led_rgb() 
{
    gpio_init(LED_R);                // Inicializa pino do LED vermelho
    gpio_init(LED_G);                // Inicializa pino do LED verde
    gpio_init(LED_B);                // Inicializa pino do LED azul
    gpio_set_dir(LED_R, GPIO_OUT);   // Define pino vermelho como saída
    gpio_set_dir(LED_G, GPIO_OUT);   // Define pino verde como saída
    gpio_set_dir(LED_B, GPIO_OUT);   // Define pino azul como saída
}

/**
 * Define a cor do LED RGB.
 * 
 * @param r Estado do LED vermelho (true = ligado, false = desligado)
 * @param g Estado do LED verde   (true = ligado, false = desligado)
 * @param b Estado do LED azul    (true = ligado, false = desligado)
 * 
 * Acende ou apaga cada cor do LED RGB conforme os parâmetros.
 */
void set_led_color(bool r, bool g, bool b) 
{
    gpio_put(LED_R, r);  // Controla LED vermelho
    gpio_put(LED_G, g);  // Controla LED verde
    gpio_put(LED_B, b);  // Controla LED azul
}