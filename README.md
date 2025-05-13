# Comunicação iot
Este projeto implementa um Webserver utilizando o microcontrolador Raspberry Pi Pico W para controle de iluminação em diferentes ambientes simulados por uma matriz de LEDs. O sistema permite a conexão via Wi-Fi e interação através de uma página web, permitindo ligar e desligar luzes remotamente.

## Funcionalidades

- Conexão à rede Wi-Fi e criação de um servidor web na porta 80.
- Página HTML com botões para acender e desligar luzes de:
  - Garagem
  - Sala
  - Quarto
  - Escritório
- Visualização das luzes na matriz de LEDs endereçáveis (WS2812).
- Controle via requisições HTTP (GET).
- Indicação visual no LED onboard da placa.
- Suporte ao modo BOOTSEL via botão físico (GPIO 6).

## Como Usar

1. **Configure o Wi-Fi:**
   - No código, substitua `WIFI_SSID` e `WIFI_PASSWORD` com suas credenciais.

2. **Compile o Projeto:**
   - Use o SDK do Raspberry Pi Pico e o `cmake` para compilar o projeto com as bibliotecas necessárias (lwIP, cyw43, etc.).

3. **Carregue o Programa:**
   - Envie o arquivo `.uf2` gerado para o Raspberry Pi Pico W em modo BOOTSEL.

4. **Conecte-se à Rede:**
   - Verifique o terminal serial para obter o IP do dispositivo.

5. **Controle via Navegador:**
   - Acesse o IP mostrado no terminal a partir de um navegador para visualizar a interface de controle das luzes.

## Componentes Utilizados

- Raspberry Pi Pico W
- Módulo Wi-Fi CYW43439 (integrado)
- Matriz de LEDs WS2812 (5x5)
- Botão físico (GPIO 6)
- IDE de desenvolvimento com suporte ao SDK do Pico
- Biblioteca lwIP para TCP/IP
- Biblioteca de controle de LEDs `matrizLed.h` (implementação própria)

## Autor
## Matheus Nepomuceno Souza


