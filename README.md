# Display OLED com Raspberry Pi Pico W

## 📌 Descrição
Este projeto implementa um sistema utilizando um **display OLED SSD1306**, **botões físicos**, **LEDs RGB** e uma **matriz de LEDs WS2812** em uma Raspberry Pi Pico W. O código controla a exibição de caracteres no display, altera estados dos LEDs e apresenta números na matriz de LEDs WS2812.

## 🔗 Link do Vídeo Ensaio

[Demonstração de Funcionamento do Projeto](https://www.youtube.com/watch?v=rPqh2b_V23Y&ab_channel=LorenzoGiuseppeOliveiraBaroni)

## 📌 Funcionalidades
- Exibir um caractere digitado no **display OLED SSD1306**.
- Controlar o estado dos **LEDs RGB** com botões físicos.
- Exibir números de 0 a 9 na **matriz de LEDs WS2812**.
- Implementação de **debounce** para evitar acionamentos repetidos dos botões.
- Comunicação via **I2C** para o display OLED e **PIO** para a matriz WS2812.

## 🛠 Hardware Utilizado
- **Raspberry Pi Pico W**
- **Display OLED SSD1306** (I2C)
- **Matriz de LEDs WS2812**
- **LEDs RGB**
- **Botões físicos**
- **Resistores pull-up** para os botões

## 📌 Conexões dos Componentes

| Componente       | Pino Pico W |
|-----------------|-------------|
| Matriz WS2812   | GPIO 7      |
| LED Vermelho    | GPIO 13     |
| LED Verde      | GPIO 11     |
| LED Azul      | GPIO 12     |
| Botão A        | GPIO 5      |
| Botão B        | GPIO 6      |
| I2C SDA (Display OLED) | GPIO 14 |
| I2C SCL (Display OLED) | GPIO 15 |

## 🔧 Instalação e Configuração
### 1️⃣ Configurar o ambiente
Antes de compilar e rodar o código, certifique-se de ter:
- **SDK do Raspberry Pi Pico** corretamente instalado.
- **Compilador GCC ARM** para processadores Cortex M0+.
- **CMake e Ninja** para build do projeto.

### 2️⃣ Clonar o repositório
```sh
git clone https://github.com/seu-usuario/display_oled.git
cd display_oled
```

### 3️⃣ Compilar o projeto

### 4️⃣ Transferir o firmware para a Pico W
- Conecte a **Pico W** ao PC em **modo BOOTSEL**.
- Copie o arquivo `.uf2` gerado para a unidade da Pico.

## 📝 Código Principal (display_oled.c)
O código principal inicializa os periféricos e gerencia a interação com os botões, LEDs e display OLED.

Principais funções:
- `atualizar_display()`: Atualiza a tela OLED com o último caractere e estado dos LEDs.
- `show_number()`: Exibe números na matriz WS2812.
- `gpio_callback()`: Interrupção para os botões, alterando estados dos LEDs.
- `main()`: Configuração dos periféricos e loop principal.

## 📌 Melhorias Futuras
- Adicionar um menu de navegação na tela OLED.
- Criar animações customizadas para a matriz WS2812.

## 📝 Licença
Este programa foi desenvolvido como um exemplo educacional e pode ser usado livremente para fins de estudo e aprendizado.

## 📌 Autor

- LORENZO GIUSEPPE OLIVEIRA BARONI

