#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/param.h>
#include <sys/fcntl.h>
#include <sys/errno.h>
#include <sys/ioctl.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <netinet/in.h>

#ifdef unix
#include <netdb.h>
#include <arpa/inet.h>

static int _debug = 0;

// For testing on Linux, build with: make webApi
#define ESP_LOGE(tag, fmt, ...)	printf("%s:" fmt "\n", tag, ##__VA_ARGS__)
#define ESP_LOGD(tag, fmt, ...)	printf("%s:" fmt "\n", tag, ##__VA_ARGS__)
#define DEBUG(fmt, ...)	do {if (_debug) printf(fmt, ##__VA_ARGS__);} while (0)

#else // ! unix

#include "esp_log.h"

#include "lwip/err.h"
#include "lwip/sockets.h"
#include "lwip/netdb.h"

// extern FILE *_myStdout;
// #define DEBUG(fmt, ...)	do {fprintf(_myStdout, fmt, ##__VA_ARGS__);} while (0)
#define DEBUG(fmt, ...)	do {} while (0)

#endif // ! unix

#include "webApi.h"

#define BUF_SIZE 1024

static const char *TAG = "httpGet";

static int readHttpLine(int sd, char *buf, int len)
    {
    int ix;
    char c;

    // read until CR and discard the LF that must follows
    for (ix = 0; ix != len; ix++)
        {
        if (read (sd, &buf[ix], 1) < 0)
	    return (-1); // disconnect
	if (buf[ix] == 0x0d)
	    {
            buf [ix] = 0;     // terminate the line
            if ((read (sd, &c, 1) < 0) || (c != 0x0a)) // discard LF
	         return (-1); // disconnect
	    DEBUG("%s\n", buf);
	    return (0);
	    }
	} 
    DEBUG("Line too long\n");
    return (-1);	// line too long
    }

// find Http lines that start with string <str>
static int findHttpLine (int sd, char *str, char *buf, int len)
    {
    int slen = strlen(str);

    while (1)
        {
        if (readHttpLine (sd, buf, len) < 0)
            return (-1);
	if (strncmp (buf, str, slen) == 0)
	    break;
	}
    return (0);	// found it
    }

// find an empty Http line
static int findHttpEmptyLine (int sd, char *buf, int len)
    {
    while (1)
        {
        if (readHttpLine (sd, buf, len) < 0)
            return (-1);
	
	if (buf[0] == 0)
	    break;
	}
    return (0);	// found it
    }

// connect to http server and send a request
static int httpConnect (char *host, char *req)
    {
    int sd;

    struct addrinfo * res;
    struct in_addr  * addr;
    const struct addrinfo hints =
	{
        .ai_family   =	AF_INET,
        .ai_socktype =	SOCK_STREAM,
	};

    struct timeval  rwait;
    rwait.tv_sec    = 10;
    rwait.tv_usec   = 0;

   if (getaddrinfo(host, "80", &hints, &res) != 0 || res == NULL)
	{
        ESP_LOGE(TAG, "getaddrinfo failed");
        return (-1);
        }

    addr = &((struct sockaddr_in *)res->ai_addr)->sin_addr;
    DEBUG("... Using %s address for host %s\n", inet_ntoa(*addr), host);

    if ((sd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
	ESP_LOGE(TAG, "Could not create socket");
        return (-1);
	}

    if (connect(sd, res->ai_addr, res->ai_addrlen) != 0)
	{
	ESP_LOGE(TAG, "Could not connect");
	close (sd);
	freeaddrinfo (res);
        return (-1);
	}
    freeaddrinfo (res);
	
    if (setsockopt(sd, SOL_SOCKET, SO_RCVTIMEO, &rwait, sizeof(rwait)) < 0)
	{
	ESP_LOGE(TAG, "set recv timeout failed");
	close (sd);
        return (-1);
	}

    DEBUG("... Connected to %s\n", host);

    /* Send the HTTP GET */
    if (write(sd, req, strlen(req)) < 0)
	{
	ESP_LOGE(TAG, "write failed");
	close (sd);
        return (-1);
	}

    DEBUG("... GET request:\n%s", req);

    return (sd);
}

// issue http GET and get a single response from esphome
static int httpGet(char *host, char *req, char *buf, int buflen)
    {
    char *b;
    int clen, sd, ret;

    if ((sd = httpConnect (host, req)) < 0)
        {
	ESP_LOGE(TAG, "httpConnect failed");
	DEBUG("httpConnect failed");
        return (-1);
        }

    DEBUG("... GET headers:\n");

    // looking for HTTP/1.1 200 OK then extract Content-Length: value
    if ((findHttpLine (sd, "HTTP/1.1 200 OK", buf, buflen) < 0)  ||
	(findHttpLine (sd, "Content-Length: ", buf, buflen) < 0) ||
         ((clen = atoi (&buf[16])) < 0) || (clen >= buflen))
	{
	ESP_LOGE(TAG, "bad headers");
	close (sd);
        return (-1);
	}

    // read until a blank line is found signalling end of headers
    findHttpEmptyLine(sd, buf, buflen);

    DEBUG("... http server replied with %d data bytes\n", clen);

    // Now read the response of length: 0 < clen < buflen
    buf[clen] = 0;
    for (b = buf; clen > 0; b += ret, clen -= ret)
	{
	if ((ret = read(sd, b, clen)) < 0)
	    break;
	}

    if (clen != 0)
	{
	ESP_LOGE(TAG, "server disconnected");
	close (sd);
        return (-1);
	}

    if (buf[0] != 0)
	DEBUG ("... httGet() content:\n%s\n", buf);

    close(sd);
    return (0);
}

int httpGetState
    (
    char *host,
    char *type,
    char *id,
    char *buf,
    int buflen
    )
    {
    char *p;
    int sd, ret;
    char *value = "", *state = "";

    // construct initial http request in buf
    ret = snprintf(buf, buflen,
	"GET /%s/%s HTTP/1.1\r\n"
	"Host: %s\r\n"
	"Connection: close\r\n"
	"\r\n",
	type, id, host);

    if (ret < 0 || ret >= buflen)
        {
	ESP_LOGE(TAG, "host/path bad");
        return (-1);
        }

    // httpGet() returns the line to parse in buf
    if (httpGet(host, buf, buf, BUF_SIZE) < 0)
        {
	DEBUG("httpGet() failed\n");
	exit (-1);
	}

    // Sample response:
    // {"id":"sensor-net_wifi_signal_","value":-84,"state":"-84 dBm"}

    // find the optional value: string
    if ((p = strstr (buf, "\"value\":")) != NULL)
	{
	value = p + 8; // skip past 8-byte "value": token

	if ((p = strstr (value, ",")) == NULL)
	    {
	    printf("httpGet esphome value end not found\n");
	    exit (-1);
	    }
	*p++ = 0; // NULL terminate the value: string

	// remove double quotes
	if (*value == '\"')
	    {
	    memmove(value, value + 1, strlen(value)+1);
	    value[strlen(value)-1] = 0;
	    }

	DEBUG("Value of %s %s is %s\n", type, id, value);
	}
    else
	p = buf; // no value:, start again

    // find the state: string
    if ((p = strstr (p, "\"state\":")) == NULL)
        {
	printf("httpGet esphome state start not found\n");
	exit (-1);
	}

    state = p + 8; // skip past 8-byte "state": token

    if (((p = strstr (state, ",")) == NULL) &&
	((p = strstr (state, "}")) == NULL))
        {
	printf("httpGet esphome state end not found\n");
	exit (-1);
	}

    *p++ = 0; // NULL terminate the state: string

    // remove double quotes
    if (*state == '\"')
	{
	memmove(state, state + 1, strlen(state)+1);
	state[strlen(state)-1] = 0;
	}

    DEBUG("State of %s %s is %s\n", type, id, state);

    // return <state> in the passed buffer
    strcpy (buf, state);

    return (0);
    }

// This routine calls cb() once for each entity dumped by the server
int httpGetIds
    (
    char *host,
    char *buf,
    int buflen,
    HTTP_GETIDS_CB cb
    )
    {
    char *p;
    int clen, sd, ret, numPings = 0;
    char *type, *id, *name, *value, *state;

    ret = snprintf(buf, buflen,
	"GET /events HTTP/1.1\r\n"
	"Host: %s\r\n"
	"Connection: close\r\n"
	"\r\n",
	host);

    if (ret < 0 || ret >= buflen)
        {
	ESP_LOGE(TAG, "host/path bad");
        return (-1);
        }

    if ((sd = httpConnect (host, buf)) < 0)
        {
	ESP_LOGE(TAG, "httpConnect failed");
        return (-1);
        }

    DEBUG("... Parsing responses:\n");

    // Only the first line is of interest:
    // data: {"id":"sensor-net_wifi_signal_","name":"Net WiFi Signal ","icon":"","entity_category":2,"value":-76,"state":"-76 dBm","sorting_weight":50,"uom":"dBm"}
    // data: {"id":"sensor-net_wifi_signal_","value":-78,"state":"-78 dBm"}
    // event: ping
    while (1)
        {
        if (readHttpLine (sd, buf, buflen) < 0)
            return (-1);
	// DEBUG("<start>%s<end>\n",buf);
	if (strncmp (buf, "event: ping", 11) == 0)
	    {
	    numPings ++;
	    continue;
	    }
	if (numPings == 2)
	    break; // we are done parsing the entity dump

        type = id = name = value = state = "";

        if (strncmp (buf, "data: {\"id\":\"", 13) == 0)
	    {
	    // snarf the entity type 
	    type = buf + 13; // skip 13 bytes: data: {"id":"
	    if ((p =  strstr(type,"-")) == NULL)
	        goto parseError;
	    *p++ = 0;

	    // snarf the entity id 
	    id = p;
	    if ((p = strstr(p,"\"")) == NULL)
	        goto parseError;
	    *p++ = 0;

	    // snarf the entity name 
	    if ((p =  strstr(p,"\"name\":\"")) == NULL)
	        {
		// value update, not an initial dump line
		numPings++;
		continue;
		}
	    name = p + 8; // skip 8 bytes: "name":"
	    if ((p =  strstr(name,"\"")) == NULL)
	        goto parseError;
	    *p++ = 0;

	    // snarf the optional entity value
	    if ((p =  strstr(p,"\"value\":")) != NULL)
		{
		value = p + 8; // skip 8 bytes: "value":
		if ((p =  strstr(value,",")) == NULL)
		    goto parseError;
		*p++ = 0;
	        } else
		    p = name + strlen(name) + 1; // restart search after name

	    // snarf the entity state
	    if ((p =  strstr(p,"\"state\":")) == NULL)
	        goto parseError;
	    state = p + 8; // skip 8 bytes: "state":
	    if (((p =  strstr(state,",")) == NULL) &&
	        ((p =  strstr(state,"}")) == NULL))
	        goto parseError;
	    *p++ = 0;

	    // remove double quotes from value and state
	    if (*value == '\"')
		{
		memmove(value, value + 1, strlen(value)+1);
		value[strlen(value)-1] = 0;
		}
	    if (*state == '\"')
	        {
		memmove(state, state + 1, strlen(state)+1);
		state[strlen(state)-1] = 0;
		}

	    if (cb != NULL)
		cb (type, id, name, value, state);
	    }
        }

    close (sd);
    return (0);

parseError:
    DEBUG ("... parse error at:%s\n", buf);
    DEBUG("type=\"%s\", id=\"%s\", name=\"%s\", value=\"%s\", state=\"%s\"\n",
			    type, id, name, value, state);
    close (sd);
    return (-1);
    }

int httpPost
    (
    char *host,
    char *type,
    char *id,
    char *method,
    char *buf,
    int buflen
    )
    {
    char *p;
    int sd, ret;
    char *value, *state;

    // construct initial http request in buf
    ret = snprintf(buf, buflen,
	"POST /%s/%s/%s HTTP/1.1\r\n"
	"Host: %s\r\n"
	"Content-Length: 0\r\n"
	"Connection: close\r\n"
	"\r\n",
	type, id, method, host);

    if (ret < 0 || ret >= buflen)
        {
	ESP_LOGE(TAG, "host/path bad");
        return (-1);
        }

    // we use httpGet() to post.  It doesn't mind.
    if (httpGet(host, buf, buf, BUF_SIZE) < 0)
        {
	DEBUG("httpGet() failed\n");
	exit (-1);
	}

    return (0);
    }

#ifdef unix
// called with type, id, name, value, state character strings
static void getids_cb(char *t, char *i, char *n, char *v, char *s)
    {
    printf("%s %s (%s) %s", t, i, n, v);    // type, id, name, value
    if (strcmp(s, v) != 0)
	printf(" (%s)", s);		    // state
    printf("\n");
    }

#ifndef WEBAPI_NO_MAIN // this file gets included by tcpcmd.c unix test binary
int main(int argc, char * argv[])
    {
    char *p, *value, *state, buffer[BUF_SIZE];

    TAG = argv[0];

    if (argc > 1 && strcmp (argv[1], "-d") == 0)
	{
	_debug = 1;
	argv++, argc--;
	} 

    if (argc == 2) // assume we are listing all objects
        {
        if (httpGetIds(argv[1], buffer, BUF_SIZE, getids_cb) < 0)
	    {
	    printf("httpGetIds() failed\n");
	    exit (-1);
	    }
	exit (0);
	}

    if (argc == 4) // get the value of a specific id
        {
        if (httpGetState(argv[1], argv[2], argv[3], buffer, BUF_SIZE) < 0)
	    {
	    printf("httpGetState() failed\n");
	    exit (-1);
	    }
	exit (0);
	}

    if (argc == 5) // use a method to change an entity value and state
        {
        if (httpPost(argv[1], argv[2], argv[3], argv[4], buffer, BUF_SIZE) < 0)
	    {
	    printf("httpPost() failed\n");
	    exit (-1);
	    }
	exit (0);
	}

    printf("Usage:\n"
	"Turn on some debug info with -d\n"
	"Get list of entities: %s -d <host>\n"
	"Get state of entity:  %s <host> <domain> <id>\n"
	"Change entity using methoed:  %s <host> <domain> <id> <method>\n"
	"Examples:\n"
	"%s 192.168.248.20\n"
	"%s 192.168.248.20 sensor net_wifi_signal_\n"
	"%s 192.168.248.20 switch cpu1_enable turn_on\n"
	"%s esphome6 light room2 \'turn_on?brightness=128&transition=2\'\n"
	, TAG, TAG, TAG, TAG, TAG, TAG, TAG);

    exit (-1);
    }
#endif // !NO_MAIN
#endif
