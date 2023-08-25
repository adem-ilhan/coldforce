#include <coldforce/core/co_std.h>

#include <coldforce/tls/co_tls_client.h>

#include <coldforce/http/co_http_tcp_extension.h>
#include <coldforce/http/co_http_server.h>

//---------------------------------------------------------------------------//
// tcp extension for http
//---------------------------------------------------------------------------//

//---------------------------------------------------------------------------//
//---------------------------------------------------------------------------//

//---------------------------------------------------------------------------//
// private
//---------------------------------------------------------------------------//

bool
co_tcp_upgrade_to_http_connection(
    co_tcp_client_t* tcp_client,
    co_http_connection_t* conn,
    const char* url_origin
)
{
#ifdef CO_CAN_USE_TLS
    if (tcp_client->sock.tls != NULL)
    {
        conn->module.destroy = co_tls_client_destroy;
        conn->module.close = co_tls_close;
        conn->module.connect = co_tls_connect;
        conn->module.send = co_tls_send;
        conn->module.receive_all = co_tls_receive_all;
    }
    else
    {
#endif
        conn->module.destroy = co_tcp_client_destroy;
        conn->module.close = co_tcp_close;
        conn->module.connect = co_tcp_connect;
        conn->module.send = co_tcp_send;
        conn->module.receive_all = co_tcp_receive_all;

#ifdef CO_CAN_USE_TLS
    }
#endif

    co_url_st* url = NULL;

    if (url_origin != NULL)
    {
        url = co_url_create(url_origin);

        if (url == NULL)
        {
            return false;
        }
    }

    conn->tcp_client = tcp_client;
    conn->tcp_client->sock.sub_class = conn;
    conn->url_origin = url;
    conn->receive_data.index = 0;
    conn->receive_data.ptr = co_byte_array_create();
    conn->receive_timer = NULL;

    return true;
}

//---------------------------------------------------------------------------//
// public
//---------------------------------------------------------------------------//

co_http_client_t*
co_tcp_upgrade_to_http(
    co_tcp_client_t* tcp_client,
    const char* url_origin
)
{
    co_http_client_t* client =
        (co_http_client_t*)co_mem_alloc(sizeof(co_http_client_t));

    if (client == NULL)
    {
        return NULL;
    }

    if (!co_tcp_upgrade_to_http_connection(
        tcp_client, &client->conn, url_origin))
    {
        co_mem_free(client);

        return NULL;
    }

    co_http_client_setup(client);

    if (co_http_connection_is_server(&client->conn))
    {
        client->conn.tcp_client->callbacks.on_receive =
            (co_tcp_receive_fn)co_http_server_on_tcp_receive_ready;
        client->conn.tcp_client->callbacks.on_close =
            (co_tcp_close_fn)co_http_server_on_tcp_close;
    }
    else
    {
        client->conn.tcp_client->callbacks.on_receive =
            (co_tcp_receive_fn)co_http_client_on_tcp_receive_ready;
        client->conn.tcp_client->callbacks.on_close =
            (co_tcp_close_fn)co_http_client_on_tcp_close;
    }

    return client;
}
