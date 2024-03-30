#include <stdio.h>
#include <stdlib.h>
#include "include/curl/curl.h"
//#include <curl/curl.h>
#include <time.h>
#include <string.h>

#define FROM_ADDR    "sender@outlook.com"
#define TO_ADDR      "recipient@org.com"
#define CC_ADDR      "copyEmail@org.com"

int codigo[6];
int i;
char codigo_string[7];
int codigo_int;
int digito_aleatorio;
int resposta;

void generateCode(){
    srand(time(NULL));

    // Gerando o código aleatório
    for (i = 0; i < 6; i++)
    {
        digito_aleatorio = rand() % 10;
        codigo[i] = digito_aleatorio;
    }

    // Convertendo o vetor para string
    for (i = 0; i < 6; i++)
    {
        codigo_string[i] = codigo[i] + '0';
    }
    codigo_string[6] = '\0';

    // Convertendo a string para inteiro
    codigo_int = atoi(codigo_string);
}

void sendMail(){
  generateCode();
  char *payload_text;

  sprintf(payload_text,
          "Date: Thu, 29 Mar 2024 19:51:00 +1100\r\n"
          "To: %s\r\n"
          "From: %s\r\n"
          "Cc: %s\r\n"
          "Subject: Autenticacao 2 fatores\r\n"
          "\r\n"
          "Código de autenticação: %d\r\n"
          "\r\n",
          TO_ADDR, FROM_ADDR, CC_ADDR, sum);

  struct upload_status {
    size_t bytes_read;
  };

  size_t payload_source(char *ptr, size_t size, size_t nmemb, void *userp) {
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

  CURL *curl;
  CURLcode res = CURLE_OK;
  struct curl_slist *recipients = NULL;
  struct upload_status upload_ctx = { 0 };

  curl = curl_easy_init();
  if(curl) {
    /* Set username and password */
    curl_easy_setopt(curl, CURLOPT_USERNAME, "yourEmail@outlook.com");
    curl_easy_setopt(curl, CURLOPT_PASSWORD, "yourPassword");


    curl_easy_setopt(curl, CURLOPT_URL, "smtp://smtp-mail.outlook.com:587");

    curl_easy_setopt(curl, CURLOPT_USE_SSL, (long)CURLUSESSL_ALL);

    /* If you don't have a certificate, use this
     curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
     curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
     */
    
    /* Use this, if you have a certificate, 
     curl_easy_setopt(curl, CURLOPT_CAINFO, "path/to/certified.pem"); ./cacert.pem
    */
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


int main(void) {

    sendMail();
    char resposta[7];
    printf("\n%s\n", codigo_string);

    printf("\Informe o codigo: ");
    fgets(resposta, sizeof(resposta), stdin);

    printf("\n%s\n", resposta);

    if(strcmp(resposta, codigo_string) == 0){
        printf("\nValidacao concluida, autenticacao liberada \n");
    } else{
        printf("Codigo diferente, autenticacao negada");
    }

    return 0;
}
