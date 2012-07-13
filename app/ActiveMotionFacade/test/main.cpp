#include <ActiveMotionFacadeAdapter.h>

using namespace std;
using namespace xce::activetrack::adapter;

int main(int argc, char* argv[]) {

	MyUtil::StrSeq params;
	params.push_back( "{\"ID\":\"24496255\",\"R\":\"http://www.renren.com/home?id=24496255\",\"X\":-199,\"Y\":303,\"T\":\"body\",\"time\",1234567890\"}" );

	ActiveMotionFacadeAdapter::instance().motion(params);

	cout << "motion done" << endl;
}

