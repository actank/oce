#include "Buffer.h"

using namespace MyUtil;

string
Buffer::drain(const BufferDrainerPtr& drainer)
{
	string result;
	IceUtil::Mutex::Lock lock(_mutex);
	cout << "Buffer::drain -> _data.size: " << _data.size() << endl;
	if (!drainer) {
		result = _data;
		_data.clear();
		return result;
	}	
	size_t length = drainer->drain(_data);
    if (length <= 0 || _data.size() < length) {
    	cout << "Buffer::drain -> result.size: " << result.size() << endl;
		return result;
	} else {
		result.assign(_data, 0, length);
		_data.erase(0, length+1);
		return result;
	}
}

void
Buffer::insert(const string& data)
{
	IceUtil::Mutex::Lock lock(_mutex);
	_data.insert(0, data);
}

void
Buffer::append(const string& data)
{
	IceUtil::Mutex::Lock lock(_mutex);
	_data.append(data);
//	if (_data.capacity() > (_data.size() * 3)) {
//		string(_data).swap(_data);
//	}
}

void
Buffer::append(const char* data, int length)
{
	IceUtil::Mutex::Lock lock(_mutex);
	_data.append(data, length);
//	if (_data.capacity() > (_data.size() * 3)) {
//		string(_data).swap(_data);
//	}
}

void
Buffer::clear()
{
	IceUtil::Mutex::Lock lock(_mutex);
	_data.clear();
}
int 
Buffer::size()
{
	IceUtil::Mutex::Lock lock(_mutex);
    return _data.size();
}

