//
// chat_client.cpp
// ~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2007 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <cstdlib>
#include <deque>
#include <iostream>
#include <boost/bind.hpp>
#include "asio.hpp"
#include <string>

using namespace std;
using asio::ip::tcp;

typedef std::deque<string> message_queue;

class client {
public:
	client(asio::io_service& io_service,
			tcp::resolver::iterator endpoint_iterator) :
		io_service_(io_service), socket_(io_service) {
		tcp::endpoint endpoint = *endpoint_iterator;
		socket_.async_connect(endpoint, boost::bind( &client::handle_connect,
				this, asio::placeholders::error, ++endpoint_iterator));
	}

	void write(const string& msg) {
		io_service_.post(boost::bind(&client::do_write, this, msg));
	}

	void close() {
		io_service_.post(boost::bind(&client::do_close, this));
	}

private:

	void handle_connect(const asio::error_code& error,
			tcp::resolver::iterator endpoint_iterator) {
		if (!error) {
			asio::async_read(socket_,
					asio::buffer(read_msg_, sizeof(read_msg_)), boost::bind(
							&client::handle_read, this,
							asio::placeholders::error));
		} else if (endpoint_iterator != tcp::resolver::iterator()) {
			socket_.close();
			tcp::endpoint endpoint = *endpoint_iterator;
			socket_.async_connect(endpoint, boost::bind(
					&client::handle_connect, this, asio::placeholders::error,
					++endpoint_iterator));
		}
	}

	void handle_read(const asio::error_code& error) {
		if (!error) {
			//			std::cout.write(read_msg_.body(), read_msg_.body_length());
			//			std::cout << "\n";
			asio::async_read(socket_,
					asio::buffer(read_msg_, sizeof(read_msg_)), boost::bind(
							&client::handle_read, this,
							asio::placeholders::error));
		} else {
			do_close();
		}
	}

	void do_write(string& msg) {
		bool write_in_progress = !write_msgs_.empty();
		write_msgs_.push_back(msg);
		if (!write_in_progress) {
			asio::async_write(socket_, asio::buffer(write_msgs_.front().data(), write_msgs_.front().length()), boost::bind(&client::handle_write, this,
					asio::placeholders::error));
		}
	}

	void handle_write(const asio::error_code& error) {
		if (!error) {
			write_msgs_.pop_front();
			if (!write_msgs_.empty()) {
				asio::async_write(socket_, asio::buffer(write_msgs_.front().data(), write_msgs_.front().length()), boost::bind(&client::handle_write, this,
						asio::placeholders::error));
			}
		} else {
			do_close();
		}
	}

	void do_close() {
		socket_.close();
	}

private:
	asio::io_service& io_service_;
	tcp::socket socket_;
	char read_msg_[256];
	message_queue write_msgs_;
};

int main(int argc, char* argv[]) {
	try
	{
		if (argc != 3)
		{
			std::cerr << "Usage: XceAsioTester <host> <port>\n";
			return 1;
		}
		const static int CLIENT_NUM = 10000;
		const static int FREQUENCY = 200;
		const static time_t STOP_TIME = -1;
		
		asio::io_service io_service;

		tcp::resolver resolver(io_service);
		tcp::resolver::query query(argv[1], argv[2]);
		tcp::resolver::iterator iterator = resolver.resolve(query);

		list<client*> clients;
		for (int i = 0; i < CLIENT_NUM; ++i) {
			clients.push_back(new client(io_service, iterator));
			if (i % FREQUENCY == 0) {
				sleep(1);
			}
		}
		asio::thread t(boost::bind(&asio::io_service::run, &io_service));
		char message[50];
		int i = 0;
		while (true) {
			list<client*> delete_clients;
			for (list<client*>::iterator it = clients.begin(); it != clients.end(); ) {
				if (i++ % FREQUENCY == 0) {
					for (int count = 0; count < 10 && it != clients.end(); ++count) {
						client* c = *it;
						it = clients.erase(it);
						c->close();
						delete_clients.push_back(c);
					}
					sleep(1);
				} else {
					client* c = *it;
					sprintf(message, "message: %d", i);
					c->write(message);
					++it;
				}
			}
			for (list<client*>::iterator it = delete_clients.begin(); it != delete_clients.end(); ++it) {
				delete *it;
				clients.push_back(new client(io_service, iterator));
			}
			if (STOP_TIME > 0 && time(NULL) >= STOP_TIME) {
				break;
			}
		}
		t.join();
	}
	catch (std::exception& e)
	{
		std::cerr << "Exception: " << e.what() << "\n";
	}

	return 0;
}
