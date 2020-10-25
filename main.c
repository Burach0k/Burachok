#include <stdio.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <openssl/bio.h>
#include <string.h>
#include <stdbool.h>
#include <errno.h>

#define HOST "api.telegram.org"
#define PORT "443"

typedef struct Params {
    char *text;
    bool help;
} Params;

Params* initParams(struct Params *params, int argc, char *argv[]) {
    params->help = false;

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--message") == 0 || strcmp(argv[i], "-M") == 0) {

            if (!argv[i + 1]) {
                printf("Некорректное сообщение");
                exit(2);
            }

            params->text = malloc(strlen(argv[i + 1]));
            params->text = argv[i + 1];
        }

        if (strcmp(argv[i], "--help") == 0 || strcmp(argv[i], "-H") == 0) {
            params->help = true;
        }
    }

    return params;
}

int main(int argc, char *argv[]) {

    BIO* bio;
    SSL* ssl;
    SSL_CTX* ctx;
    char *mesage;
    Params *params = malloc(sizeof(Params));

    if (argc < 2) {
        printf("Неверно введена команда, для справки используйте --help\n");
        exit(1);
    }

    params = initParams(params, argc, argv);

    if (params->help) {
        printf("Доступные команды: \n--message || -M: Сообщение для отправки\n");
    }

    SSL_library_init();

    ctx = SSL_CTX_new(SSLv23_client_method());

    if (ctx == NULL) {
        printf("Ctx is null\n");
    }

    bio = BIO_new_ssl_connect(ctx);

    BIO_set_conn_hostname(bio, HOST ":" PORT);

    if(BIO_do_connect(bio) <= 0) {
        printf("Failed connection\n");
        return 1;
    }

    if (params->text) {
        char *request = malloc(sizeof(char) * 300);
        char *req = "GET /bot1369784402:AAHGidEt_GrOGGVfXC3v_s-fx2IIXJ1pjJw/sendMessage?chat_id=288253273&text=";
        char *http = " HTTP/1.1\r\n";

        strcpy(request, req);
        strncat(request, params->text, strlen(params->text));
        strncat(request, http, strlen(http));

        char *host = "Host: " HOST "\r\n";
        char *connection = "Connection: close\r\n\r\n";

        char* write_buf = malloc(sizeof(char) * 600);
        strcpy(write_buf, request);
        strncat(write_buf, host, strlen(host));
        strncat(write_buf, connection, strlen(connection));

        if(BIO_write(bio, write_buf, strlen(write_buf)) <= 0) {
            printf("Failed write\n");
        }

        int size;
        char buf[1024];

        for(;;) {
            size = BIO_read(bio, buf, 1023);

            if(size <= 0) {
                break;
            }

            buf[size] = 0;

            printf("%s", buf);
        }


        free(request);
        free(write_buf);
    }

    BIO_free_all(bio);
    SSL_CTX_free(ctx);
    free(params);

    return 0;
}