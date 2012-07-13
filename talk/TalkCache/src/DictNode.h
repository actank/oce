#ifndef __DICT_NODE_H__
#define __DICT_NODE_H__

#include "CommonType.h"
#include<boost/shared_ptr.hpp>
#include<ext/hash_map>

namespace com
{
namespace xiaonei
{
namespace talk
{
namespace cache
{

using namespace std;
using namespace __gnu_cxx;
using namespace boost;

class DictNode;
typedef shared_ptr<DictNode> DictNodePtr;
typedef hash_map<unsigned char, DictNodePtr> Char2NodeMap;

class DictNode
{
protected:
    Char2NodeMap _node;
    unsigned int _stat;
	int _tid;

public:

	DictNode():_stat(NULL_STAT){};	
	
    DictNodePtr add(unsigned char key,unsigned int stat = NULL_STAT, int tid = -1);
    unsigned int getStat();
	int getTid();
    DictNodePtr getChild(unsigned char key);
    bool empty();
    bool contains(unsigned char key);
};



};
};
};
};
#endif

