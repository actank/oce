#ifndef _STREAM_H_
#define _STREAM_H_

#include "ObjectCacheI.h"

using namespace MyUtil;

class StreamObject: virtual public Ice::Object, virtual public IceUtil::Mutex {
public:
	StreamObject(ofstream& file) : _outfile(file) {}
	void Write(int userId, int friendId, int visit, int visited, int reply, int replyed, int talk, int talked) {
		IceUtil::Mutex::Lock lock(*this);

		ostringstream stream;
		stream << userId << "\t" << friendId << "\t" << visit << "\t" << visited << "\t" << 
				reply << "\t" << replyed << "\t" << talk << "\t" << talked << "\n";

		if (_outfile) {
			_outfile<< stream.str();		
		}
	}

	void WriteString(const std::string& ss) {
		IceUtil::Mutex::Lock lock(*this);
		
		if (_outfile) {
			_outfile << ss;	
		}
	}

private:
	ofstream& _outfile;
};

typedef IceUtil::Handle<StreamObject> StreamObjectPtr;

#endif
