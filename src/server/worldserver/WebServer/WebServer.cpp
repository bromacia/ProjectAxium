#include "WebServer.h"
#include "mongoose.h"
#include "World.h"

//@todo: Add json lib

static int index_html(struct mg_connection *conn) {
    mg_printf_data(conn, "World server uptime: %u", sWorld->GetUptime());

    return 0;
}

void WebServer::run()
{
    struct mg_server *server;

    // Create and configure the server
    server = mg_create_server(NULL);
    mg_set_option(server, "listening_port", "8080");
    mg_add_uri_handler(server, "/", index_html);

    // Serve request
    printf("Starting WebServer on port %s\n", mg_get_option(server, "listening_port"));
    for (;;) {
        mg_poll_server(server, 1000);
    }

    // Cleanup, and free server instance
    mg_destroy_server(&server);
}
