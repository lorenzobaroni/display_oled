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
#include "inc/ssd1306.h" // Biblioteca para controlar o display ssd1306

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
volatile bool debounceActive = false;  // Estado do debounce
const unsigned long debounceDelay = 200; // Tempo de debounce

PIO pio = pio0;
uint sm;
uint offset;

ssd1306_t ssd;
char ultimo_char = ' '; // Variável para armazenar o último caractere digitado

// Filtrar entrada do teclado
bool eh_caractere_valido(char c) {
    return (c >= '0' && c <= '9') || (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z');
}

// Inicialização do display OLED
void inicializar_display() {
    ssd1306_init(&ssd, WIDTH, HEIGHT, false, endereco, I2C_PORT); 

    // Valida se a configuração do display foi bem-sucedida
    if (ssd.width == 0 || ssd.height == 0) {
        printf("Erro ao inicializar display OLED\n");
        return;
    }

    ssd1306_config(&ssd);
    ssd1306_send_data(&ssd);
    ssd1306_fill(&ssd, false);
    ssd1306_send_data(&ssd);
}


void atualizar_display() {
    ssd1306_fill(&ssd, false); // Limpa a tela

    // Exibir o caractere digitado em tamanho maior
    char texto_char[2] = {ultimo_char, '\0'}; 
    ssd1306_draw_large_char(&ssd, texto_char[0], 50, 10); // Exibe grande no topo

    // Exibir estado dos LEDs abaixo do caractere
    if (ledGreenState && ledBlueState) {
        ssd1306_draw_string(&ssd, "Ambos LEDs", 20, 40);
        ssd1306_draw_string(&ssd, "Ligados", 30, 50);
    } 
    else if (ledGreenState) {
        ssd1306_draw_string(&ssd, "LED Verde", 30, 40);
        ssd1306_draw_string(&ssd, "Ligado", 40, 50);
    } 
    else if (ledBlueState) {
        ssd1306_draw_string(&ssd, "LED Azul", 30, 40);
        ssd1306_draw_string(&ssd, "Ligado", 40, 50);
    } 
    else {
        ssd1306_draw_string(&ssd, "Nenhum LED", 20, 40);
        ssd1306_draw_string(&ssd, "Ligado", 40, 50);
    }

    ssd1306_send_data(&ssd); // Atualiza o display
}

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
    static uint8_t last_number = 255; // Número armazenado para evitar redibug
    if (num > 9 || num == last_number) return;

    last_number = num;
    for (int i = 0; i < NUM_LEDS; i++) {
        uint32_t color = formatos_numeros[num][i] ? 0x00FF00 : 0x000000;
        pio_sm_put_blocking(pio, sm, color << 8);
    }
}

// Debounce por temporizador
int64_t debounce_timer_callback(alarm_id_t id, void *user_data) {
    debounceActive = false;  // Libera o botão para ser pressionado novamente
    return 0;
}


// Função de interrupção para ambos os botões
void gpio_callback(uint gpio, uint32_t events) {
    if (debounceActive) return;
    debounceActive = true;

    add_alarm_in_ms(debounceDelay, debounce_timer_callback, NULL, false);

    if (gpio == BUTTON_A) {
        if (gpio_get(BUTTON_A) == 0) { // Verifica se o botão realmente está pressionado
            ledGreenState = !ledGreenState;
            gpio_put(LED_GREEN, ledGreenState);
            printf("Botão A pressionado. LED Verde %s\n", ledGreenState ? "Ligado" : "Desligado");
        }
    } 
    else if (gpio == BUTTON_B) {
        if (gpio_get(BUTTON_B) == 0) { // Verifica se o botão realmente está pressionado
            ledBlueState = !ledBlueState;
            gpio_put(LED_BLUE, ledBlueState);
            printf("Botão B pressionado. LED Azul %s\n", ledBlueState ? "Ligado" : "Desligado");
        }
    }

    atualizar_display(); 
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
    ssd1306_init(&ssd, WIDTH, HEIGHT, false, endereco, I2C_PORT);
    ssd1306_config(&ssd);
    ssd1306_send_data(&ssd);
    ssd1306_fill(&ssd, false);
    ssd1306_send_data(&ssd);

    // Configuração correta da interrupção para múltiplos botões
    gpio_set_irq_enabled_with_callback(BUTTON_A, GPIO_IRQ_EDGE_FALL, true, &gpio_callback);
    gpio_set_irq_enabled_with_callback(BUTTON_B, GPIO_IRQ_EDGE_FALL, true, &gpio_callback);

    init_matrix(); // Inicializa a Matriz WS2812

    while (true) {
        char received_char = ' ';
        if (scanf("%c", &received_char) == 1 && eh_caractere_valido(received_char)) {
            ultimo_char = received_char;
            atualizar_display();
            if (received_char >= '0' && received_char <= '9') {
                show_number(received_char - '0');
            }
        }
        sleep_ms(100);
    }
    return 0;
}
