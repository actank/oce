/*
 * ClientTest.cpp
 *
 *  Created on: 2010-9-17
 *      Author: tangzhaohui
 */

#include <Ice/Ice.h>
#include <iostream>
#include <string.h>
#include "PVCountDemo.h"

using namespace std;
using namespace Demo;

class PVCountDemoClient: virtual public Ice::Application {
public:
	virtual int run(int, char *[]) {
		string index = "0";
		cout << "Input the index of the server node: ";
		cin >> index;

		string name = "PVCountDemo@PVCountDemo" + index;
		cout << name << endl;

		Ice::CommunicatorPtr ic = communicator();
		Ice::ObjectPrx objectPrx = ic->stringToProxy(name);

		pvCountDemo = PVCountDemoPrx::checkedCast(objectPrx);

		if (!pvCountDemo) {
			throw "Invalid Proxy!";
		}

		cout << "\tSelect the operation:" << endl;
		cout << "\tAdd------------输入0" << endl;
		cout << "\tUpdate---------输入1" << endl;
		cout << "\tRemove---------输入2" << endl;
		cout << "\tGet------------输入3（默认）" << endl;
		cout << "\tExit-----------输入exit" << endl;
		string operation = "";
		int userId = 0;
		int pvCount = 0;
		while (true) {
			cout << "-----------------------------------------------" << endl;
			cout << "Input the operation: " << endl;
			cin >> operation;
			if (operation == string("0")) {
				cout << "###Test Add###: " << endl;
				cout << "Input userId: " << endl;
				cin >> userId;
				cout << "Input pvCount: " << endl;
				cin >> pvCount;
				testAdd(userId, pvCount);
			} else if (operation == string("1")) {
				cout << "###Test update###: " << endl;
				cout << "Input userId: " << endl;
				cin >> userId;
				cout << "Input pvCount: " << endl;
				cin >> pvCount;
				testUpdate(userId, pvCount);
			} else if (operation == string("2")) {
				cout << "###Test remove###: " << endl;
				cout << "Input userId: " << endl;
				cin >> userId;
				testRemove(userId);
			} else if (operation == string("3")) {
				cout << "###Test get###: " << endl;
				cout << "Input userId: " << endl;
				cin >> userId;
				cout << "The pvCount is: " << testGet(userId) << endl;
			} else if (operation == string("exit")) {
				return 0;
			} else {
				cout << "Input error, again please! " << endl;
			}
		}
	}
protected:
	PVCountDemoPrx pvCountDemo;
private:
	void testAdd(int userId, int pvCount) {
		pvCountDemo->add(userId, pvCount);
	}
	void testUpdate(int userId, int pvCount) {
		pvCountDemo->update(userId, pvCount);
	}
	void testRemove(int userId) {
		pvCountDemo->remove(userId);
	}
	int testGet(int userId) {
		return pvCountDemo->getPVCountByUserId(int(userId));
	}
};

int main(int argc, char* argv[]) {
	PVCountDemoClient client;
	return client.main(argc, argv);
}
