#ifndef __TYPE_H__
#define __TYPE_H__

namespace MyUtil
{
using namespace std;

template <class T>
struct Type2Type
{
	typedef T OriginalType;
};

template <int v>
struct Int2Type
{
	enum { value = v };
};

};
#endif

