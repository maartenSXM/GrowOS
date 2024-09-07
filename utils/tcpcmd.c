//  Connect to this using socat like this:
//	socat FILE:`tty`,onlcr=1 tcp:192.168.248.20:1337

#include <stdio.h>
#include <unistd.h>
#include <stdint.h>
#include <sys/fcntl.h>
#include <sys/param.h>
#include <string.h>
#include <errno.h>

// #default GOS_CONFIG_TCPCMD_PORT 1337
#ifndef GOS_CONFIG_TCPCMD_PORT
#define GOS_CONFIG_TCPCMD_PORT 1337
#endif

#ifdef unix

#define GOS_CONFIG_UART_MONITOR 0
#define ESP_LOGE(tag, fmt, ...) printf("%s:" fmt "\n", tag, ##__VA_ARGS__)
#define ESP_LOGD(tag, fmt, ...) printf("%s:" fmt "\n", tag, ##__VA_ARGS__)
#define vTaskDelete(x)
#define GOS_STRINGIFY(x) "unixTcpCmd"
#define WEBAPI_NO_MAIN
#include "webApi.c"

#else // !unix

#include "freertos/FreeRTOS.h"

#include "esp_log.h"
#include "esp_idf_version.h"

#if ESP_IDF_VERSION < ESP_IDF_VERSION_VAL(5,3,0)
  #include "soc/uart_struct.h"
  #include "driver/uart.h"
  #include "esp_vfs.h"
  #include "esp_vfs_dev.h"
#else // esp-idf version is >= 5.3.0
  #include "driver/uart.h"
  #include "esp_vfs.h"
  #include "driver/uart_vfs.h"	
#endif

#include <lwip/sockets.h>
#include <lwip/netdb.h>

#include "gos.h"
#include "webApi.h"

// #default GOS_CONFIG_UART_MONITOR yes
#ifndef GOS_CONFIG_UART_MONITOR 
#define GOS_CONFIG_UART_MONITOR 1
#endif

#if GOS_CONFIG_UART_MONITOR
#define UART_NAME   "/dev/uart/0"
#define UART_NUM   UART_NUM_0
static int uart_fd = -1;
#endif // GOS_CONFIG_UART_MONITOR

#endif // !unix

#define PROMPT	 "\x1b[92mGrowOS %%\x1b[0m "

static int clientLogout = 0;

static char tc_tag[] = "tcpcmdd";

static void tcpcmdRecv(int sd, uint8_t *buffer, size_t size);

#if 1
// #define MY_STDOUT
#ifdef MY_STDOUT
FILE *_myStdout; // XXX make static

#define printf(fmt, ...)			\
 do						\
   {						\
   if (_myStdout != NULL)			\
     {						\
     fprintf(_myStdout, fmt, ##__VA_ARGS__);	\
     fflush (_myStdout);			\
     }						\
   else						\
     {						\
     fprintf(stdout, fmt, ##__VA_ARGS__);	\
     fflush (stdout);				\
     }						\
   } while (0)
#endif
#else
int _myCsock = -1;
#define printf(fmt, ...)			\
 do						\
   {						\
   if (_myCsock != -1)				\
     dprintf(_myCsock, fmt, ##__VA_ARGS__);	\
   else						\
     {						\
     fprintf(stdout, fmt, ##__VA_ARGS__);	\
     fflush (stdout);				\
     }						\
   } while (0)
#endif

static void doClient(int sd)
    {
    fd_set rfds;
    int s, readBytes;
    size_t sizeBytes;
#if GOS_CONFIG_UART_MONITOR 
    int maxFd = (sd >= uart_fd ? sd : uart_fd) + 1;
#else
    int maxFd = sd + 1;
#endif // GOS_CONFIG_UART_MONITOR 
    struct timeval tv =
        {
        .tv_sec = 5,
        .tv_usec = 0,
        };

    uint8_t buf[128];

    printf ("\n\x1b[92;1mWelcome to %s!\n\n",
					GOS_STRINGIFY(GOS_PROJECT_NAME));
    printf (PROMPT);

    FD_ZERO (&rfds);

    while(1)
	{
#if GOS_CONFIG_UART_MONITOR 
	if (uart_fd >= 0)
            FD_SET  (uart_fd, &rfds);
#endif // GOS_CONFIG_UART_MONITOR

        FD_SET  (sd, &rfds);

        // s = select (maxFd, &rfds, NULL, NULL, &tv);
        s = select (maxFd, &rfds, NULL, NULL, NULL);

        if (s < 0)
            {
            if (errno == EINTR)
                continue;
            ESP_LOGE(tc_tag, "Select failed: errno %d", errno);
            printf("Select failed\n");
            break;
            }

	if (clientLogout)
	    {
	    clientLogout = 0;
  	    tcpcmdRecv (sd, (uint8_t *)"quit\r", 5);  // force cmd "quit"
	    // xxx - should also tear down tcpcmd daemon
	    break;
	    }

        if (s == 0) // timeout
            continue;

#if GOS_CONFIG_UART_MONITOR 
        if (uart_fd >=0 && FD_ISSET(uart_fd, &rfds))
            {
            if (uart_get_buffered_data_len(UART_NUM, &sizeBytes) != ESP_OK)
                {
                ESP_LOGE(tc_tag, "UART peek error");
                printf("UART peek error\n");
                break;
                }

	    if (sizeBytes == 0)
	        continue;

            if (sizeBytes > sizeof(buf))
                sizeBytes = sizeof(buf);

            if ((readBytes = read (uart_fd, buf, sizeBytes)) <= 0)
                {
                ESP_LOGE(tc_tag, "UART read error");
                printf("UART read error\n");
                break;
                }

            if (write (sd, buf, readBytes) < 0)
                {
                ESP_LOGE(tc_tag, "client disconnected on write");
                return;
                }
            }
#endif // GOS_CONFIG_UART_MONITOR 

        if (FD_ISSET(sd, &rfds))
            {
  	    if ((readBytes = recv (sd, (char *)buf, sizeof(buf), 0)) <=0)
                {
                ESP_LOGE (tc_tag, "client disconnected on read");
                return;
                }

  	    tcpcmdRecv (sd, buf, readBytes);
	    }
	}
    }

void tcpcmdListen()
    {

    int csock;
    const int enable=1;
    struct sockaddr_in saddr, caddr;
    int ssock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    if (ssock == -1)
        {
    	ESP_LOGE(tc_tag, "socket: %d (%s)", errno, strerror(errno));
	close (ssock);
	}

    if (setsockopt(ssock, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0)
        {
    	ESP_LOGE(tc_tag, "setsockopt: %d (%s)", errno, strerror(errno));
	close (ssock);
	}

    saddr.sin_family = AF_INET;
    saddr.sin_addr.s_addr = htonl(INADDR_ANY);
    saddr.sin_port = htons(GOS_CONFIG_TCPCMD_PORT);

    if (bind(ssock, (struct sockaddr *)&saddr, sizeof(saddr)) < 0)
	{
    	ESP_LOGE(tc_tag, "bind: %d (%s)", errno, strerror(errno));
	close (ssock);
    	return;
        }

    // only one client at a time, to save resources
    if (listen(ssock, 1) < 0)
	{
    	ESP_LOGE(tc_tag, "listen: %d (%s)", errno, strerror(errno));
	close (ssock);
    	return;
        }

    while(1)
	{
	FILE *saveStdout;
#ifdef MY_STDOUT
        FILE **pStdout = &_myStdout;
#else 
        FILE **pStdout = &stdout;
#endif
	// char stdout_buf[256];

    	socklen_t len = sizeof(caddr);
    	if ((csock = accept(ssock, (struct sockaddr *)&caddr, &len)) < 0)
	    {
    	    ESP_LOGE(tc_tag, "accept: %d (%s)", errno, strerror(errno));
    	    return;
    	    }

    	ESP_LOGD(tc_tag, "We have a new client connection!");

	saveStdout = *pStdout;
	*pStdout = fdopen (csock, "w");     // redirect printf() to csock
	
	// setvbuf(*pStdout, stdout_buf, _IOLBF, sizeof(stdout_buf));

	// enable line buffering didn't work, so turn it off
	setvbuf(*pStdout, NULL, _IONBF, 0);

	// _myCsock = csock;
    	doClient (csock);
	// _myCsock = -1;

	shutdown(csock, SHUT_RDWR);

	fclose (*pStdout);    // will also call close(csock);

	*pStdout = saveStdout;
        }
    }

#ifndef unix // for unix, we get this from webApi.c
// called with type, id, name, value, state character strings
static void getids_cb(char *t, char *i, char *n, char *v, char *s)
    {
    printf("%s %s (%s) %s", t, i, n, v);    // type, id, name, value
    if (strcmp(s, v) != 0)
        printf(" (%s)", s);                 // state
    printf("\n");
    }
#endif // !unix

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
    	printf ("tcpcmdd: out of memory\n");
    	shutdown (sd, SHUT_RDWR);
#if GOS_CONFIG_UART_MONITOR
    	if (uart_fd >= 0)
    	    {
    	    close (uart_fd);
    	    uart_fd = -1;
    	    }
#endif // GOS_CONFIG_UART_MONITOR
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

static void tcpcmdRecv(int sd, uint8_t *buffer, size_t size)
    {
    char *p, *line = (char *) buffer;

    // find CR, truncate buffer there
    p = line;
    while (size != 0)
        {
        if (*p == '\n')
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
	    "	exit command session\n"
	    "esphome\n"
	    "	issue esphome commands\n"
#if GOS_CONFIG_UART_MONITOR
	    "set uart_monitor on\n"
	    "	enable monitoring of UART\n"
	    "set uart_monitor off\n"
	    "	disable monitoring of UART\n"
#endif // GOS_CONFIG_UART_MONITOR
	    );
	break;
	}

      if (strcmp (line, "quit") ==0)
        {
	printf("Good bye.\n");
    	shutdown (sd, SHUT_RDWR);   // causes next select() to error out
#if GOS_CONFIG_UART_MONITOR
	if (uart_fd >= 0)
	    {
	    close (uart_fd);
	    uart_fd = -1;
    	    }
#endif // GOS_CONFIG_UART_MONITOR
	return;
	}

      if (strncmp (line, "esphome", 7) == 0)
        {
	doEsphomeCmd (sd, line + 7);
	break;
	}

#if GOS_CONFIG_UART_MONITOR
      if (strcmp (line, "set uart_monitor on") ==0)
        {
	if (uart_fd >= 0)
            printf ("uart_monitor is already on\n");
	else
	    {
	    if ((uart_fd = open(UART_NAME, O_RDWR)) < 0)
		{
		ESP_LOGE(tc_tag, "Unable to open UART");
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
#endif // GOS_CONFIG_UART_MONITOR

      // if we get here, no legit command was found

      printf ("huh?\n");

      } while (0); // end of do

    printf (PROMPT);
    }

static void tcpcmdd(void *data)
    {
#if GOS_CONFIG_UART_MONITOR
    uart_config_t uart_config =
        {
        .baud_rate = 115200,
        .data_bits = UART_DATA_8_BITS,
        .parity    = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE
        };
#endif // GOS_CONFIG_UART_MONITOR

    ESP_LOGD (tc_tag, ">> tcpcmdTask");

#if GOS_CONFIG_UART_MONITOR
    if (uart_param_config (UART_NUM, &uart_config) != ESP_OK)
        {
        ESP_LOGE(tc_tag, "Uart param config failed");
	vTaskDelete (NULL);
        }

    uart_driver_delete (UART_NUM_0);
    if (uart_driver_install(UART_NUM_0, 256, 0, 0, NULL, 0) != ESP_OK)
        {
        ESP_LOGE(tc_tag, "Uart driver install failed");
	vTaskDelete (NULL);
        }

#if ESP_IDF_VERSION < ESP_IDF_VERSION_VAL(5,3,0)
    esp_vfs_dev_uart_register();
    esp_vfs_dev_uart_use_driver(UART_NUM);
#else
    uart_vfs_dev_register();
    uart_vfs_dev_use_driver(UART_NUM);
#endif

#endif // GOS_CONFIG_UART_MONITOR

    tcpcmdListen ();

    ESP_LOGD (tc_tag, "<< tcpcmdTask");

    vTaskDelete (NULL);
    }

#define ALLOC_TCB_OPT    (MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT)
#define ALLOC_STACK_OPT	 (MALLOC_CAP_SPIRAM   | MALLOC_CAP_8BIT)

#define STACK_SIZE_BYTES (20 * 1024)
#define STACK_SIZE_WORDS (STACK_SIZE_BYTES / sizeof(StackType_t))

#ifdef unix
int main(void)
    {
    tcpcmdd((void *)NULL);
    return(0);
    }
#else // ! unix
esp_err_t tcpcmdServerInit (void)
    {
    static int init = 0;

#if 0
    StaticTask_t * pTcb;
    StackType_t	 * pStack;
#endif

    if (init)
	return ESP_OK;

#if 0
    pTcb =   heap_caps_malloc(sizeof(StaticTask_t), ALLOC_TCB_OPT);
    pStack = heap_caps_malloc(STACK_SIZE_BYTES, ALLOC_STACK_OPT);

    // xTaskCreateStatic() returns NULL if pStack or pTcb are NULL
    if (xTaskCreateStatic (tcpcmdd, "tcpcmdd", STACK_SIZE_WORDS, NULL, 5,
						pStack, pTcb) == NULL)
	{
	free (pTcb);	    // free(NULL) is legit
	free (pStack);	    // free(NULL) is legit
        return ESP_FAIL;
	}
#else
    if (xTaskCreate(tcpcmdd,"tcpcmdd",STACK_SIZE_WORDS,NULL,5,NULL) != pdPASS)
        return ESP_FAIL;
#endif

    init = 1;

    ESP_LOGI(tc_tag, "tcpcmdServerInit() done");

    return ESP_OK;
    }

esp_err_t tcpcmdServerTerm (void)
    {
    clientLogout = 1;

    ESP_LOGI(tc_tag, "tcpcmdServerTerm() done");

    return ESP_OK;
    }
#endif // !unix
