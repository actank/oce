#ifndef COMMON_H
#define COMMON_H
#include <string>
#include <openssl/bio.h>
#include <openssl/err.h>
#include <openssl/rand.h>
#include <openssl/ssl.h>
#include <openssl/x509v3.h>
#include <IceUtil/Monitor.h>
#include <IceUtil/Thread.h>
#include <deque>
#include <vector>
#include <exception>
#include <stdexcept>
#include "LogWrapper.h"
using namespace std;
using namespace MyUtil;

#include <pthread.h>


#include <unistd.h>


int THREAD_setup(void);
int THREAD_cleanup(void);

#define PORT            "2195"
//#define SERVER          "gateway.sandbox.push.apple.com"
//#define CLIENT          "gateway.sandbox.push.apple.com"
#define SERVER          "gateway.push.apple.com"
#define CLIENT          "gateway.push.apple.com"


#define int_error(msg)  handle_error(__FILE__, __LINE__, msg)

struct MsgEntry {
	string tokenStr;
	string payload;
};

//void handle_error(const char *file, int lineno, const char *msg);

void init_OpenSSL(void);

//int verify_callback(int ok, X509_STORE_CTX *store);

//long post_connection_check(SSL *ssl, char *host);

//void seed_prng(void);

unsigned int htod(char c);

unsigned int char2binary(const string c);

vector<unsigned> string2binary(std::string s);

class APNSPusher: public IceUtil::Thread, public IceUtil::Monitor<
		IceUtil::Mutex> {
public:
	APNSPusher(const string& certfile, const string& password) :
		_certfile(certfile), _password(password) {
		_ssl_state = false;
		MCE_DEBUG("APNSPusher::APNSPusher --> certfile =  " << _certfile
				<< " password = " << _password);
	}
	virtual void run();
	void insertMsg(MsgEntry & entry);
	void pushOneMsg(MsgEntry & m);
	bool init();
	~APNSPusher();

private:
	bool _init_connection();
	SSL_CTX * _setup_client_ctx(void);
	int _do_client_loop(SSL *ssl);
	int _sendPayload(SSL *, const vector<unsigned> &, const char *, size_t);
	deque<MsgEntry> _msgQueue;

	BIO *_conn;
	SSL *_ssl;
	SSL_CTX *_ctx;

	bool _ssl_state;
	string _certfile;
	string _password;
};

class wrong_token_string: public std::runtime_error {
public:
	explicit wrong_token_string(const std::string &s) :
		std::runtime_error(s) {
	}
};

#endif /* COMMON_H */
