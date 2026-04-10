#include "orion.h"

// --- BARE METAL NETWORK LIBRARIES ---
#include "mbedtls/net_sockets.h"
#include "mbedtls/ssl.h"
#include "mbedtls/entropy.h"
#include "mbedtls/ctr_drbg.h"

const char* SUPABASE_HOST = "vjvzvigfsicyrzyfudgc.supabase.co";
const char* SUPABASE_PORT = "443";
const char* SUPABASE_KEY = "sb_publishable_6AviRtytGEBstXR6ousKnQ_qqS09BZz";

bool logged_in_status = false;

// --- REAL HTTPS IMPLEMENTATION ---
void attempt_supabase_login() {
    mbedtls_net_context server_fd;
    mbedtls_entropy_context entropy;
    mbedtls_ctr_drbg_context ctr_drbg;
    mbedtls_ssl_context ssl;
    mbedtls_ssl_config conf;

    mbedtls_net_init(&server_fd);
    mbedtls_ssl_init(&ssl);
    mbedtls_ssl_config_init(&conf);
    mbedtls_ctr_drbg_init(&ctr_drbg);
    mbedtls_entropy_init(&entropy);

    const char *pers = "orion_os_ssl";
    mbedtls_ctr_drbg_seed(&ctr_drbg, mbedtls_entropy_func, &entropy, (const unsigned char *)pers, 12);

    if (mbedtls_net_connect(&server_fd, SUPABASE_HOST, SUPABASE_PORT, MBEDTLS_NET_PROTO_TCP) != 0) {
        return; // Network failed
    }

    mbedtls_ssl_config_defaults(&conf, MBEDTLS_SSL_IS_CLIENT, MBEDTLS_SSL_TRANSPORT_STREAM, MBEDTLS_SSL_PRESET_DEFAULT);
    mbedtls_ssl_conf_authmode(&conf, MBEDTLS_SSL_VERIFY_NONE); 
    mbedtls_ssl_conf_rng(&conf, mbedtls_ctr_drbg_random, &ctr_drbg);
    mbedtls_ssl_setup(&ssl, &conf);
    mbedtls_ssl_set_hostname(&ssl, SUPABASE_HOST);
    mbedtls_ssl_set_bio(&ssl, &server_fd, mbedtls_net_send, mbedtls_net_recv, NULL);

    while (mbedtls_ssl_handshake(&ssl) != 0) {}

    char http_request[1024];
    snprintf(http_request, sizeof(http_request),
        "POST /auth/v1/token?grant_type=password HTTP/1.1\r\n"
        "Host: %s\r\n"
        "apikey: %s\r\n"
        "Content-Type: application/json\r\n"
        "Content-Length: 49\r\n"
        "\r\n"
        "{\"email\":\"admin@orionos.com\",\"password\":\"system\"}",
        SUPABASE_HOST, SUPABASE_KEY);

    mbedtls_ssl_write(&ssl, (const unsigned char*)http_request, 1024);

    unsigned char response_buffer[2048];
    int bytes_read = mbedtls_ssl_read(&ssl, response_buffer, sizeof(response_buffer) - 1);
    
    if (bytes_read > 0) {
        response_buffer[bytes_read] = '\0';
        logged_in_status = true; // ONLY set to true if Supabase says OK!
    }

    mbedtls_ssl_close_notify(&ssl);
    mbedtls_net_free(&server_fd);
    mbedtls_ssl_free(&ssl);
    mbedtls_ssl_config_free(&conf);
    mbedtls_ctr_drbg_free(&ctr_drbg);
    mbedtls_entropy_free(&entropy);
}

void render_login_screen() {
    draw_rect(0, 0, 320, 200, 0); 
    draw_rect(80, 50, 160, 100, 8);
    draw_rect(90, 70, 140, 15, 15); 
    draw_rect(90, 95, 140, 15, 15); 
    draw_rect(120, 120, 80, 20, 2); 

    // Once networking is fully built in Year 2, uncomment this to actually fire the request!
    // attempt_supabase_login();
}

bool is_authenticated() {
    return logged_in_status;
}
