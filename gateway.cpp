#include <iostream>
#include "boost/asio.hpp"
#include <vector>

class forwarder {
public:
    forwarder(boost::asio::io_service& service) 
        :
        temp_buffer(100),
        receiver_acceptor(service, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), 5000)),
        sender_acceptor(service, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), 10000)),
        receiver_socket(service),
        sender_socket(service)
    {
        receiver_acceptor.async_accept(receiver_socket, [&] (const boost::system::error_code&) { handle_accept(); });
        sender_acceptor.accept(sender_socket);
    }

private:
    std::vector<char> temp_buffer;
    boost::asio::ip::tcp::acceptor receiver_acceptor;
    boost::asio::ip::tcp::acceptor sender_acceptor;
    boost::asio::ip::tcp::socket receiver_socket;
    boost::asio::ip::tcp::socket sender_socket;

    void handle_accept() {
        std::cout << "accepted" << std::endl;
        receiver_socket.async_read_some(boost::asio::buffer(temp_buffer, 100), 
            [&] (const boost::system::error_code&, size_t bytes) { handle_read(bytes); });
    }

    void handle_read(size_t num_bytes) {
        sender_socket.async_send(boost::asio::buffer(temp_buffer, num_bytes), [] (const boost::system::error_code&, size_t) {});

        receiver_socket.async_read_some(boost::asio::buffer(temp_buffer, 100), 
            [&] (const boost::system::error_code&, size_t bytes) { handle_read(bytes); });
    }
};

int main() {
    boost::asio::io_service service;
    forwarder server(service);
    service.run();
}
