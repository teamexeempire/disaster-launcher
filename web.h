#ifndef WEB_H
#define WEB_H
#include "types.h"
#include <curl/curl.h>

typedef void (*webprogress_t)(void*, double);
typedef struct
{
	string			url;
	string			auth; /* Auth token for github */
	FILE*			file;

	boolean			success;

	struct __webprogress
	{
		webprogress_t callback;
		void*		  data;
	} progress;

	struct __webresponse
	{
		int			code; /* Response code */

		byte_ptr    data;
		size_t      length;
	} response;

	enum
	{
		WERROR_OK,
		WERROR_CURL_INIT_FAILED,
		WERROR_CURL_REQUEST_FAILED,

	} error;
	CURLcode errorCode;

} webrequest_t;

webrequest_t* web_init				(void);
void		  web_set_url			(webrequest_t* request, const string url);
void		  web_set_auth			(webrequest_t* request, const string token);
void		  web_set_file			(webrequest_t* request, const string filename);
void		  web_set_progress		(webrequest_t* request, webprogress_t func);
void		  web_set_progress_data	(webrequest_t* request, void* data);
void		  web_perform			(webrequest_t* request);
const string  web_geterror			(webrequest_t* request);
void		  web_free				(webrequest_t* request);

#endif