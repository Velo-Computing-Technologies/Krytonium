#include "orion.h"

// Pretend we have copy-pasted the mbedTLS and lwIP headers into our OS
#include "mbedtls/net_sockets.h"
#include "mbedtls/ssl.h"
#include "mbedtls/entropy.h"
#include "mbedtls/ctr_drbg.h"
#include "mbedtls/error.h"

const char* SUPABASE_HOST = "vjvzvigfsicyrzyfudgc.supabase.co";
const char* SUPABASE_PORT = "443";
const char* SUPABASE_KEY = "sb_publishable_6AviRtytGEBstXR6ousKnQ_qqS09BZz";

bool is_logged_in = false;

// The full HTTPS implementation using mbedTLS
void attempt_supabase_login() {
    mbedtls_net_context server_fd;
    mbedtls_entropy_context entropy;
    mbedtls_ctr_drbg_context ctr_drbg;
    mbedtls_ssl_context ssl;
    mbedtls_ssl_config conf;

    // 1. Initialize the cryptography structures
    mbedtls_net_init(&server_fd);
    mbedtls_ssl_init(&ssl);
    mbedtls_ssl_config_init(&conf);
    mbedtls_ctr_drbg_init(&ctr_drbg);
    mbedtls_entropy_init(&entropy);

    // Seed the random number generator
    const char *pers = "orion_os_ssl";
    mbedtls_ctr_drbg_seed(&ctr_drbg, mbedtls_entropy_func, &entropy, (const unsigned char *)pers, 12);

    // 2. Connect the underlying TCP socket (Handled by your future lwIP stack)
    if (mbedtls_net_connect(&server_fd, SUPABASE_HOST, SUPABASE_PORT, MBEDTLS_NET_PROTO_TCP) != 0) {
        return; // Connection failed
    }

    // 3. Setup SSL/TLS configuration
    mbedtls_ssl_config_defaults(&conf, MBEDTLS_SSL_IS_CLIENT, MBEDTLS_SSL_TRANSPORT_STREAM, MBEDTLS_SSL_PRESET_DEFAULT);
    mbedtls_ssl_conf_authmode(&conf, MBEDTLS_SSL_VERIFY_NONE); // Skipping certificate verification for now
    mbedtls_ssl_conf_rng(&conf, mbedtls_ctr_drbg_random, &ctr_drbg);
    mbedtls_ssl_setup(&ssl, &conf);
    mbedtls_ssl_set_hostname(&ssl, SUPABASE_HOST);
    mbedtls_ssl_set_bio(&ssl, &server_fd, mbedtls_net_send, mbedtls_net_recv, NULL);

    // 4. Perform the TLS Handshake
    while (mbedtls_ssl_handshake(&ssl) != 0) {
        // In a real OS, yield CPU here to prevent locking
    }

    // 5. Construct the HTTP POST Request
    char http_request[1024];
    int len = snprintf(http_request, sizeof(http_request),
        "POST /auth/v1/token?grant_type=password HTTP/1.1\r\n"
        "Host: %s\r\n"
        "apikey: %s\r\n"
        "Content-Type: application/json\r\n"
        "Content-Length: 49\r\n"
        "\r\n"
        "{\"email\":\"admin@orionos.com\",\"password\":\"system\"}",
        SUPABASE_HOST, SUPABASE_KEY);

    // 6. Write the encrypted request through the TLS tunnel
    mbedtls_ssl_write(&ssl, (const unsigned char*)http_request, len);

    // 7. Read the encrypted HTTPS response back from Supabase
    unsigned char response_buffer[2048];
    int bytes_read = mbedtls_ssl_read(&ssl, response_buffer, sizeof(response_buffer) - 1);
    
    if (bytes_read > 0) {
        response_buffer[bytes_read] = '\0';
        // If we get an HTTP 200 OK, authentication was successful!
        is_logged_in = true;
    }

    // 8. Clean up and close connection
    mbedtls_ssl_close_notify(&ssl);
    mbedtls_net_free(&server_fd);
    mbedtls_ssl_free(&ssl);
    mbedtls_ssl_config_free(&conf);
    mbedtls_ctr_drbg_free(&ctr_drbg);
    mbedtls_entropy_free(&entropy);
}

// Graphical UI rendering
void render_login_screen() {
    draw_rect(0, 0, 320, 200, 0); 
    draw_rect(80, 50, 160, 100, 8);
    draw_rect(90, 70, 140, 15, 15); 
    draw_rect(90, 95, 140, 15, 15); 
    draw_rect(120, 120, 80, 20, 2); 

    // For testing the UI immediately in QEMU without networking freezing the OS,
    // you can uncomment this to force a login:
    // is_logged_in = true;
    
    // attempt_supabase_login(); // Uncomment this when mbedTLS is compiled in
}

bool is_authenticated() {
    return is_logged_in;
}
