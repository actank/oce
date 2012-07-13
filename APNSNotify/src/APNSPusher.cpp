#include <string>
#include <arpa/inet.h>
#include <stdint.h>
#include <vector>
#include <deque>
#include <list>
#include <err.h>
#include <stdio.h>
#include "APNSPusher.h"
#include <IceUtil/Monitor.h>
#include <IceUtil/Thread.h>
using namespace std;


int DEVICE_BINARY_SIZE = 32;
int MAXPAYLOAD_SIZE = 256;



void init_OpenSSL(void) {
	if ( !SSL_library_init()) {

		exit(-1);
	}
	SSL_load_error_strings();
}





//void seed_prng(void) {
//	RAND_load_file("/dev/urandom", 1024);
//}

unsigned int htod(char c) {
	unsigned int i;
	if (c >= 'a' && c <= 'f')
		i = c - 'a' + 10;
	else if (c >= 'A' && c <= 'F')
		i = c - 'A' + 10;
	else if (c >= '0' && c <= '9')
		i = c - '0';
	else {
		throw wrong_token_string("wrong char of token");
	}
	return i;
}

unsigned int char2binary(const string s) {
	unsigned int dec = 0;
	for (int i = 0; s[i] != 0; ++i) {
		try {
			dec = dec * 16 + htod(s[i]);
		} catch (const wrong_token_string &e) {
			throw;
		}
	}
	return dec;
}

vector<unsigned> string2binary(std::string s) {
	string::iterator itr;
	vector<unsigned> binary;
	for (itr = s.begin(); itr != s.end(); ++itr) {
		if (*itr == ' ')
			s.erase(itr);
	}
	if(s.size() != 64)
		throw wrong_token_string("wrong size of token");

	int i;

	for (itr = s.begin(), i = 0; i < 8; itr += 8, ++i) {
		string tmpstring(itr, itr + 8);
		try {
			binary.push_back(char2binary(tmpstring));
		} catch (const wrong_token_string &e) {
			throw;
		}
	}
	return binary;

}




void revert(unsigned int in[]) {
	int i;
	for (i = 0; i < 8; i++) {
		in[i] = htonl(in[i]);
	}
}

void APNSPusher::run() {
	IceUtil::Monitor<IceUtil::Mutex>::Lock lock(*this);

	/*	MsgEntry me;
	 me.tokenStr = "0f55fcb2 9ef754bd 5d69e3b7 8e182e5a 8cd75ed6 98b6c832 54522a3d 2ef1fcfd";
	 me.payload = "{ \"aps\" : { \"alert\" : \"From Bob\" , \"badge\" : 0 } }";
	 _msgQueue.push_back(me);*/

	while (1) {
		while (_msgQueue.size() == 0 || _ssl_state == false) {
			wait();
		}

		MsgEntry m = _msgQueue.front();
		_msgQueue.pop_front();

		pushOneMsg(m);

	}
}

void APNSPusher::insertMsg(MsgEntry & entry) {
	if(entry.payload.size() > 249){
		MCE_WARN("payload exceeds 250 bytes" );
		return;
	}
	IceUtil::Monitor<IceUtil::Mutex>::Lock lock(*this);
	_msgQueue.push_back(entry);
	//if (_msgQueue.size() == 1)
	notify();

}

int APNSPusher::_sendPayload(SSL *sslPtr, const vector<unsigned> & TokenVec,
		const char *payloadBuff, size_t payloadLength) {
	int rtn = 0;
	unsigned deviceTokenBinary[8];

	memcpy(deviceTokenBinary, &TokenVec[0], sizeof(int) * TokenVec.size());
	revert(deviceTokenBinary);

	if (sslPtr && deviceTokenBinary && payloadBuff && payloadLength) {
		uint8_t command = 0; /* command number */

		char binaryMessageBuff[sizeof(uint8_t) + sizeof(uint16_t)
		 + DEVICE_BINARY_SIZE + sizeof(uint16_t) + payloadLength];
		//char binaryMessageBuff[293];
		/* message format is, |COMMAND|TOKENLEN|TOKEN|PAYLOADLEN|PAYLOAD| */
		char *binaryMessagePt = binaryMessageBuff;
		uint16_t networkOrderTokenLength = htons(DEVICE_BINARY_SIZE);
		uint16_t networkOrderPayloadLength = htons(payloadLength);
		/* command */
		*binaryMessagePt++ = command;
		/* token length network order */
		memcpy(binaryMessagePt, &networkOrderTokenLength, sizeof(uint16_t));
		binaryMessagePt += sizeof(uint16_t);
		/* device token */
		memcpy(binaryMessagePt, deviceTokenBinary, DEVICE_BINARY_SIZE);
		binaryMessagePt += DEVICE_BINARY_SIZE;
		/* payload length network order */
		memcpy(binaryMessagePt, &networkOrderPayloadLength, sizeof(uint16_t));
		binaryMessagePt += sizeof(uint16_t);
		/* payload */
		memcpy(binaryMessagePt, payloadBuff, payloadLength);
		binaryMessagePt += payloadLength;

		int code =SSL_write(sslPtr, binaryMessageBuff, (binaryMessagePt	- binaryMessageBuff));


		if (code >= 0) {
			rtn = 1;
			MCE_INFO("SSL_write() Success\n");
		} else {
			MCE_WARN("SSL_write() fail\n");
		}

		switch(SSL_get_error(sslPtr, code)){
		case SSL_ERROR_NONE:
			MCE_WARN("SSL_ERROR_NONE");
			break;
		case SSL_ERROR_ZERO_RETURN:
			MCE_WARN("SSL_ERROR_ZERO_RETURN" );
			break;
		case SSL_ERROR_WANT_READ:
			MCE_WARN( "SSL_ERROR_WANT_READ" );
			break;
		case SSL_ERROR_WANT_WRITE:
			MCE_WARN("SSL_ERROR_WANT_WRITE");
			break;
		case SSL_ERROR_SYSCALL:
			MCE_WARN("SSL_ERROR_SYSCALL");
			break;
		default:
			MCE_INFO("APNSPusher::_sendPayload --> SSL_get_error --> In default");
		}


	}
	return rtn;

}

APNSPusher::~APNSPusher() {
	SSL_clear(_ssl);
	MCE_INFO("SSL Connection closed\n");

	SSL_free(_ssl);
	SSL_CTX_free(_ctx);

}
bool APNSPusher::_init_connection(){
	init_OpenSSL();
//	seed_prng();

	_ctx = _setup_client_ctx();

	if(!_ctx){
		return false;
	}
	while(!(_conn = BIO_new_connect(SERVER  ":"  PORT) )){
		MCE_WARN("Error creating connection BIO");
	}


	while(BIO_do_connect(_conn) <= 0){
		MCE_WARN( "Error connecting to remote machine");
		sleep(2);
	}

	while(!(_ssl = SSL_new(_ctx))){
		MCE_WARN("Error creating an SSL context");
		sleep(2);
	}
	SSL_set_bio(_ssl, _conn, _conn);

	while(SSL_connect(_ssl) <= 0){
		MCE_WARN( "Error connecting SSL object");;
		sleep(2);
	}
/*	if (SSL_connect(ssl) <= 0)
		int_error("Error connecting SSL object");*/

	MCE_INFO( "SSL Connection opened!\n");
	_ssl_state = true;
	return true;
}
bool APNSPusher::init() {
	if(_init_connection()){
		start();	
		return true;
	}
	return false;
}

void APNSPusher::pushOneMsg(MsgEntry & m) {
	vector<unsigned int> token;

	try {

		token = string2binary(m.tokenStr);
		while (_sendPayload(_ssl, token, m.payload.c_str(), m.payload.size()) == 0) {
			_ssl_state = false;
			MCE_WARN("send fail,reconnecting...");
			SSL_clear(_ssl);
			MCE_WARN( "SSL Connection closed\n");
			SSL_free(_ssl);
			_init_connection();

		}

	} catch (const wrong_token_string &e) {

		MCE_WARN(e.what() );
		return;
	}

}

SSL_CTX * APNSPusher::_setup_client_ctx(void) {
	SSL_CTX *ctx;
	//char * cpassword = const_cast<char *>(password.c_str());

	ctx = SSL_CTX_new(SSLv23_method());

	if (SSL_CTX_use_certificate_chain_file(ctx, _certfile.c_str()) != 1){
		MCE_WARN("Error loading certificate from file");
	}
	SSL_CTX_set_default_passwd_cb_userdata(ctx, (void *)const_cast<char *>(_password.c_str()) );

	if (SSL_CTX_use_PrivateKey_file(ctx, _certfile.c_str(), SSL_FILETYPE_PEM) != 1){
		MCE_WARN("Error loading private key from file");
	}
	return ctx;
}

