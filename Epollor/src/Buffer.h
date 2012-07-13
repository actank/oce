#ifndef __BUFFER_H__
#define __BUFFER_H__

#include <IceUtil/Mutex.h>
#include <Ice/Ice.h>
#include <IceUtil/IceUtil.h>
#include <vector>

namespace MyUtil
{
using namespace std;

class BufferDrainer : public IceUtil::Shared
{
public:
	virtual int drain(const string& data) = 0;	
};
typedef IceUtil::Handle<BufferDrainer> BufferDrainerPtr;

class Buffer : public IceUtil::Shared
{
public:
    string drain(const BufferDrainerPtr& drainer);
    
    void insert(const string& data);
    
    void append(const string& data);
    
    void append(const char* data, int length);
    
    void clear();

    int size();
private:
    string _data;
    IceUtil::Mutex _mutex;
};
typedef IceUtil::Handle<Buffer> BufferPtr;

}
#endif
