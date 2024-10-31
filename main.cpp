#include <ixwebsocket/IXWebSocket.h>
#include <iostream>

#include "DeribitClient.h"

int main() {
    const std::string client_id = getenv("client_id");
    const std::string client_secret = getenv("client_secret");

    DeribitClient client1(client_id, client_secret);
    client1.connect();


    std::cout <<
            "Enter option:\n 0 - connect\n1 - place order\n2 - cancel order\n3 - modify order\n4 - subscribe order book\n5 - get current positions"
            << std::endl;

    std::string instrument;
    std::string order_id;
    double amount;
    double price;

    int option;
    while (std::cin >> option) {
        switch (option) {
            case 0:
                client1.connect();
                break;
            case 1:
                std::cout << "Enter instrument" << std::endl;
                std::cin >> instrument;
                std::cout << "Enter amount" << std::endl;
                std::cin >> amount;
                std::cout << "Enter price" << std::endl;
                std::cin >> price;
                client1.placeOrder(instrument, 40, 100);
                break;
            case 2:
                std::cout << "Enter order_id" << std::endl;
                std::cin >> order_id;
                client1.cancelOrder(order_id);
                break;
            case 3:
                std::cout << "Enter order_id" << std::endl;
                std::cin >> order_id;
                std::cout << "Enter new price" << std::endl;
                std::cin >> price;
                std::cout << "Enter new amount" << std::endl;
                std::cin >> amount;
                client1.modifyOrder(order_id, price, amount);
                break;
            case 4:
                std::cout << "Enter instrument" << std::endl;
                std::cin >> instrument;
                client1.subscribeOrderBook(instrument);
                break;
            case 5:
                std::cout << "Enter instrument" << std::endl;
                std::cin >> instrument;
                client1.getCurrentPositions(instrument);
                break;
            default: std::cout << "Invalid option" << std::endl;
        }
        std::cout <<
                "Enter option:\n0 - connect\n1 - place order\n2 - cancel order\n3 - modify order\n4 - subscribe order book\n5 - get current positions"
                << std::endl;
        std::cout << "> " << std::flush;
    }

    return 0;
}
