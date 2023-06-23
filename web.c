#include "web.h"
#include "log.h"

#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include <curl/easy.h>
#include <math.h>

static size_t cb(void* data, size_t size, size_t nmemb, void* clientp) //stolen from curl.se
{
	size_t realsize = size * nmemb;
	struct __webresponse * mem = (struct __webresponse*)clientp;

	char* ptr = realloc(mem->data, mem->length + realsize + 1);

	if (ptr == NULL)
		return 0;

	mem->data = ptr;
	memcpy(&(mem->data[mem->length]), data, realsize);
	mem->length += realsize;
	mem->data[mem->length] = 0;

	return realsize;
}

static size_t pcb(void* data, double dltotal, double dlnow, double ultotal, double ulnow)
{
	if (!data)
		return 0;

	struct __webprogress* req = (struct __webprogress*)data;
	if (!req->callback)
		return 0;

	if (isnan(dlnow / dltotal))
		return 0;

	req->callback(req->data, dlnow / dltotal);
	
	return 0;
}

webrequest_t* web_init(void)
{
	webrequest_t* request = malloc(sizeof(webrequest_t));

	if (!request)
	{
		ILOG("Failed to allocate request body.");
		return NULL;
	}

	memset(request, 0, sizeof(webrequest_t));

	ILOG("Request 0x%p", (void*)request);
	return request;
}

void web_set_url(webrequest_t* request, const string url)
{
	if (!request)
		return;

	request->url = url;
}

void web_set_auth(webrequest_t* request, const string token)
{
	if (!request)
		return;

	request->auth = token;
}

void web_set_file(webrequest_t* request, const string filename)
{
	if (!request)
		return;

	request->file = fopen(filename, "wb");

	if (!request->file)
		EMSGBX("Failed to open file \"%s\" for downloading!", filename);
}

void web_set_progress(webrequest_t* request, webprogress_t func)
{
	if (!request)
		return;

	request->progress.callback = func;
}

void web_set_progress_data(webrequest_t* request, void* data)
{
	if (!request)
		return;

	request->progress.data = data;
}

void web_perform(webrequest_t* request)
{
	if (!request)
		return;

	struct curl_slist* headers = curl_slist_append(NULL, "Cache-Control: no-cache, no-store");
	headers = curl_slist_append(headers, "Pragma: no-cache");

	if (request->auth)
	{
#define AUTH_BUFFER_SIZE 1024

		char auth[AUTH_BUFFER_SIZE];

		snprintf(auth, AUTH_BUFFER_SIZE, "Authorization: Bearer %s", request->auth);
		headers = curl_slist_append(headers, auth);
	}

	CURL* curl = curl_easy_init();

	if (!curl)
	{
		request->error = WERROR_CURL_INIT_FAILED;
		return;
	}
	
	if (request->file)
	{
		headers = curl_slist_append(headers, "Accept: application/octet-stream");
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, NULL);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, request->file);
	}
	else
	{
		headers = curl_slist_append(headers, "Accept: */*");
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, cb);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void*)&request->response);
	}

	if (request->progress.callback)
	{
		curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0L);
		curl_easy_setopt(curl, CURLOPT_PROGRESSFUNCTION, pcb);
		curl_easy_setopt(curl, CURLOPT_PROGRESSDATA, (void*)&request->progress);
	}

	curl_easy_setopt(curl, CURLOPT_URL, request->url);
	curl_easy_setopt(curl, CURLOPT_USERAGENT, "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/112.0.0.0 Safari/537.36 OPR/98.0.0.0");
	curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
	curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
	curl_easy_setopt(curl, CURLOPT_FAILONERROR, 1L);

	ILOG("Requesting 0x%p...", (void*)request, request->url);
	request->errorCode = curl_easy_perform(curl);

	switch (request->errorCode)
	{
	case CURLE_OK:
		request->success = TRUE;
		ILOG("Request 0x%p ok!", (void*)request);
		break;

	default:
		request->success = FALSE;
		request->error = WERROR_CURL_REQUEST_FAILED;
		ILOG("Request 0x%p failed!", (void*)request);
		break;
	}

	curl_slist_free_all(headers);
	curl_easy_cleanup(curl);

	if(request->file)
		fclose(request->file);
}

const string web_geterror(webrequest_t* request)
{
	return curl_easy_strerror(request->errorCode);
}

void web_free(webrequest_t* request)
{
	if (!request)
		return;

	if (request->response.data)
	{
		ILOG("Request 0x%p body destroyed.", (void*)request->response.data);
		free(request->response.data);
		request->response.data = NULL;
	}

	ILOG("Request 0x%p destroyed.", (void*)request);
	free(request);
}
