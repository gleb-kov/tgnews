#ifndef TGSERVER_SERVER_H
#define TGSERVER_SERVER_H

#include <map>
#include <memory>

#include "net/io_api.h"
#include "net/address.h"
#include "net/timer.h"
#include "net/socket.h"
#include "net/storage.h"

#include "http/request.h"
#include "http/parser.h"

#include "sysapi.h"
#include "thread_pool.h"


#define ERRLOG_LVL 15
#define CLIENT_THP_SIZE 2
#define CLIENT_BUFF_SIZE 1000


class server {
private:
    struct client_connection;

private:
    ipv4::server_socket socket;
    std::map<client_connection*, std::unique_ptr<client_connection>> clients;

private:
    void on_connect(io_api::io_context& ctx);

public:
    server(io_api::io_context& ctx, ipv4::endpoint const& ep);
};

class http_buff {
private:
    std::string data;

public:
    http_buff() noexcept = default;

    int append(char* d, int size);

    bool ready() const noexcept;

    http::request<true> get_request() const;
};

struct server::client_connection {
private:
    server* srv;
    ipv4::socket socket;
    thread_pool<CLIENT_THP_SIZE> thp;
    storage<std::string> storage;

    char buff[CLIENT_BUFF_SIZE]{0};
    http_buff req_buff;

private:
    void on_disconnect();

    void on_read();

    void on_write();

public:
    client_connection(io_api::io_context& ctx, server* srv);
};


#endif //TGSERVER_SERVER_H
