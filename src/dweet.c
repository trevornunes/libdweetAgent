/*
 * dweet.c
 *
 *  Created on: Nov 28, 2014
 *      Author: tnunes
 */

#include <curl/curl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cJSON.h"
#include "dweet.h"
#define HAVE_PTHREAD 1


#ifdef HAVE_PTHREAD
#include <pthread.h>

// simple dumb thread serializer in case we are executed by N threads at the same time:
pthread_mutex_t  gDweetMutex = PTHREAD_MUTEX_INITIALIZER;
#define MLOCK pthread_mutex_lock( &gDweetMutex );
#define MUNLOCK pthread_mutex_unlock( &gDweetMutex );

#else
#define MLOCK
#define MUNLOCK
#endif


// TBC
int is_network_available() { return 1; };

// char *testUrl = "https://dweet.io/get/latest/dweet/for/my-thing-name";
//{"this":"succeeded","by":"dweeting","the":"dweet","with":{"thing":"tbot","created":"2014-12-01T22:55:55.047Z","content":{"sensor":"\"155\""}}}

char *dweet_get_item_value(char *s, const char *itemName);

struct CurlBuffer
{
    char *payload;
    size_t size;
};

struct dweetInfo
{
    int is_connected;
    int is_initialized;
    int debug_enabled;
    int curl_error;
    char url[4096];
    int chunked_mode;  // HTTP chunked mode.
    int skip_verify_peer;
    int skip_verify_host;
    struct CurlBuffer *response;
};

static struct dweetInfo Info; // writeback buffer for CURL and some other information.
static dweetEntry_t sDweet; // default container.
static dweetEntry_t Dweet; // the dweet container we will push out.

void dweetEnableDebug(int enableDisable)
{
    Info.debug_enabled = enableDisable;
}

void dweetSetup(dweetEntry_t dweetCfg)
{
    memcpy(&sDweet, &dweetCfg, sizeof(dweetEntry_t));
}


void dweetChunkedHTTP(int onOff)
{
    if(onOff)
    {
        Info.chunked_mode = 1;
    } else {
        Info.chunked_mode = 0;
    }
}
void dweetCurlSecurityCheck(int onOff)
{
    Info.skip_verify_host = ~onOff;
    Info.skip_verify_peer = ~onOff;

}


void dweetInit(const char *theThing)
{
    DBGTRACK
    MLOCK

    Info.is_connected = 0;
    Info.response = 0;
    Info.debug_enabled = 0;
    Info.skip_verify_host =1;  // you can re-enable SSL/CA checks with dweetCurlSecurityCheck(1); 
    Info.skip_verify_peer =1;  
	  Info.chunked_mode = 1;     // you can disable HTTP chunk mode with dweetChunkedHTTP(0);

    if (!theThing) {
        return;
    }

    // Already configured, lets destroy old stuff and reconfig.Delete old objects, so we can re-initialize it.
    if(Info.is_initialized)
    {
      free(Dweet.getURL);
      Dweet.getURL = 0;
      free(Dweet.putURL);
      Dweet.putURL = 0;
    }

    if( !Dweet.getURL) {
        memset(&(Info.url[0]), 0, 1024);
        char *putPtr = (char *) malloc(1024);
        sprintf(putPtr, "https://dweet.io/dweet/for/%s?", theThing);
        char *getPtr = (char *) malloc(1024);
        sprintf(getPtr, "https://dweet.io/get/latest/dweet/for/%s", theThing);
        Dweet.getURL = getPtr;
        Dweet.putURL = putPtr;
    }

    Info.is_initialized = 1;
    MUNLOCK

}

void dweetFlush()
{
    if (Info.response) {
        if (Info.response->payload) {
            MLOCK
            free(Info.response->payload);
            MUNLOCK
        }
    }

}

size_t writemem_cb(void *ptr, size_t size, size_t nmemb, void *stream)
{
    size_t realsize = size * nmemb;
    Info.response = (struct CurlBuffer *) stream;

    if (Info.debug_enabled) {
        fprintf(stderr, "writemem_cb: cURL data size=%ld elems=%ld, malloc %ld bytes \n", size, nmemb,
                (Info.response->size + realsize + 1));
    }

    Info.response->payload = (char *) malloc(Info.response->size + realsize + 1);
    if (Info.response->payload == NULL) {
        return 0;
    }

    if (Info.debug_enabled) {
        fprintf(stderr, "writemem_cb: copy %ld bytes \n", realsize);
    }

    memcpy(&(Info.response->payload[0]), ptr, realsize);

    Info.response->size = realsize;
    Info.response->payload[Info.response->size] = '\0';

    return realsize;
}

char *dweetGetItemContentURL(char *url, char *item)
{
    DBGTRACK

    if (!url) {
        return "error URL";
    }

    if (!item)
        return "error item";

    dweetConnect(url);
    return dweetGetItemContent(item);

}

cJSON *dweetGetPayloadJSON()
{
    if (Info.curl_error) {
        return 0;
    }

    if( Info.response->payload) {
      cJSON *jreq = cJSON_Parse(Info.response->payload);
      if (jreq) {
        return jreq; // user needs to delete this object.
      }
    }

    return 0;
}

char *dweetGetPayloadStr()
{
    if (Info.curl_error) {
        return NULL;
    }

    if( Info.response->payload) {
         return (Info.response->payload);
		 }

    else {
      if( Info.debug_enabled ) {
         return NULL;
	    }
    }
	 return NULL;
}

char *dweetGetItemContent(char *item) // get the last dweet message 'item' value.
{
    DBGTRACK
    cJSON *json_req = NULL;
    cJSON *json_item = NULL;
    char *itemString = NULL;
    int szArray = 0;

    dweetConnect(Dweet.getURL);

    if (!item)
        return "error";

    if (Info.curl_error) {
        return "";
    }

    if( !Info.response->payload ) {
        return "";
    }

    json_req = cJSON_Parse(Info.response->payload);
    szArray = cJSON_GetArraySize(json_req);

    if (Info.debug_enabled)
        fprintf(stderr, "dweetGetItemContent: array-size %d \n", szArray);

/*
    if (szArray > 1) {
        fprintf(stderr, "dweetGetItemContent: Handle JSON array here ...\n");
    }
*/

    json_item = cJSON_GetObjectItem(json_req, item);
    if (Info.debug_enabled) {
        fprintf(stderr, "dweet_getLastItem: %s = '%s'\n", item, cJSON_Print(json_item));
	  }
    itemString = cJSON_Print(json_item);
    return (itemString);
}


const char *dweetGetItemVal(const char *itemName)
{
    cJSON *json_resp;
    cJSON *json_item;
    cJSON *json_content;
    cJSON *json_payload;
    char *itemStr = 0;
    char *itemMatchPayload = 0;

    static char *errorStr = "error";

   // cJSON *json_item_content;

    if (itemName) {
        json_resp = dweetGetPayloadJSON();
        if(!json_resp) {
            return errorStr;
        }

        json_item = cJSON_GetObjectItem(json_resp, "with");

        if(json_item) {
            itemStr = cJSON_Print(json_item);
            itemStr[0] = ' ';   // need to strip [] brackets from dweet.io content.
            itemStr[strlen(itemStr)-1] = ' ';
            json_content= cJSON_Parse(itemStr);  // JSON block 'with' and no square brackets ...
            // Now we can extract payload correctly without crashing cJSON ...
            json_payload = cJSON_GetObjectItem(json_content, "content");
            cJSON_Delete(json_content);
            cJSON_Delete(json_item);
            itemMatchPayload = cJSON_Print( cJSON_GetObjectItem(json_payload, itemName) );
            if(itemMatchPayload) {
                return itemMatchPayload;
            } else {
                return errorStr;
            }
        }
    }

    return errorStr;
}

const char *dweetGetCurlStatusStr()
{
    return curl_easy_strerror(Info.curl_error);
}

int dweetRequestOkay()
{
    if (Info.curl_error == 0)
        return 1;
    else {
        return -1;
    }
}
/*
 *
 */
char *dweet_get_item_value(char *s, const char *itemName)
{
    DBGTRACK
    int i=0;

    cJSON *json_req = NULL;
    cJSON *json_resp = NULL;

    char *itemString = NULL;

    if (!itemName)
        return "error";

    if (Info.curl_error) {
        return NULL;
    }

    json_req = cJSON_Parse(s);
    cJSON *item = cJSON_GetObjectItem(json_req,"content");

    if(!item)
        return "error";

    for (i = 0 ; i < cJSON_GetArraySize(item) ; i++)
    {
       cJSON * subitem = cJSON_GetArrayItem(item, i);
             json_resp = cJSON_GetObjectItem(subitem, itemName );
            itemString = cJSON_Print(json_resp);
    }

    json_req   = cJSON_Parse(s);
    json_resp  = cJSON_GetObjectItem( json_req, itemName);
    itemString = cJSON_Print(json_resp);

    if (Info.debug_enabled) {
        printf("dweet_get_item_value(%s) returns %s\n", itemName, itemString);
	  }

     cJSON_Delete(json_req);
     cJSON_Delete(json_resp);

    return itemString;
}




int dweetPushThis(const char *newData)
{
    static char itemUrl[2048];

    if(!newData) {
       return -1;
    }

    if (is_network_available()) {
        memset( &itemUrl[0], 0, 2048);
        sprintf(&itemUrl[0], "%s%s", Dweet.putURL, newData);
        dweetConnect(itemUrl);
        dweetDisconnect();
        if (Info.debug_enabled) {
            fprintf(stderr, "dweetPutData: PUSH %s done.\n", newData);
			  }
        return 0;
    } else {
        return -1;
    }
}




void dweetPull()
{
    if (is_network_available()) {
        dweetConnect(Dweet.getURL); // caches the payload internally , so we can close the connection now.
        fprintf(stderr, "dweetPull: PULL '%s' from %s\n", dweetGetPayloadStr(), Dweet.getURL);
        dweetDisconnect();
    } else {
        printf("no network.\n");
    }
}




/*
 *
 *
 */
int dweetConnect(char *dweetUrl)
{
    struct curl_slist *chunk = NULL;

    if(!dweetUrl) {
        return -1;
    }

    CURL *curl;
    CURLcode res;

    if (!dweetUrl) {
        fprintf(stderr, "null/bad URL\n");
        return -1;
    }

    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();

    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, dweetUrl);

        if (Info.debug_enabled) {
            curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
         }

        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writemem_cb);
        curl_easy_setopt(curl, CURLOPT_MAXCONNECTS, 300 );

        // Chunked 'streamed HTML' without verifying data sizes == faster transfer
        if(Info.chunked_mode ) {
             chunk = curl_slist_append(chunk, "Transfer-Encoding: chunked");
             res = curl_easy_setopt(curl, CURLOPT_HTTPHEADER, chunk);
        }

        if (Info.skip_verify_peer)
            curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);

        if (Info.skip_verify_host)
            curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);


        // multiple threads could stil modify the dweetUrl, change the verify option on the fly etc.
        // we just want to ensure that the callback to dump the received payload and transmit to the
        // server is serialized ...

        MLOCK
        res = curl_easy_perform(curl);
        if(Info.chunked_mode ) {
            curl_slist_free_all(chunk);
        }
        MUNLOCK

        if (res != CURLE_OK) {
            Info.curl_error = res;
            curl_easy_cleanup(curl);
            return -1;
        } else {
            if (Info.debug_enabled) {
                Info.curl_error = 0;
            }
            return 0;
        }
    }
    return -1;
}

void dweetDisconnect()
{
    if (Info.debug_enabled) {
         fprintf(stderr, "dweetDisconnect\n");
    }
   curl_global_cleanup();
}

