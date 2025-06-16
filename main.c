#include "FreeRTOS.h"
#include "task.h"
#include <stdio.h>
#include "pico/stdlib.h"
#include "queue.h"
#include "semphr.h"
#include "hardware/adc.h"
#include "hardware/pwm.h"
#include "include/display.h"
#include "inc/ssd1306.h"
#include "include/led.h"
#include "include/buzzer.h"
#include "include/button.h"

#define ADC_TEMPERATURE_CHANNEL 4
#define N_SAMPLES 20
//#define TEMP_ALTA

extern volatile bool button_a_pressed;

QueueHandle_t xFilaTemperatura;
SemaphoreHandle_t xAlarmeSemaforo;

uint8_t display_buffer[DISPLAY_WIDTH * DISPLAY_HEIGHT / 8];

volatile float temperatura_global = 0.0f; // Variável global para temperatura

float adc_to_temperature(uint16_t adc_value)
{
    const float conversion_factor = 3.3f / (1 << 12);
    float voltage = adc_value * conversion_factor;
    float temperature = 27.0f - (voltage - 0.706f) / 0.001721f;
    return temperature;
}

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
            temperatura += 15.0f;
#endif
#ifdef TEMP_MEDIA
            temperatura += 5.0f;
#endif
            temperaturas[i] = temperatura;
            soma += temperatura;
            vTaskDelay(pdMS_TO_TICKS(10));
        }
        float media = soma / N_SAMPLES;

        printf("Temperatura média (%d amostras): %.2f °C\n", N_SAMPLES, media);

        temperatura_global = media; // Atualiza variável global

        xQueueSend(xFilaTemperatura, &media, portMAX_DELAY); // Só para o display

        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

void task_display_oled(void *params)
{
    float temperatura;
    while (1)
    {
        if (xQueueReceive(xFilaTemperatura, &temperatura, portMAX_DELAY))
        {
            display_temp(display_buffer, temperatura);
            vTaskDelay(pdMS_TO_TICKS(1000));
        }
    }
}

void task_led_status(void *params)
{
    while (1)
    {
        float temperatura = temperatura_global;
        if (temperatura < 25.0f)
            set_led_color(false, true, false); // Verde
        else if (temperatura < 35.0f)
            set_led_color(true, true, false); // Amarelo
        else
            set_led_color(true, false, false); // Vermelho
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}

void task_botao(void *params)
{
    while (1)
    {
        if (button_a_pressed)
        {
            button_a_pressed = false; // Limpa o flag
            xSemaphoreGive(xAlarmeSemaforo); 
            vTaskDelay(pdMS_TO_TICKS(300)); 
        }
        vTaskDelay(pdMS_TO_TICKS(50)); 
    }
}

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
        vTaskDelay(pdMS_TO_TICKS(200));
    }
}

int main()
{
    stdio_init_all();

    adc_init();
    adc_set_temp_sensor_enabled(true);
    adc_select_input(ADC_TEMPERATURE_CHANNEL);

    setup_i2c();
    ssd1306_init();

    setup_buttons();

    setup_led_rgb();

    xAlarmeSemaforo = xSemaphoreCreateBinary();

    xFilaTemperatura = xQueueCreate(20, sizeof(float));

    xTaskCreate(task_read_temp, "Armazenar Temperatura", 256, NULL, 1, NULL);
    xTaskCreate(task_display_oled, "Display OLED", 256, NULL, 1, NULL);
    xTaskCreate(task_led_status, "LED Status", 256, NULL, 1, NULL);
    xTaskCreate(task_buzzer_alert, "Buzzer Alerta", 256, NULL, 3, NULL);
    xTaskCreate(task_botao, "Botao", 256, NULL, 4, NULL);

    vTaskStartScheduler();

    while (1);
}