//
// Created by aditya on 10/31/24.
//

#ifndef DERIBITCLIENT_H
#define DERIBITCLIENT_H
#include <string>
#include <ixwebsocket/IXWebSocket.h>
#include <rapidjson/document.h>


class DeribitClient {
public:
    DeribitClient(std::string client_id, std::string client_secret);
    void connect();
    void placeOrder(const std::string &instrument, double amount, double price);
    void cancelOrder(const std::string &orderId);
    void modifyOrder(const std::string &orderId, double newPrice, double newAmount);
    void subscribeOrderBook(const std::string &instrument);
    void getCurrentPositions(const std::string &instrument);

private:
    std::string url = "wss://test.deribit.com/ws/api/v2";
    ix::WebSocket web_socket;
    std::string client_id;
    std::string client_secret;
    std::string access_token;
    std::string refresh_token;

    void sendRequest(const rapidjson::Document &document);

    void handleMessage(const ix::WebSocketMessagePtr &msg);
};

#endif //DERIBITCLIENT_H
