#include "FreeRTOS.h"
#include "task.h" 
#include <stdio.h>                  
#include "pico/stdlib.h"                              
#include "queue.h"                  
#include "hardware/adc.h"           
#include "include/display.h"        
#include "inc/ssd1306.h"            
#include "include/led.h"        

#define ADC_TEMPERATURE_CHANNEL 4   
#define N_SAMPLES 20  
//#define TEMP_ALTA
//#define TEMP_MEDIA             

QueueHandle_t xFilaTemperatura;     

uint8_t display_buffer[DISPLAY_WIDTH * DISPLAY_HEIGHT / 8]; 

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

        xQueueSend(xFilaTemperatura, &media, portMAX_DELAY); 
        
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

// Tarefa para indicar status da temperatura com LED RGB
void task_led_status(void *params)
{
    float temperatura;
    while (1)
    {
        if (xQueueReceive(xFilaTemperatura, &temperatura, portMAX_DELAY))
        {
            if (temperatura < 25.0f) // NORMAL
            {
                set_led_color(false, true, false); // Verde
            }
            else if (temperatura < 35.0f) // ALERTA
            {
                set_led_color(true, true, false); // Amarelo (Vermelho + Verde)
            }
            else // CRÍTICO
            {
                set_led_color(true, false, false); // Vermelho
            }
            vTaskDelay(pdMS_TO_TICKS(500)); 
        }
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

    setup_led_rgb(); // Inicializa o LED RGB

    xFilaTemperatura = xQueueCreate(5, sizeof(float));

    xTaskCreate(task_read_temp, "Armazenar Temperatura", 256, NULL, 1, NULL);
    xTaskCreate(task_display_oled, "Display OLED", 512, NULL, 1, NULL);
    xTaskCreate(task_led_status, "LED Status", 256, NULL, 1, NULL); // Cria a tarefa do LED

    vTaskStartScheduler();          

    while (1);                      
}