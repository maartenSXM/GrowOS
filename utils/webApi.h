#ifndef __INCLUDE_WEB_API_H__
#define __INCLUDE_WEB_API_H__

#ifdef __cplusplus
extern "C" {
#endif

// The state on <host> of entity <type>/<id> is returned in buf
// <buf> should be longer than the longest line returned by the web server
extern int httpGetState
    (
    char *host,
    char *type,
    char *id,
    char *buf,
    int buflen
    );

// Callback parameters are all strings: type, id, name, value, state
typedef void (*HTTP_GETIDS_CB)(char *t, char *i, char *n, char *v, char *s);

// <cb> is called once for each entity on <host>
// <buf> should be longer than the longest line returned by the web server
extern int httpGetIds
    (
    char *host,
    char *buf,
    int buflen,
    HTTP_GETIDS_CB cb
    );

// Apply <method> to entity <type>/<id> on <host>
// as defined here: https://esphome.io/web-api
// <buf> should be longer than the longest line returned by the web server
extern int httpPost
    (
    char *host,
    char *type,
    char *id,
    char *method,
    char *buf,
    int buflen
    );

#ifdef __cplusplus
}
#endif

#endif // __INCLUDE_WEB_API_H__
