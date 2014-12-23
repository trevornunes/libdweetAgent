
#ifndef __DWEET__
#define __DWEET__

#ifdef __cplusplus
extern "C"
{
#endif

#include "cJSON.h"
#include <stdio.h>

#define DBGTRACK fprintf(stderr,"DBGTRACK %s:%d\n",__FUNCTION__,__LINE__);
#define DBGPRINT(format, ...) \
    do { \
        fprintf(stderr,"%s:%d ",__FUNCTION__,__LINE__); \
        fprintf(stderr,(format, ##__VA_ARGS__); \
    } while(0)


typedef struct dweetEntry {
    char *getURL;     // https://dweet.io/get/latest/dweet/for/tbot
    char *putURL;     // https://dweet.io/for/tbot?
    char *putURLItem; // putURL + item...
    char *itemName;   // sensor
    int   value;      // [0-N]
} dweetEntry_t ;


// Setup
void  dweetInit(const char *theThing);              // setup root PUSH/PULL URLs to use for dweet.io
void  dweetSetup( dweetEntry_t dweetCfg );
void  dweetCurlSecurityCheck(int onOff);            // SSL,CA authenticate or skip ?
void  dweetChunkedHTTP(int OnOff);                  // Enable/Disable chunked HTTP headers

int   dweetConnect(char *url);                      // connect to dweet URL, ( store response )
void  dweetDisconnect();                            // disconnect URL
void  dweetFlush();                                 // delete the stored response from dweet.
int   dweetRequestOkay();                           // check if response contains "this:succeeded"
void  dweetEnableDebug(int enableDisable);          // 1 or 0, enable debug tracing or keep it quiet (default)
const char *dweetGetCurlStatusStr();                      // return description of error if any.



// Push
void dweetPutData(char *item, int value);           // send item=value with push URL
int  dweetPushThis(const char *newData);                  // send custom newData string with push URL , -1 if fails.

// Pull
void dweetPull();                                   // read/pull whatever thing is set in dweetInit getURL
const char *dweetGetItemVal(const char *itemName);              // e.g. "temperature" from a previous dweetPull or Push.

// Query/Read response.
char  *dweetGetItemContentURL(char *url, char*item);  // return value of a JSON item stored at URL
cJSON *dweetGetPayloadJSON();                         // get response as JSON.
char  *dweetGetPayloadStr();                          // get response as string..
char  *dweetGetItemContent(char *item);               // get an item value from JSON.


/*
 * PUSH example
 *
 *     dweetInit("mything");
 *     dweetPushThis("something=42");
 *     fprintf("%s", dweetGetPayloadStr() );  // the response.
 *
 * PULL example
 *
 *     dweetInit("mything");
 *     dweetPull();
 *     fprintf(stderr,"%s",dweetGetPayloadStr() ); // the response.
 *
 */

#ifdef __cplusplus
}
#endif

#endif

