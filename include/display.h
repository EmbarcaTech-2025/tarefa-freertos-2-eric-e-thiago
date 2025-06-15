#ifndef DISPLAY_H
#define DISPLAY_H

#include <stdint.h>
#include <stddef.h>

// Largura e altura do display OLED SSD1306 em pixels
#define DISPLAY_WIDTH 128
#define DISPLAY_HEIGHT 64

// Pinos GPIO utilizados para o barramento I2C do display
#define I2C_SDA 14   // Pino SDA do I2C
#define I2C_SCL 15   // Pino SCL do I2C

/**
 * Inicializa a interface I2C para comunicação com o display OLED.
 * Deve ser chamada antes de qualquer operação de exibição.
 */
void setup_i2c(void);

/**
 * Exibe a temperatura no display OLED.
 * 
 * @param buffer      Buffer de vídeo do display (array de bytes)
 * @param temperatura Valor da temperatura a ser exibido
 */
void display_temp(uint8_t *buffer, float temperatura);

/**
 * Limpa o display OLED, apagando todo o conteúdo.
 * 
 * @param buffer Buffer de vídeo do display (array de bytes)
 */
void limpar_display(uint8_t *buffer);

#endif