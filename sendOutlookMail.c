#include <stdio.h>
#include <stdlib.h>
//#include "include/curl/curl.h"
//#include "<curl/curl.h>"

#define FROM_ADDR    "remetente@org.com"
#define TO_ADDR      "destinatario@org.com"
#define CC_ADDR      "copia@org.com"

//#define FROM_MAIL "Sender Person " FROM_ADDR
//#define TO_MAIL   "A Receiver " TO_ADDR
//#define CC_MAIL   "Copy" CC_ADDR

static const char *payload_text =
  "Date: Thu, 28 Mar 2024 08:27:30 +1100\r\n"
  "To: " TO_ADDR "\r\n"
  "From: " FROM_ADDR "\r\n"
  "Cc: " CC_ADDR "\r\n"
  "Message-ID: <dcd7cb36-11db-487a-9f3a-e652a9458efd@"
  "teste.outlook.org>\r\n"
  "Subject: Teste envio mensagem\r\n"
  "\r\n" /* empty line to divide headers from body, see RFC 5322 */
  "Ola este email foi envido por JOHN PETER .\r\n"
  "\r\n"
  "Foi utilizado a linguagem C com a lib curl.\r\n";

struct upload_status {
  size_t bytes_read;
};

static size_t payload_source(char *ptr, size_t size, size_t nmemb, void *userp) {
  struct upload_status *upload_ctx = (struct upload_status *)userp;
  const char *data;
  size_t room = size * nmemb;

  if((size == 0) || (nmemb == 0) || ((size*nmemb) < 1)) {
    return 0;
  }

  data = &payload_text[upload_ctx->bytes_read];

  if(data) {
    size_t len = strlen(data);
    if(room < len)
      len = room;
    memcpy(ptr, data, len);
    upload_ctx->bytes_read += len;

    return len;
  }

  return 0;
}

int main(void)
{
  CURL *curl;
  CURLcode res = CURLE_OK;
  struct curl_slist *recipients = NULL;
  struct upload_status upload_ctx = { 0 };

  curl = curl_easy_init();
  if(curl) {
    /* Set username and password */
    curl_easy_setopt(curl, CURLOPT_USERNAME, "user");
    curl_easy_setopt(curl, CURLOPT_PASSWORD, "secret");


    curl_easy_setopt(curl, CURLOPT_URL, "smtp://smtp-mail.outlook.com:587");

    curl_easy_setopt(curl, CURLOPT_USE_SSL, CURLUSESSL_ALL);
    
    curl_easy_setopt(curl, CURLOPT_USE_SSL, (long)CURLUSESSL_ALL);

    curl_easy_setopt(curl, CURLOPT_CAINFO, "./cacert.pem");

    curl_easy_setopt(curl, CURLOPT_MAIL_FROM, FROM_ADDR);

    recipients = curl_slist_append(recipients, TO_ADDR);
    recipients = curl_slist_append(recipients, CC_ADDR);
    curl_easy_setopt(curl, CURLOPT_MAIL_RCPT, recipients);

    curl_easy_setopt(curl, CURLOPT_READFUNCTION, payload_source);
    curl_easy_setopt(curl, CURLOPT_READDATA, &upload_ctx);
    curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);
    
    curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);

    /* Send the message */
    res = curl_easy_perform(curl);

    /* Check for errors */
    if(res != CURLE_OK)
      fprintf(stderr, "curl_easy_perform() failed: %s\n",
              curl_easy_strerror(res));

    /* Free the list of recipients */
    curl_slist_free_all(recipients);

    /* Always cleanup */
    curl_easy_cleanup(curl);
  }

  return (int)res;
}
