#include <CallGuarantee/src/CallGuarantee.h>
#include <iostream>

using namespace std;

class D : public Ice::Object {
};

class CallGuaranteeTest : public callguarantee::CallGuarantee, 
	public MyUtil::Singleton<CallGuaranteeTest> {
public:
	CallGuaranteeTest() : CallGuarantee(1, 10, 2) {}
	virtual bool Do(const std::vector<std::pair<int, Ice::ObjectPtr> >& params) {
		cout << "do" << " " << params.size() << endl;
		return false;
	}
        virtual bool Redo(const std::vector<int>& ids) {
		cout << "redo" << endl;
		return false;
	}
};

int main() {
	CallGuaranteeTest::instance().Add(1,new D);
	CallGuaranteeTest::instance().Add(1,new D);
	CallGuaranteeTest::instance().Add(1,new D);
	CallGuaranteeTest::instance().Add(1,new D);
	CallGuaranteeTest::instance().Add(1,new D);
	CallGuaranteeTest::instance().Add(1,new D);
	CallGuaranteeTest::instance().Add(1,new D);
	CallGuaranteeTest::instance().Add(1,new D);
	CallGuaranteeTest::instance().Add(1,new D);
	CallGuaranteeTest::instance().Add(1,new D);
	CallGuaranteeTest::instance().Add(1,new D);
	CallGuaranteeTest::instance().Add(1,new D);
	sleep(5);
	return 0;
}
