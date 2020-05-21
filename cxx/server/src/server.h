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

#include "sysapi/sysapi.h"
#include "sysapi/thread_pool.h"

#include "name_daemon/name_daemon.h"
#include "PyServer.h"
#include "html/parser.h"
#include "language/src/lang_detect/langdetect.h"


#define ERRLOG_LVL 10
#define CLIENT_THP_SIZE 2
#define CLIENT_BUFF_SIZE 1000


struct concurrent_detector {
    std::mutex m;
    langdetect::Detector detector;

public:
    langdetect::Detected detect(std::string const& data);
};


class server {
private:
    struct client_connection;

private:
    ipv4::server_socket socket;
    std::map<client_connection*, std::unique_ptr<client_connection>> clients;
    name_daemon daemon;
    PyServer pyserver;
    concurrent_detector detector;

private:
    void on_connect(io_api::io_context& ctx);

    void process_put(http::request&& request);

public:
    server(io_api::io_context& ctx, ipv4::endpoint const& server_ep, ipv4::endpoint const& pyserver_ep);

    void process(http::request&& request);
};

struct server::client_connection {
private:
    server* srv;
    ipv4::socket socket;
    thread_pool<CLIENT_THP_SIZE> thp;
    storage<std::string> stor;

    char buff[CLIENT_BUFF_SIZE]{0};
    http::parser<http::request> parser;

private:
    void on_disconnect();

    void on_read();

    void on_write();

public:
    client_connection(io_api::io_context& ctx, server* srv);
};


#endif //TGSERVER_SERVER_H
