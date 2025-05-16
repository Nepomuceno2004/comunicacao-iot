#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hardware/gpio.h"

#include "lib/matrizLed.h"
#include "lib/joystick.h"

#include <stdlib.h>
#include <string.h>
#include "pico/cyw43_arch.h"
#include "lwip/pbuf.h"
#include "lwip/tcp.h"
#include "lwip/netif.h"

// configuração do wifi
#define WIFI_SSID "SSID"
#define WIFI_PASSWORD "PASSWORD"

// Pino do LED de status do Wi-Fi
#define LED_PIN CYW43_WL_GPIO_LED_PIN

// pino do botão
#define botaoB 6

double temperatura = 25.0; // Começa com 25°C
double ph = 7.0;           // Começa 7.0 de ph

#define MAX_PH 14.0
#define MIN_PH 0.0

#define STEP 0.4 // Passo de variação para cada movimento do joystick

// matrizes que definem quais leds acendem
bool escritorio[NUM_PIXELS] = {
    1, 1, 0, 0, 0,
    0, 0, 0, 1, 1,
    0, 0, 0, 0, 0,
    0, 0, 0, 0, 0,
    0, 0, 0, 0, 0};

bool quarto[NUM_PIXELS] = {
    0, 0, 0, 1, 1,
    1, 1, 0, 0, 0,
    0, 0, 0, 0, 0,
    0, 0, 0, 0, 0,
    0, 0, 0, 0, 0};

bool garagem[NUM_PIXELS] = {
    0, 0, 0, 0, 0,
    0, 0, 0, 0, 0,
    0, 0, 0, 0, 0,
    0, 0, 0, 1, 1,
    1, 1, 0, 0, 0};

bool sala[NUM_PIXELS] = {
    0, 0, 0, 0, 0,
    0, 0, 0, 0, 0,
    0, 0, 0, 0, 0,
    1, 1, 0, 0, 0,
    0, 0, 0, 1, 1};

// Funções de servidor TCP
static err_t tcp_server_accept(void *arg, struct tcp_pcb *newpcb, err_t err);
static err_t tcp_server_recv(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err);

// Função que lida com o pedido do usuário
void user_request(char **request);

// Trecho para modo BOOTSEL com botão B
#include "pico/bootrom.h"
void gpio_irq_handler(uint gpio, uint32_t events)
{
    reset_usb_boot(0, 0);
}

int main()
{
    stdio_init_all();

    // inicializa e configura o botão
    gpio_init(botaoB);
    gpio_set_dir(botaoB, GPIO_IN);
    gpio_pull_up(botaoB);
    gpio_set_irq_enabled_with_callback(botaoB, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler);
    // Aqui termina o trecho para modo BOOTSEL com botão B

    matriz_init();
    joystick_init();

    // Tentativa de inicializar o Wi-Fi
    while (cyw43_arch_init())
    {
        printf("Falha ao inicializar Wi-Fi\n");
        sleep_ms(100);
        return -1;
    }

    // Desliga o LED de status do Wi-Fi
    cyw43_arch_gpio_put(LED_PIN, 0);

    // Configura o Wi-Fi para o modo cliente
    cyw43_arch_enable_sta_mode();

    printf("Conectando ao Wi-Fi...\n");
    // Tentativa de conexão ao Wi-Fi
    while (cyw43_arch_wifi_connect_timeout_ms(WIFI_SSID, WIFI_PASSWORD, CYW43_AUTH_WPA2_AES_PSK, 50000))
    {
        printf("Falha ao conectar ao Wi-Fi\n");
        sleep_ms(100);
        return -1;
    }
    printf("Conectado ao Wi-Fi\n");

    if (netif_default)
    {
        printf("IP do dispositivo: %s\n", ipaddr_ntoa(&netif_default->ip_addr));
    }

    // Configuração do servidor TCP
    struct tcp_pcb *server = tcp_new();
    if (!server)
    {
        printf("Falha ao criar servidor TCP\n");
        return -1;
    }

    if (tcp_bind(server, IP_ADDR_ANY, 80) != ERR_OK)
    {
        printf("Falha ao associar servidor TCP à porta 80\n");
        return -1;
    }

    // Inicia o servidor TCP
    server = tcp_listen(server);

    // Configura a função de aceitação de conexão
    tcp_accept(server, tcp_server_accept);
    printf("Servidor ouvindo na porta 80\n");

    while (true)
    {
        cyw43_arch_poll(); // mantém a conexão ativa
        sleep_ms(100);
    }

    // "Desinicializa" a arquitetura do Wi-Fi
    cyw43_arch_deinit();
    return 0;
}

void condition_read(void)
{
    int16_t dx, dy;
    joystick_leitura_corrigida(&dx, &dy);
    printf("Temperatura X: %i, PH Y: %d\n", dx, dy);

    // Ajuste de temperatura (eixo X)
    temperatura += (dx > 0) ? STEP : -STEP;

    // Ajuste de ph (eixo Y)
    if (abs(dy) > 50)
    {
        if ((ph - STEP >= 0) && (ph + STEP <= 14))
        {
            ph += (dy > 0) ? STEP : -STEP;
        }
    }

    return;
}

// Função de aceitação de conexão TCP
static err_t tcp_server_accept(void *arg, struct tcp_pcb *newpcb, err_t err)
{
    tcp_recv(newpcb, tcp_server_recv);
    return ERR_OK;
}

// Função que trata as requisições HTTP dos usuários
void user_request(char **request)
{
    // Checa se a requisição contém um comando para acender ou apagar uma luz
    if (strstr(*request, "GET /garagem_on") != NULL)
    {
        atualizarLeds(garagem, true);
    }
    else if (strstr(*request, "GET /garagem_off") != NULL)
    {
        atualizarLeds(garagem, false);
    }
    else if (strstr(*request, "GET /sala_on") != NULL)
    {
        atualizarLeds(sala, true);
    }
    else if (strstr(*request, "GET /sala_off") != NULL)
    {
        atualizarLeds(sala, false);
    }
    else if (strstr(*request, "GET /quarto_on") != NULL)
    {
        atualizarLeds(quarto, true);
    }
    else if (strstr(*request, "GET /quarto_off") != NULL)
    {
        atualizarLeds(quarto, false);
    }
    else if (strstr(*request, "GET /escritorio_on") != NULL)
    {
        atualizarLeds(escritorio, true);
    }
    else if (strstr(*request, "GET /escritorio_off") != NULL)
    {
        atualizarLeds(escritorio, false);
    }
    else if (strstr(*request, "GET /on") != NULL)
    {
        cyw43_arch_gpio_put(LED_PIN, 1);
    }
    else if (strstr(*request, "GET /off") != NULL)
    {
        cyw43_arch_gpio_put(LED_PIN, 0);
    }
};

// Função que recebe dados da conexão TCP
static err_t tcp_server_recv(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err)
{
    if (!p)
    {
        tcp_close(tpcb);
        tcp_recv(tpcb, NULL);
        return ERR_OK;
    }

    char *request = (char *)malloc(p->len + 1);
    memcpy(request, p->payload, p->len);
    request[p->len] = '\0';

    printf("Request: %s\n", request);

    user_request(&request);

    condition_read();

    char html[1024];

    snprintf(html, sizeof(html),
             "HTTP/1.1 200 OK\r\n"
             "Content-Type: text/html\r\n"
             "\r\n"
             "<!DOCTYPE html>\n"
             "<html>\n"
             "<head>\n"
             "<title>Luzes</title>\n"
             "<meta http-equiv='refresh' content='5'>" // Atualiza a página a cada 5 segundos
             "<style>\n"
             "body{background:#191919;font-family:sans-serif;text-align:center;margin-top:50px;}\n"
             "h1{color:#fff;font-size:30px;margin-bottom:20px;}\n"
             "button{background:LightGray;font-size:24px;margin:6px;padding:10px 20px;border-radius:8px;}\n"
             "</style>\n"
             "</head>\n"
             "<body>\n"
             "<h1>Luzes</h1>\n"
             "<form action=\"./sala_on\"><button>Sala ON</button></form>\n"
             "<form action=\"./sala_off\"><button>Sala OFF</button></form>\n"
             "<form action=\"./quarto_on\"><button>Quarto ON</button></form>\n"
             "<form action=\"./quarto_off\"><button>Quarto OFF</button></form>\n"
             "<form action=\"./escritorio_on\"><button>Escritorio ON</button></form>\n"
             "<form action=\"./escritorio_off\"><button>Escritorio OFF</button></form>\n"
             "<form action=\"./garagem_on\"><button>Garagem ON</button></form>\n"
             "<form action=\"./garagem_off\"><button>Garagem OFF</button></form>\n"
             "<h1><b>AQUARIO:</b></h1>\n"
             "<h1>Temperatura: %.2f C</h1>\n"
             "<h1>PH: %.2f</h1>\n"
             "</body>\n"
             "</html>\n",
             temperatura, ph);

    // Envia a resposta HTTP com o HTML
    tcp_write(tpcb, html, strlen(html), TCP_WRITE_FLAG_COPY);
    // Envia os dados para o cliente
    tcp_output(tpcb);

    // libera a memória e o buffer de dados
    free(request);
    pbuf_free(p);

    return ERR_OK;
}