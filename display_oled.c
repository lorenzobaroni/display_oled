// Bibliotecas necessárias para Raspberry Pi Pico W
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include "hardware/pio.h"
#include "hardware/gpio.h"
#include "hardware/i2c.h"
#include "pico/stdlib.h"
#include "ws2812.pio.h" // Biblioteca para controlar LEDs WS2812
#include "inc/font.h"  // Biblioteca de fontes personalizada
#include "inc/ssd1306.h"

// Definições de hardware
#define MATRIX_PIN 7
#define NUM_LEDS 25
#define LED_RED 13
#define LED_GREEN 11
#define LED_BLUE 12
#define BUTTON_A 5
#define BUTTON_B 6
#define I2C_PORT i2c1
#define I2C_SDA 14
#define I2C_SCL 15
#define endereco 0x3C

// Variáveis de estado
volatile bool ledGreenState = false;
volatile bool ledBlueState = false;
volatile unsigned long lastInterruptTime = 0;
const unsigned long debounceDelay = 200; // Debounce de 200ms

PIO pio = pio0;
uint sm;
uint offset;

// Números para a matriz de LEDs
const uint32_t formatos_numeros[10][NUM_LEDS] = {
    {1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 0, 0, 0, 1, 1, 0, 0, 0, 1, 1, 1, 1, 1, 1}, // 0
    {0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0}, // 1
    {1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1}, // 2 
    {1, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1}, // 3
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 0, 0, 0, 1}, // 4
    {1, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 1}, // 5
    {1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 1}, // 6
    {0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 1, 1, 1, 1, 1}, // 7
    {1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 1, 1, 1, 1}, // 8
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 1, 1, 1, 1}  // 9
};

// Inicializa a Matriz WS2812
void init_matrix() {
    offset = pio_add_program(pio, &ws2812_program);
    sm = pio_claim_unused_sm(pio, true);
    ws2812_program_init(pio, sm, offset, MATRIX_PIN, 800000, false);
}

// Exibe um número na Matriz WS2812
void show_number(uint8_t num) {
    if (num > 9) return;
    for (int i = 0; i < NUM_LEDS; i++) {
        uint32_t color = formatos_numeros[num][i] ? 0x00FF00 : 0x000000; // Verde para ativo, preto para inativo
        pio_sm_put_blocking(pio, sm, color << 8);
    }
}

// Função genérica de interrupção para ambos os botões
void gpio_callback(uint gpio, uint32_t events) {
    unsigned long currentTime = to_ms_since_boot(get_absolute_time());
    if (currentTime - lastInterruptTime < debounceDelay) {
        return;  // Ignorar interrupção se for muito rápida
    }
    lastInterruptTime = currentTime;

    if (gpio == BUTTON_A) {
        ledGreenState = !ledGreenState;
        gpio_put(LED_GREEN, ledGreenState);
        printf("Botão A pressionado. LED Verde %s\n", ledGreenState ? "Ligado" : "Desligado");
    } else if (gpio == BUTTON_B) {
        ledBlueState = !ledBlueState;
        gpio_put(LED_BLUE, ledBlueState);
        printf("Botão B pressionado. LED Azul %s\n", ledBlueState ? "Ligado" : "Desligado");
    }
}



int main() {
    
    stdio_init_all();

    // Inicializa I2C
    i2c_init(I2C_PORT, 400 * 1000);
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);

    gpio_init(LED_GREEN);
    gpio_set_dir(LED_GREEN, GPIO_OUT);
    gpio_put(LED_GREEN, 0);
    
    gpio_init(LED_BLUE);
    gpio_set_dir(LED_BLUE, GPIO_OUT);
    gpio_put(LED_BLUE, 0);
    
    gpio_init(BUTTON_A);
    gpio_set_dir(BUTTON_A, GPIO_IN);
    gpio_pull_up(BUTTON_A);
    gpio_set_irq_enabled(BUTTON_A, GPIO_IRQ_EDGE_FALL, true);

    gpio_init(BUTTON_B);
    gpio_set_dir(BUTTON_B, GPIO_IN);
    gpio_pull_up(BUTTON_B);
    gpio_set_irq_enabled(BUTTON_B, GPIO_IRQ_EDGE_FALL, true);

    // Inicializa o display SSD1306
    ssd1306_t ssd;
    ssd1306_init(&ssd, WIDTH, HEIGHT, false, endereco, I2C_PORT);
    ssd1306_config(&ssd);
    ssd1306_send_data(&ssd);

    ssd1306_fill(&ssd, false);
    ssd1306_send_data(&ssd);

    // Configuração correta da interrupção para múltiplos botões
    gpio_set_irq_enabled_with_callback(BUTTON_A, GPIO_IRQ_EDGE_FALL, true, &gpio_callback);
    gpio_set_irq_enabled_with_callback(BUTTON_B, GPIO_IRQ_EDGE_FALL, true, &gpio_callback);

    init_matrix(); // Inicializa a Matriz WS2812

    char received_char = ' ';

     while (true) {
        if (stdio_usb_connected()) {
            received_char = getchar(); // Lê o caractere enviado pelo Serial Monitor

            printf("Recebido: %c\n", received_char); // Debug no Serial Monitor

            // Atualiza o display com o caractere recebido
            ssd1306_fill(&ssd, false);
            ssd1306_draw_char(&ssd, received_char, 50, 30);
            ssd1306_send_data(&ssd);
            
            if (received_char >= '0' && received_char <= '9') {
                show_number(received_char - '0');
            }
        }
        sleep_ms(100);
    }
    return 0;
}
