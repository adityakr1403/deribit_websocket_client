//
// Created by aditya on 10/31/24.
//

#include "DeribitClient.h"

#include <iostream>
#include <utility>
#include <rapidjson/document.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>

DeribitClient::DeribitClient(std::string client_id, std::string client_secret): client_id(std::move(client_id)),
    client_secret(std::move(client_secret)) {
    web_socket.setUrl(url);
    web_socket.setOnMessageCallback([this](const ix::WebSocketMessagePtr &msg) {
        if (msg->type == ix::WebSocketMessageType::Message) {
            handleMessage(msg);
        } else if (msg->type == ix::WebSocketMessageType::Open) {
            std::cout << "Connection established" << std::endl;
            std::cout << "> " << std::flush;
        } else if (msg->type == ix::WebSocketMessageType::Error) {
            std::cout << "Connection error: " << msg->errorInfo.reason << std::endl;
            std::cout << "> " << std::flush;
        }
    });

    web_socket.start();
}


void DeribitClient::connect() {
    rapidjson::Document document;
    document.SetObject();

    document.AddMember("id", 1, document.GetAllocator());
    document.AddMember("method", "public/auth", document.GetAllocator());
    document.AddMember("jsonrpc", "2.0", document.GetAllocator());
    document.AddMember("params", rapidjson::Value(rapidjson::kObjectType), document.GetAllocator());
    document["params"].AddMember("grant_type", "client_credentials", document.GetAllocator());
    // document["params"].AddMember("scope", "session:read", document.GetAllocator());
    document["params"].AddMember("client_id", rapidjson::Value(client_id.c_str(), document.GetAllocator()),
                                 document.GetAllocator());
    document["params"].AddMember("client_secret", rapidjson::Value(client_secret.c_str(), document.GetAllocator()),
                                 document.GetAllocator());

    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    document.Accept(writer);

    std::cout << "Sending authentication request..." << std::endl;

    web_socket.send(buffer.GetString());
}

void DeribitClient::placeOrder(const std::string &instrument, const double amount, const double price) {
    rapidjson::Document orderData;
    orderData.SetObject();

    orderData.AddMember("id", 2, orderData.GetAllocator());
    orderData.AddMember("method", "private/buy", orderData.GetAllocator());
    orderData.AddMember("jsonrpc", "2.0", orderData.GetAllocator());
    orderData.AddMember("access_token", rapidjson::Value(access_token.c_str(), orderData.GetAllocator()),
                        orderData.GetAllocator());
    orderData.AddMember("params", rapidjson::Value(rapidjson::kObjectType), orderData.GetAllocator());
    orderData["params"].AddMember("instrument_name", rapidjson::Value(instrument.c_str(), orderData.GetAllocator()),
                                  orderData.GetAllocator());
    orderData["params"].AddMember("amount", amount, orderData.GetAllocator());
    orderData["params"].AddMember("price", price, orderData.GetAllocator());
    orderData["params"].AddMember("type", "limit", orderData.GetAllocator());
    orderData["params"].AddMember("post_only", true, orderData.GetAllocator());

    sendRequest(orderData);
}

void DeribitClient::cancelOrder(const std::string &orderId) {
    rapidjson::Document cancelData;
    cancelData.SetObject();

    cancelData.AddMember("id", 3, cancelData.GetAllocator());
    cancelData.AddMember("method", "private/cancel", cancelData.GetAllocator());
    cancelData.AddMember("jsonrpc", "2.0", cancelData.GetAllocator());
    cancelData.AddMember("access_token", rapidjson::Value(access_token.c_str(), cancelData.GetAllocator()),
                         cancelData.GetAllocator());
    cancelData.AddMember("params", rapidjson::Value(rapidjson::kObjectType), cancelData.GetAllocator());
    cancelData["params"].AddMember("order_id", rapidjson::Value(orderId.c_str(), cancelData.GetAllocator()),
                                   cancelData.GetAllocator());

    sendRequest(cancelData);
}

void DeribitClient::modifyOrder(const std::string &orderId, double newPrice, double newAmount) {
    rapidjson::Document modifyData;
    modifyData.SetObject();

    modifyData.AddMember("id", 4, modifyData.GetAllocator());
    modifyData.AddMember("method", "private/edit", modifyData.GetAllocator());
    modifyData.AddMember("jsonrpc", "2.0", modifyData.GetAllocator());
    modifyData.AddMember("access_token", rapidjson::Value(access_token.c_str(), modifyData.GetAllocator()),
                         modifyData.GetAllocator());
    modifyData.AddMember("params", rapidjson::Value(rapidjson::kObjectType), modifyData.GetAllocator());
    modifyData["params"].AddMember("order_id", rapidjson::Value(orderId.c_str(), modifyData.GetAllocator()),
                                   modifyData.GetAllocator());
    modifyData["params"].AddMember("amount", newAmount, modifyData.GetAllocator());
    modifyData["params"].AddMember("price", newPrice, modifyData.GetAllocator());

    sendRequest(modifyData);
}

void DeribitClient::subscribeOrderBook(const std::string &instrument) {
    rapidjson::Document subscribeData;
    subscribeData.SetObject();

    subscribeData.AddMember("id", 5, subscribeData.GetAllocator());
    subscribeData.AddMember("method", "public/subscribe", subscribeData.GetAllocator());
    subscribeData.AddMember("jsonrpc", "2.0", subscribeData.GetAllocator());
    subscribeData.AddMember("params", rapidjson::Value(rapidjson::kObjectType), subscribeData.GetAllocator());
    subscribeData["params"].AddMember("channels", rapidjson::Value(rapidjson::kArrayType),
                                      subscribeData.GetAllocator());
    subscribeData["params"]["channels"].PushBack(
        rapidjson::Value(("book." + instrument + ".none.1.100ms").c_str(), subscribeData.GetAllocator()),
        subscribeData.GetAllocator());

    sendRequest(subscribeData);
}

void DeribitClient::getCurrentPositions(const std::string &instrument) {
    rapidjson::Document positionData;
    positionData.SetObject();

    positionData.AddMember("id", 6, positionData.GetAllocator());
    positionData.AddMember("method", "private/get_position", positionData.GetAllocator());
    positionData.AddMember("jsonrpc", "2.0", positionData.GetAllocator());
    positionData.AddMember("access_token", rapidjson::Value(access_token.c_str(), positionData.GetAllocator()),
                           positionData.GetAllocator());
    positionData.AddMember("params", rapidjson::Value(rapidjson::kObjectType), positionData.GetAllocator());
    positionData["params"].AddMember("instrument_name",
                                     rapidjson::Value(instrument.c_str(), positionData.GetAllocator()),
                                     positionData.GetAllocator());

    sendRequest(positionData);
}


void DeribitClient::sendRequest(const rapidjson::Document &document) {
    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    document.Accept(writer);

    web_socket.send(buffer.GetString());
}

void DeribitClient::handleMessage(const ix::WebSocketMessagePtr &msg) {
    const std::string msg_str = msg->str;
    std::cout << "received message: " << msg_str << std::endl;
    rapidjson::Document document;
    document.Parse(msg_str.c_str());
    if (document.IsObject()) {
        if (document.HasMember("result") && document["result"].IsObject()) {
            const rapidjson::Value &result = document["result"];
            if (result.HasMember("access_token") && result["access_token"].IsString()) {
                access_token = result["access_token"].GetString();
                std::cout << "access_token : " << result["access_token"].GetString() << "\n";
            }
            if (result.HasMember("refresh_token") && result["refresh_token"].IsString()) {
                refresh_token = result["refresh_token"].GetString();
                std::cout << "refresh_token : " << result["refresh_token"].GetString() << "\n";
            }
        }
    }
    std::cout << "> " << std::flush;
}
