// Funções de controle e visualização para o display OLED SSD1306
#include <stdio.h>
#include <string.h> 
#include "include/display.h"
#include "inc/ssd1306.h"
#include "pico/stdlib.h"

/**
 * Inicializa a interface I2C para comunicação com o display OLED.
 * 
 * Configura o barramento I2C1 com a frequência definida em ssd1306_i2c_clock,
 * além de configurar os pinos SDA e SCL como função I2C e ativar pull-up.
 */
void setup_i2c(void)
{
    i2c_init(i2c1, ssd1306_i2c_clock * 1000);
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);
}


void display_temp(uint8_t *buffer, float temperatura) 
{
    // Limpa o buffer de exibição
    memset(buffer, 0, DISPLAY_WIDTH * DISPLAY_HEIGHT / 8);

    // Escreve o título no topo do display
    ssd1306_draw_string(buffer, 5, 0, "Temperatura"); 

    // Cria uma string temporária para exibir o valor da temperatura
    char temp_str[32];
    snprintf(temp_str, sizeof(temp_str), "Valor: %.2f %cC", temperatura, 0xB0);
    ssd1306_draw_string(buffer, 5, 16, temp_str); 

    // Define qual parte do display será usada (tela toda)
    struct render_area frame_area = 
    {
        .start_column = 0,
        .end_column = ssd1306_width - 1,
        .start_page = 0,
        .end_page = ssd1306_n_pages - 1
    };

    calculate_render_area_buffer_length(&frame_area); // Calcula o tamanho do buffer para renderização
    render_on_display(buffer, &frame_area);           // Atualiza o display OLED com o conteúdo do buffer
}

/**
 * Limpa o display OLED, apagando todo o conteúdo.
 * 
 * @param buffer Buffer de imagem do display (array de bytes)
 * 
 * Preenche o buffer com zeros e atualiza toda a área do display.
 */
void limpar_display(uint8_t *buffer) 
{
    memset(buffer, 0, DISPLAY_WIDTH * DISPLAY_HEIGHT / 8);

    struct render_area frame_area = 
    {
        .start_column = 0,
        .end_column = ssd1306_width - 1,
        .start_page = 0,
        .end_page = ssd1306_n_pages - 1
    };

    calculate_render_area_buffer_length(&frame_area);
    render_on_display(buffer, &frame_area);
}