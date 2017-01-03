/* http_get - Retrieves a web page over HTTP GET.
 *
 * See http_get_ssl for a TLS-enabled version.
 *
 * This sample code is in the public domain.,


TODO: fixa protokoll

    paket för GPIO
    paket för UART




 */
#include "espressif/esp_common.h"
#include "esp/uart.h"

#include <string.h>

#include "FreeRTOS.h"
#include "task.h"

#include "lwip/err.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include "lwip/netdb.h"
#include "lwip/api.h"

#include "lwip/dns.h"

#include "xtensa_ops.h"
#include "esp/interrupts.h"


//#include "ssid_config.h"



#define PORT 420

struct protocol_packet {
    uint16_t command;
    uint16_t length;
};

//#define printf(...) ;

int uart_read(int uart, char *ptr, int len) {
    int ch, i;
    for(i = 0; i < len; i++) {
        ch = uart_getc_nowait(uart);
        if (ch < 0) break;
        ptr[i] = ch;
    }
    return i;
}

int uart_write(int uart, const char *ptr, int len) {
    for(int i = 0; i < len; i++) {
        uart_putc(uart, ptr[i]);
    }
    return len;
}

volatile int hue = 0;
volatile int hue_sep = 50;//13;
volatile int hue_speed = 3;//1;
volatile int hue_delay = 1;//10;


static void server_task(void *pvParameters) {

    printf("Setting up socket for listening\n");
    struct netconn *nc = netconn_new(NETCONN_TCP);

    if (netconn_bind(nc, IP_ADDR_ANY, PORT) != ERR_OK) {
        printf("Failed to bind!\n");
        return;
    }
     
    netconn_listen(nc);
    struct netconn *client = NULL;

    while (1) {
        printf("Awaiting connection\n");

        err_t err = netconn_accept(nc, &client);
        printf("Connection established\n");

        if (err == ERR_OK) {
            printf("Accepted\n");
            //TEST_ASSERT_TRUE_MESSAGE(err == ERR_OK, "Error accepting connection");

            ip_addr_t client_addr;
            uint16_t port_ignore;
            netconn_peer(client, &client_addr, &port_ignore);

            // snprintf(buf, sizeof(buf), "test ping\r\n");
            // printf("Device A: send data: %s\n", buf);
            // netconn_write(client, buf, strlen(buf), NETCONN_COPY);

            struct pbuf *pb = NULL;
            // for (int i = 0; i < 10; i++) {
                //TEST_ASSERT_EQUAL_INT_MESSAGE(ERR_OK, netconn_recv_tcp_pbuf(client, &pb), "Failed to receive data");
            while (netconn_recv_tcp_pbuf(client, &pb) == ERR_OK) {
                struct protocol_packet* pp;

                // printf("Recieved data: ");
                // uart_write(0, pb->payload, pb->len);

                //Check if payload begins with djävulskanin

                if (pb->len >= strlen("djävulskanin")) {
                    if (strncmp(pb->payload, "djävulskanin", strlen("djävulskanin")) == 0) {
                        printf("Kanin!\n");
                    }

                }

                // printf("\n");

                // if (pb->len >= sizeof(pp)) {
                //     pp = (struct protocol_packet*) pb->payload;
                //     printf("Command: %i  Length: %i  Data: %i\n", pp->command, pp->length, pb->len);

                //     struct protocol_packet answer;

                //     switch (pp->command) {

                //         case 0x10:  //Send data to uart
                //             answer = (struct protocol_packet) {
                //                 .command = 0x1001,   //Ack
                //                 .length = 0,
                //             };

                //             uart_write(0, pb->payload + sizeof(struct protocol_packet), pp->length);
                //             uart_flush_txfifo(0);

                //             netconn_write(client, &answer , sizeof(answer), NETCONN_COPY);
                //          break;

                //         case 0x11:  //enable gpio output

                //             //we should check size of packet first
                //             {
                //                 uint16_t* gpio_id = (uint16_t*)(pb->payload + sizeof(struct protocol_packet));
                //                 gpio_enable(*gpio_id, GPIO_OUTPUT);
                //                 answer = (struct protocol_packet) {
                //                     .command = 0x1001,   //Ack
                //                     .length = 0,
                //                 };
                //                 netconn_write(client, &answer , sizeof(answer), NETCONN_COPY);
                //             }
                //             break;

                //         case 0x12:  //write gpio

                //             //we should check size of packet first and use a struct is nicer
                //             {
                //                 uint16_t* gpio_id = (uint16_t*) (pb->payload + sizeof(struct protocol_packet));
                //                 gpio_write(gpio_id[0], gpio_id[1]);
                //                 answer = (struct protocol_packet) {
                //                     .command = 0x1001,   //Ack
                //                     .length = 0,
                //                 };
                //                 netconn_write(client, &answer , sizeof(answer), NETCONN_COPY);
                //             }
                //             break;

                //         case 0x13:  //read uart

                //             //we should check size of packet first
                //             {
                //                 uint16_t* data_to_read = (uint16_t*) (pb->payload + sizeof(struct protocol_packet));

                //                 void* databuf = calloc(*data_to_read, 1);
                //                 //We should check pointer

                //                 uint16_t data_read = uart_read(0, databuf, *data_to_read);

                //                 answer = (struct protocol_packet) {
                //                     .command = 0x1001,   //Ack
                //                     .length = data_read,
                //                 };
                //                 netconn_write(client, &answer , sizeof(answer), NETCONN_COPY);
                //                 netconn_write(client, databuf , data_read, NETCONN_COPY);
                //                 free(databuf);
                //             }
                //             break;

                //         case 0x14:  //setup uart

                //             //we should check size of packet first
                //             {
                //                 uint32_t* vvp = (uint32_t*) (pb->payload + sizeof(struct protocol_packet));

                //                 printf("Payload address: %p\nbaud address: %p\n", pb->payload, vvp);


                //                 //uart_set_baud(0, *baudrate);
                //                 int vv = *vvp;
                //                 printf("Uart baudrate: %i\n", vv);
                //                 //uart_set_baud(0, 115200);
                //                 //we should check errors, if we can

                //                 answer = (struct protocol_packet) {
                //                     .command = 0x1001,   //Ack
                //                     .length = 0,
                //                 };
                //                 netconn_write(client, &answer , sizeof(answer), NETCONN_COPY);
                //             }
                //             break;

                //         default:
                //             answer = (struct protocol_packet) {
                //                 .command = 0x1000,   //NAck
                //                 .length = 0,
                //             };
                //             netconn_write(client, &answer , sizeof(answer), NETCONN_COPY);
                //             break;
                //     }



                // } else {
                //     printf("Unfinished packet\n");
                //     vTaskDelay(10 / portTICK_PERIOD_MS);
                // }

                pbuf_free(pb);

            }


            //TEST_ASSERT_TRUE_MESSAGE(pb->len > 0, "No data received");
            //printf("Device A: received data: %s\n", buf);
            //TEST_ASSERT_FALSE_MESSAGE(strcmp((const char*)buf, "test pong\r\n"), "Received wrong data");
            printf("Done\n");
            netconn_delete(client);
        } else {
            printf("Failed to accept\n");
        }

    }
}

//gpio_set_iomux_function(2, IOMUX_GPIO2_FUNC_UART1_TXD);



// static void hello_world(void* args) {
//     while(1) {
//         char buf[32];        
//         uart_rxfifo_wait(0, 1);
//         int bl = uart_read(0, buf, 32);
//         uart_write(0, "Echoed data is: ", 16);
//         uart_write(0, buf, bl);
//         uart_write(0, "\r\n", 2);
//         //printf("Wrote %i bytes\n", bl);
//         //vTaskDelay(1000 / portTICK_PERIOD_MS);
//     }
// }




inline void colorHexagon(int hue, int *R, int *G, int *B) {
    int frac = hue >> 8;
    int ci = hue & 0xFF;
    int cd = 255 - ci;
    int cs = 255;
    switch (frac) {
        case 0: *R = cs;    *G = ci;    *B = 0; break;      //R1    G+  B0
        case 1: *R = cd;    *G = cs;    *B = 0; break;      //R-    G1  B0
        case 2: *R = 0; *G = cs;    *B = ci; break; //R0    G1  B+
        case 3: *R = 0; *G = cd;    *B = cs; break; //R0    G-  B1
        case 4: *R = ci;    *G = 0; *B = cs; break; //R+    G0  B1
        case 5: *R = cs;    *G = 0; *B = cd; break; //R1    G0  B-
    }
}



void create_led_byte(char* target, int value) {
    for (int i=0; i<8; i++) {
        target[i] = (1 << (7-i)) & value ? 0xFE : 0x80;
    }
}



static void bunny_cycle(void* args) {
    uart_set_baud(1, 5000000);
    const int ledcount = 19;


    int hue = 0;
    int hue_sep = 50;//13;
    int hue_speed = 3;//1;
    int hue_delay = 1;//10;


    char ledbuf[24*ledcount];

    while(1) {

        hue = (hue + hue_speed) % (6*256);

        for (int led=0; led<19; led++) {
            int R, G, B;
            colorHexagon((hue + led*hue_sep) % (6*256), &R, &G, &B);

            create_led_byte(&ledbuf[led*24+0], R);
            create_led_byte(&ledbuf[led*24+8], G);
            create_led_byte(&ledbuf[led*24+16], B);

        }
        uart_write(1, ledbuf, 24*ledcount);

        //vTaskDelay(hue_delay / portTICK_PERIOD_MS);
        vTaskDelay(5 / portTICK_PERIOD_MS);
        
    }

}


volatile int irq_count=0;

void IRAM uart_irq(void) {
    irq_count++;
}

void debug_data(void* args) {
    while (1) {
        vTaskDelay(1000/portTICK_PERIOD_MS);
        printf("IRQ count: %i\n", irq_count);
    }
}

void user_init(void) {
//    uart_set_baud(0, 115200);

    iomux_set_pullup_flags(gpio_to_iomux(2), 0);
    iomux_set_function(gpio_to_iomux(2), IOMUX_GPIO2_FUNC_UART1_TXD);

    // _xt_isr_attach(INUM_UART, uart_irq);
    // _xt_isr_unmask(BIT(INUM_UART));

    // vPortEnterCritical();
    // uint32_t ints_enabled = 0;
    // XSR(ints_enabled, intenable);
    // WSR(ints_enabled | BIT(INUM_UART), intenable);
    // vPortExitCritical();


//    gpio_enable(2, GPIO_OUTPUT);
    // gpio_enable(gpio, GPIO_OUTPUT);
    // gpio_enable(gpio, GPIO_OUTPUT);

    // gpio_write(gpio, 0);
    // gpio_write(gpio, 1);

    // iomux_set_pullup_flags(gpio_to_iomux(7), 0);
    // iomux_set_function(gpio_to_iomux(7), IOMUX_GPIO7_FUNC_UART1_TXD);
    // iomux_set_pullup_flags(gpio_to_iomux(8), IOMUX_PIN_PULLUP);
    // iomux_set_function(gpio_to_iomux(8), IOMUX_GPIO8_FUNC_UART1_RXD);


    //printf("SDK version:%s\n", sdk_system_get_sdk_version());

    struct sdk_station_config config = {
        .ssid = "SSID FOR WIFI",
        .password = "WPA KEY FOR WIFI",
    };

    /* required to call wifi_set_opmode before station_set_config */
    sdk_wifi_set_opmode(STATION_MODE);
    sdk_wifi_station_set_config(&config);

    xTaskCreate(&server_task, "server_task", 256, NULL, 2, NULL);
    xTaskCreate(&bunny_cycle, "bunny_cycle", 256, NULL, 10, NULL);
    xTaskCreate(&debug_data, "debug_data", 256, NULL, 2, NULL);
//    xTaskCreate(&hello_world, "hello_world", 256, NULL, 2, NULL);
}

