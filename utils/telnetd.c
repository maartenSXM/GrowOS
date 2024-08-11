/*
 * Inspired by git@github.com:nkolban/esp32-snippets.git
 * by Neil Kolban <kolban1@kolban.com>, with thanks.
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/param.h>
#include <sys/fcntl.h>
#include <sys/errno.h>
#include <sys/ioctl.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include "freertos/FreeRTOS.h"
#include <lwip/def.h>
#include <lwip/sockets.h>

#include "esp_log.h"
#include "esp_vfs.h"
#include "esp_vfs_dev.h"
#include "driver/uart.h"
#include "esp_idf_version.h"

#include "gos.h"
#include "libtelnet.h"
#include "webApi.h"

#define UART_NAME   "/dev/uart/0"
#define UART_NUM   UART_NUM_0

// #define PROMPT	 "%% "
#define PROMPT	 "\x1b[92mGrowOS %%\x1b[0m "

static int uart_fd = -1;

static char td_tag[] = "telnetd";

#if 0
FILE *_myStdout; // XXX make static

#define printf(fmt, ...)			\
 do						\
   {						\
   if (_myStdout != NULL)			\
     fprintf(_myStdout, fmt, ##__VA_ARGS__);	\
   else						\
     fprintf(stdout, fmt, ##__VA_ARGS__);	\
   } while (0)
#endif

static void (*myRecvCallback)(int sd, uint8_t *buffer, size_t size);

static void myTelnetCallback
    (
    telnet_t *thisTelnet,
    telnet_event_t *event,
    void *userData
    )
    {
    int sd = (int) userData;

    switch (event->type)
        {
        case TELNET_EV_SEND:
    	    if (send(sd, event->data.buffer, event->data.size, 0) < 0)
    		ESP_LOGE(td_tag, "send: %d (%s)", errno, strerror(errno));
        break;

        case TELNET_EV_DATA:
    	    // ESP_LOGD(td_tag, "received data, len=%d", event->data.size);
    	    if (myRecvCallback != NULL)
		{
    	        myRecvCallback (sd, (uint8_t *) (event->data.buffer),
				     (size_t) (event->data.size));
		}
        break;
    
        default:
	break;
        }
    }

static void doTelnet(int sd)
    {
    fd_set rfds;
    int s, readBytes;
    size_t sizeBytes;
    int maxFd = (sd >= uart_fd ? sd : uart_fd) + 1;
    struct timeval tv =
        {
        .tv_sec = 5,
        .tv_usec = 0,
        };

    static const telnet_telopt_t myTelnetOpts[] = {
	{ TELNET_TELOPT_ECHO,      TELNET_WILL, TELNET_DONT },
	{ TELNET_TELOPT_TTYPE,     TELNET_WILL, TELNET_DONT },
	{ TELNET_TELOPT_COMPRESS2, TELNET_WONT, TELNET_DO   },
	{ TELNET_TELOPT_ZMP,       TELNET_WONT, TELNET_DO   },
	{ TELNET_TELOPT_MSSP,      TELNET_WONT, TELNET_DO   },
	{ TELNET_TELOPT_BINARY,    TELNET_WONT, TELNET_DO   },
	{ TELNET_TELOPT_NAWS,      TELNET_WILL, TELNET_DONT },
	{ -1, 0, 0 }
    };

    uint8_t buf[128];

    telnet_t *tnHandle;

    tnHandle = telnet_init (myTelnetOpts, myTelnetCallback, 0, (void *) sd);

    printf ("\n\x1b[92;1mWelcome to %s!\n\n",
					GOS_STRINGIFY(GOS_BSP_esphome_name));
    printf (PROMPT);

    FD_ZERO (&rfds);
  
    while(1)
	{
	if (uart_fd >= 0)
            FD_SET  (uart_fd, &rfds);
        FD_SET  (sd, &rfds);

        s = select (maxFd, &rfds, NULL, NULL, &tv);

        if (s < 0)
            {
            if (errno == EINTR)
                continue;
            ESP_LOGE(td_tag, "Select failed: errno %d", errno);
            printf("Select failed\n");
            goto bail;
            }

        if (s == 0) // timeout
            continue;

        if (uart_fd >=0 && FD_ISSET(uart_fd, &rfds))
            {
            if (uart_get_buffered_data_len(UART_NUM, &sizeBytes) != ESP_OK)
                {
                ESP_LOGE(td_tag, "UART peek error");
                printf("UART peek error\n");
                goto bail;
                }

	    if (sizeBytes == 0)
	        continue;

            if (sizeBytes > sizeof(buf))
                sizeBytes = sizeof(buf);

            if ((readBytes = read (uart_fd, buf, sizeBytes)) <= 0)
                {
                ESP_LOGE(td_tag, "UART read error");
                printf("UART read error\n");
                goto bail;
                }

            if (write (sd, buf, readBytes) < 0)
                {
                ESP_LOGE(td_tag, "client disconnected on write");
                printf("client disconnected on write\n");
                goto bail;
                }
            }

        if (FD_ISSET(sd, &rfds))
            {
  	    if ((readBytes = recv (sd, (char *)buf, sizeof(buf), 0)) <=0)
                {
                ESP_LOGE (td_tag, "client disconnected on read");
                goto bail;
                }
  	    telnet_recv (tnHandle, (char *)buf, readBytes);
	    }
	}
bail:
    // ESP_LOGD(td_tag, "Telnet client finished");

    telnet_free(tnHandle);
    }

void telnetMyListen
    (
    void (*callbackParam)(int sd, uint8_t *buffer, size_t size)
    )
    {

    int csock;
    struct sockaddr_in saddr;
    int ssock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    myRecvCallback = callbackParam;

    saddr.sin_family = AF_INET;
    saddr.sin_addr.s_addr = htonl(INADDR_ANY);
    saddr.sin_port = htons(23);

    if (bind(ssock, (struct sockaddr *)&saddr, sizeof(saddr)) < 0)
	{
    	ESP_LOGE(td_tag, "bind: %d (%s)", errno, strerror(errno));
    	return;
        }

    // only one telnet client at a time, to save resources
    if (listen(ssock, 1) < 0)
	{
    	ESP_LOGE(td_tag, "listen: %d (%s)", errno, strerror(errno));
    	return;
        }

    while(1)
	{
	FILE *saveStdout;

    	socklen_t len = sizeof(saddr);
    	if ((csock = accept(ssock, (struct sockaddr *)&saddr, &len)) < 0)
	    {
    	    ESP_LOGE(td_tag, "accept: %d (%s)", errno, strerror(errno));
    	    return;
    	    }

    	ESP_LOGD(td_tag, "We have a new client connection!");

	saveStdout = stdout;
	stdout = fdopen (csock, "w");     // redirect printf() to csock
	
	// setvbuf(stdout, NULL, _IOLBF, 256);
	// setlinebuf (stdout);

	// enable line buffering didn't work, so turn it off
	setvbuf(stdout, NULL, _IONBF, 0);

    	doTelnet (csock);

	close (csock);

	stdout = saveStdout;
        }
    }

// called with type, id, name, value, state character strings
static void getids_cb(char *t, char *i, char *n, char *v, char *s)
    {
    printf("%s %s (%s) %s", t, i, n, v);    // type, id, name, value
    if (strcmp(s, v) != 0)
        printf(" (%s)", s);                 // state
    printf("\n");
    }

#define HTTP_LINE_SIZE 256

void doEsphomeCmd (int sd, char *cmd)
    {
    int ret = 0;
    char *me = "esphome";
    static char *pBuf;
    static char *h, *t, *i, *m; // host, type, id, method

    if (pBuf == NULL)
        {
        if (((pBuf = malloc (HTTP_LINE_SIZE)) == NULL) ||
           ((h = malloc (128)) == NULL))
    	{
    	printf ("telnet: out of memory\n");
    	shutdown (sd, SHUT_RD);
    	if (uart_fd >= 0)
    	    {
    	    close (uart_fd);
    	    uart_fd = -1;
    	    }
    	return;
    	}
        // give h, t, i, and m 32 bytes each
        t = h + 32; i = t + 32; m = i + 32;
        *h = *t = *i = *m =0;
        }

    ret = sscanf (cmd, " %s %s %s %s", h, t, i, m);

    if (ret == 1) // we are listing all entity ids
        {
        if (httpGetIds(h, pBuf, HTTP_LINE_SIZE, getids_cb) < 0)
	   printf("httpGetIds() failed\n");
        return;
        }

    if (ret == 3) // get the value of a specific entity id
        {
        if (httpGetState(h, t, i, pBuf, HTTP_LINE_SIZE) < 0)
	   printf("httpGetState() failed\n");
        else
	   printf("%s\n", pBuf);
        return;
        }

    if (ret == 4) // run a method on an entity id
        {
        if (httpPost(h, t, i, m, pBuf, HTTP_LINE_SIZE) < 0)
    	printf("httpPost() failed\n");
        else
    	printf("ok\n");
        return;
        }

    printf("Usage:\n"
	"Get list of entities: %s <hostname | ip>\n"
	"Get state of entity:  %s <hostname | ip> <type> <id>\n"
	"Set state of entity:  %s <hostname | ip> <type> <id> <method>\n"
	"Examples:\n"
	"%s 192.168.248.20\n"
	"%s 192.168.248.20 sensor net_wifi_signal_\n"
	"%s 192.168.248.20 switch cpu1_enable turn_on\n"
	"%s esphome6 light room2 \'turn_on?brightness=128&transition=2\'\n"
	, me, me, me, me, me, me, me);
    }

static void myRecvHandler(int sd, uint8_t *buffer, size_t size)
    {
    char *p, *line = (char *) buffer;

    // find CR, truncate buffer there
    p = line;
    while (size != 0)
        {
        if (*p == '\r')
	    {
	    *p = '\0';
	    break;
	    }
        p++, size--;
	}

    do { // so we can break instead of using a goto

      // no carriage return or empty line, nothing to do
      if (size == 0 || line[0] == '\0')
	break;

      if (strcmp (line, "help") ==0)
        {
	printf
	    (
	    "help\n"
	    "	this help\n"
	    "quit\n"
	    "	exit telnet session\n"
	    "esphome\n"
	    "	issue esphome commands\n"
	    "set uart_monitor on\n"
	    "	enable monitoring of UART\n"
	    "set uart_monitor off\n"
	    "	disable monitoring of UART\n"
	    );
	break;
	}

      if (strcmp (line, "quit") ==0)
        {
	printf("Good bye.\n");
    	shutdown (sd, SHUT_RD);
	if (uart_fd >= 0)
	    {
	    close (uart_fd);
	    uart_fd = -1;
    	    }
	return;
	}

      if (strncmp (line, "esphome", 7) == 0)
        {
	doEsphomeCmd (sd, line + 7);
	break;
	}

      if (strcmp (line, "set uart_monitor on") ==0)
        {
	if (uart_fd >= 0)
            printf ("uart_monitor is already on\n");
	else
	    {
	    if ((uart_fd = open(UART_NAME, O_RDWR)) < 0)
		{
		ESP_LOGE(td_tag, "Unable to open UART");
		printf("Unable to open UART\n");
		}
	    else
		printf ("ok\n");
	    }
	break;
	}

      if (strcmp (line, "set uart_monitor off") ==0)
        {
	if (uart_fd >= 0)
	    {
	    close (uart_fd);
	    uart_fd = -1;
            printf ("ok\n");
	    }
	else
            printf ("uart_monitor is not on\n");
	break;
	}

      // if we get here, no legit command was found

      printf ("huh?\n");

      } while (0); // end of do

    printf (PROMPT);
    }

static void telnetd(void *data)
    {
    uart_config_t uart_config =
        {
        .baud_rate = 115200,
        .data_bits = UART_DATA_8_BITS,
        .parity    = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE
        };

    ESP_LOGD (td_tag, ">> telnetTask");

    uart_param_config (UART_NUM, &uart_config);

    if (uart_driver_install(UART_NUM_0, 256, 0, 0, NULL, 0) != ESP_OK)
        {
        ESP_LOGE(td_tag, "Driver installation failed");
	vTaskDelete (NULL);
        }

    esp_vfs_dev_uart_register();	    // 5.2
    esp_vfs_dev_uart_use_driver(0);	    // 5.2

    // uart_vfs_dev_register();		    // 5.3
    // uart_vfs_dev_use_driver(UART_NUM);   // 5.3

    telnetMyListen (myRecvHandler);

    ESP_LOGD (td_tag, "<< telnetTask");

    vTaskDelete (NULL);
    }

esp_err_t telnetServerInit (void)
    {
    static int init = 0;

    StaticTask_t    *pTcb;
    StackType_t	    *pStack;

    if (init)
	return ESP_OK;

    ESP_LOGD(td_tag, "telnetServerInit() starting");

    // allocate telnetd stack and scratch space in PSRAM */
#define ALLOC_INTERNAL  (MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT)
#define ALLOC_PSRAM	(MALLOC_CAP_SPIRAM   | MALLOC_CAP_8BIT)
#define STACK_SIZE	(20 * 1024 / sizeof(int)) // in words

    pTcb =   heap_caps_malloc(sizeof(StaticTask_t), ALLOC_INTERNAL);
    pStack = heap_caps_malloc(STACK_SIZE, ALLOC_PSRAM);

    // xTaskCreateStatic() returns NULL if pStack or pTcb are NULL
    if (xTaskCreateStatic (telnetd, "telnetd", STACK_SIZE, NULL, 5,
						pStack, pTcb) == NULL)
	{
	if (pTcb)	    // even though free(NULL) is legit
	    free (pTcb);
	if (pStack)	    // even though free(NULL) is legit
	    free (pStack);
        return ESP_FAIL;
	}

    init = 1;

    ESP_LOGI(td_tag, "telnetServerInit() done");

    return ESP_OK;
    }

