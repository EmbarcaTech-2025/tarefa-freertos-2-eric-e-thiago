
# Tarefa: Roteiro de FreeRTOS #2 - EmbarcaTech 2025

Autor: Eric Roma e Thiago Young de Azevedo

Curso: Residência Tecnológica em Sistemas Embarcados

Instituição: EmbarcaTech - HBr

Campinas, Junho de 2025

---

# 🌡️ Monitor de Temperatura com Alarme Usando FreeRTOS na BitDogLab

## 📌 Proposta da Atividade

Desenvolver uma aplicação embarcada na placa **BitDogLab**, utilizando **FreeRTOS** e ao menos um periférico da placa. O projeto deve demonstrar o uso de multitarefa, comunicação entre tarefas e recursos de sincronização do sistema operacional em tempo real.

---

## 🎯 Objetivos Principais

- Utilizar o sistema operacional FreeRTOS para criar tarefas concorrentes.
- Aplicar conceitos de comunicação entre tarefas com filas e semáforos.
- Interagir com múltiplos periféricos embarcados da BitDogLab.
- Detectar temperatura elevada e emitir alertas visuais e sonoros.
- Permitir que o usuário desligue o alarme com um botão físico.

---

## ⚙️ Funcionamento Resumido

O sistema realiza a leitura contínua da temperatura interna do microcontrolador (canal ADC 4), calcula a média de 20 amostras e:

1. Exibe a temperatura no display OLED.
2. Controla a cor do LED RGB conforme faixas de temperatura:
   - Verde: temperatura < 25 °C
   - Amarelo: 25 °C ≤ temperatura < 35 °C
   - Vermelho: temperatura ≥ 35 °C
3. Ativa um alarme sonoro (buzzer) caso a temperatura ultrapasse 35 °C.
4. Permite desligar o alarme pressionando o botão A da BitDogLab.

O sistema é modularizado em cinco tarefas FreeRTOS, cada uma responsável por um comportamento específico. A troca de dados entre elas é feita por fila de temperatura e semáforo de controle de alarme.

---

## 📦 GPIOs Utilizadas

| Componente        | Descrição               | GPIO(s) envolvida(s)     |
|-------------------|-------------------------|---------------------------|
| Sensor de temperatura | Interno ao RP2040 (ADC) | Canal 4 (ADC interno)     |
| Display OLED      | Saída I2C               | GPIO14 (SDA), GPIO15 (SCL) |
| LED RGB           | Sinalização visual      | GPIO11 (LED Verde), GPIO12 (LED Azul), GPIO13 (LED Vermelho) |
| Buzzer            | Alarme sonoro (PWM)     | GPIO21 (Buzzer A) |
| Botão A           | Entrada digital         | GPIO5 |

---

## 📂 Tarefas FreeRTOS Utilizadas

| Tarefa               | Prioridade | Responsabilidade                                  |
|----------------------|------------|---------------------------------------------------|
| `task_read_temp`     | 1          | Lê ADC, calcula temperatura média e envia à fila |
| `task_display_oled`  | 1          | Recebe da fila e atualiza o display OLED         |
| `task_led_status`    | 1          | Altera cor do LED RGB conforme temperatura       |
| `task_buzzer_alert`  | 3          | Ativa buzzer se temperatura > 35 °C              |
| `task_botao`         | 4          | Monitora botão e libera semáforo para buzzer     |

---

## 🔁 Recursos de FreeRTOS Utilizados

- `xTaskCreate`: Criação das 5 tarefas.
- `vTaskDelay`: Controle de tempo em todas as tarefas.
- `xQueueCreate`, `xQueueSend`, `xQueueReceive`: Comunicação de temperatura entre tarefas.
- `xSemaphoreCreateBinary`, `xSemaphoreTake`, `xSemaphoreGive`: Sincronização entre botão e alarme.

---

## 🧪 Demonstração Funcional

O link abaixo é referente a um vídeo demonstrativo, apresentando o funcionamento completo do algoritmo do monitor de temperatura na BitDogLab usando FreeRTOS. 

Neste vídeo é possível notar:

 - A exibição da temperatura no display OLED.
 - A alteração automática da cor do LED RGB de acordo com a temperatura detectada pelo sensor interno da placa (apesar de terem sido utilizadas constantes de teste para "forçar" uma temperatura média e alta).
 - A ativação do alarme pelo buzzer quando a temperatura chega a mais de 35 °C.
 - O desligamento deste alarme ao pressionar o botão A.

Link: https://youtu.be/LclHyqQy56w

---

## 📚 Bibliotecas Utilizadas

| Biblioteca              | Finalidade                                |
|-------------------------|--------------------------------------------|
| `FreeRTOS.h`, `task.h`  | Gerenciamento de tarefas                   |
| `queue.h`, `semphr.h`   | Comunicação e sincronização entre tarefas |
| `pico/stdlib.h`         | Funções básicas do Pico SDK               |
| `hardware/adc.h`        | Leitura de ADC interno do RP2040          |
| `hardware/pwm.h`        | Controle de PWM (buzzer)                  |
| `inc/ssd1306.h`         | Controle do display OLED                  |
| `include/display.h`     | Funções auxiliares de visualização         |
| `include/led.h`         | Controle do LED RGB                       |
| `include/buzzer.h`      | Inicialização e controle do buzzer        |
| `include/button.h`      | Configuração e leitura do botão           |

---

## 🗂️ Organização do Projeto

- `main.c` – Código principal com criação de tarefas e inicializações.
- `display.h/.c`, `led.h/.c`, `buzzer.h/.c`, `button.h/.c` – Drivers auxiliares organizados por periférico.
- `inc/ssd1306.h` – Biblioteca de controle do display OLED.

---

## ✅ Conclusão

Este projeto demonstrou o uso prático do **FreeRTOS** na plataforma **RP2040/BitDogLab**, com foco em:
- Gerenciamento multitarefa.
- Comunicação segura entre tarefas.
- Interação com periféricos reais.
- Aplicação funcional com resposta a eventos físicos (temperatura e botão).

---

## 📜 Licença
GNU GPL-3.0.
