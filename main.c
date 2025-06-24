/*
 * Autor: Eric Senne Roma e Thiago Azevedo
 * Data de Criação: 24/06/2025
 * Descrição: Código principal do projeto de monitoramento de temperatura usando FreeRTOS no Raspberry Pi Pico.
 *            O sistema lê a temperatura interna do microcontrolador, exibe no display OLED, indica o status com LED RGB,
 *            aciona um buzzer em caso de temperatura alta e permite silenciar o alarme via botão.
 *            Utiliza multitarefa com FreeRTOS, filas para comunicação entre tarefas e semáforo para controle do alarme.
 */

// Inclusão das bibliotecas do FreeRTOS, periféricos do Pico e módulos do projeto
#include "FreeRTOS.h"
#include "task.h"
#include <stdio.h>
#include "pico/stdlib.h"
#include "queue.h"
#include "semphr.h"
#include "hardware/adc.h"      // ADC para leitura da temperatura
#include "hardware/pwm.h"      // PWM para controle do buzzer
#include "include/display.h"   // Funções do display OLED
#include "inc/ssd1306.h"       // Driver do display SSD1306
#include "include/led.h"       // Controle do LED RGB
#include "include/buzzer.h"    // Controle do buzzer
#include "include/button.h"    // Controle do botão

#define ADC_TEMPERATURE_CHANNEL 4   // Canal do ADC para sensor de temperatura interna
#define N_SAMPLES 20                // Número de amostras para média móvel
// #define TEMP_ALTA                // Descomente para simular temperatura alta
// #define TEMP_MEDIA               // Descomente para simular temperatura média

extern volatile bool button_a_pressed; // Flag global do botão A (externa, definida em outro arquivo)

QueueHandle_t xFilaTemperatura;        // Fila para enviar temperatura para o display
SemaphoreHandle_t xAlarmeSemaforo;     // Semáforo para controle do alarme/buzzer

uint8_t display_buffer[DISPLAY_WIDTH * DISPLAY_HEIGHT / 8]; // Buffer do display OLED

volatile float temperatura_global = 0.0f; // Variável global para temperatura média

/**
 * Converte valor lido do ADC para temperatura em graus Celsius.
 * Fórmula baseada no datasheet do RP2040.
 */
float adc_to_temperature(uint16_t adc_value)
{
    const float conversion_factor = 3.3f / (1 << 12);
    float voltage = adc_value * conversion_factor;
    float temperature = 27.0f - (voltage - 0.706f) / 0.001721f;
    return temperature;
}

/**
 * Tarefa responsável por ler a temperatura do sensor interno, calcular a média e enviar para a fila.
 */
void task_read_temp(void *params)
{
    float temperaturas[N_SAMPLES] = {0};

    while (1)
    {
        float soma = 0.0f;
        for (int i = 0; i < N_SAMPLES; i++)
        {
            uint16_t adc_value = adc_read();
            float temperatura = adc_to_temperature(adc_value);
#ifdef TEMP_ALTA
            temperatura += 15.0f; // Simula temperatura alta
#endif
#ifdef TEMP_MEDIA
            temperatura += 5.0f;  // Simula temperatura média
#endif
            temperaturas[i] = temperatura;
            soma += temperatura;
            vTaskDelay(pdMS_TO_TICKS(10)); // Aguarda 10 ms entre amostras
        }
        float media = soma / N_SAMPLES;

        printf("Temperatura média (%d amostras): %.2f °C\n", N_SAMPLES, media);

        temperatura_global = media; // Atualiza variável global

        xQueueSend(xFilaTemperatura, &media, portMAX_DELAY); // Envia para o display

        vTaskDelay(pdMS_TO_TICKS(1000)); // Aguarda 1 segundo para próxima média
    }
}

/**
 * Tarefa responsável por receber a temperatura da fila e exibir no display OLED.
 */
void task_display_oled(void *params)
{
    float temperatura;
    while (1)
    {
        if (xQueueReceive(xFilaTemperatura, &temperatura, portMAX_DELAY))
        {
            display_temp(display_buffer, temperatura);
            vTaskDelay(pdMS_TO_TICKS(1000)); // Atualiza display a cada 1 segundo
        }
    }
}

/**
 * Tarefa responsável por indicar o status da temperatura usando o LED RGB.
 * Verde: temperatura baixa, Amarelo: média, Vermelho: alta.
 */
void task_led_status(void *params)
{
    while (1)
    {
        float temperatura = temperatura_global;
        if (temperatura < 25.0f)
            set_led_color(false, true, false); // Verde
        else if (temperatura < 35.0f)
            set_led_color(true, true, false);  // Amarelo
        else
            set_led_color(true, false, false); // Vermelho
        vTaskDelay(pdMS_TO_TICKS(500)); // Atualiza LED a cada 0,5 segundo
    }
}

/**
 * Tarefa responsável por detectar o pressionamento do botão e liberar o semáforo do alarme.
 */
void task_botao(void *params)
{
    while (1)
    {
        if (button_a_pressed)
        {
            button_a_pressed = false; // Limpa o flag
            xSemaphoreGive(xAlarmeSemaforo); // Libera o semáforo para silenciar o buzzer
            vTaskDelay(pdMS_TO_TICKS(300));  // Debounce do botão
        }
        vTaskDelay(pdMS_TO_TICKS(50)); // Polling do botão
    }
}

/**
 * Tarefa responsável por acionar o buzzer quando a temperatura está alta.
 * O buzzer só para quando o botão for pressionado.
 */
void task_buzzer_alert(void *params)
{
    pwm_init_buzzer(BUZZER_A, BUZZER_FREQ);

    bool alarme_ativo = false;

    while (1)
    {
        if (temperatura_global > 35.0f && !alarme_ativo)
        {
            alarme_ativo = true;
            // Buzzer apita continuamente até o botão ser pressionado
            pwm_set_gpio_level(BUZZER_A, 128);
            xSemaphoreTake(xAlarmeSemaforo, portMAX_DELAY);
            pwm_set_gpio_level(BUZZER_A, 0); // Desliga o buzzer
        }
        // Reseta o alarme se a temperatura voltar ao normal
        if (temperatura_global <= 35.0f)
        {
            alarme_ativo = false;
            pwm_set_gpio_level(BUZZER_A, 0); // Garante que está desligado
        }
        vTaskDelay(pdMS_TO_TICKS(200)); // Aguarda 200 ms
    }
}

/**
 * Função principal: inicializa periféricos, cria tarefas e inicia o scheduler do FreeRTOS.
 */
int main()
{
    stdio_init_all(); // Inicializa UART/USB para debug

    adc_init();
    adc_set_temp_sensor_enabled(true);
    adc_select_input(ADC_TEMPERATURE_CHANNEL);

    setup_i2c();
    ssd1306_init();

    setup_buttons();

    setup_led_rgb();

    xAlarmeSemaforo = xSemaphoreCreateBinary(); // Cria semáforo binário

    xFilaTemperatura = xQueueCreate(20, sizeof(float)); // Cria fila para temperaturas

    // Criação das tarefas do FreeRTOS
    xTaskCreate(task_read_temp, "Armazenar Temperatura", 256, NULL, 1, NULL);
    xTaskCreate(task_display_oled, "Display OLED", 256, NULL, 1, NULL);
    xTaskCreate(task_led_status, "LED Status", 256, NULL, 1, NULL);
    xTaskCreate(task_buzzer_alert, "Buzzer Alerta", 256, NULL, 3, NULL);
    xTaskCreate(task_botao, "Botao", 256, NULL, 4, NULL);

    vTaskStartScheduler(); // Inicia o escalonador do FreeRTOS

    while (1); // Loop infinito caso o scheduler retorne (não deve acontecer)
}