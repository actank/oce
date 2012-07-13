#include "DictNode.h"

using namespace std;
using namespace com::xiaonei::talk::cache::loader;


DictNodePtr
DictNode:: add(unsigned char key, unsigned int stat, int tid)
{
	Char2NodeMap::iterator it = _node.find(key);
	if(it == _node.end())
	{

		DictNodePtr node(new DictNode());
		_node.insert(pair<unsigned char, DictNodePtr>(key,node));
		node->_stat = stat;
		node->_tid = tid;
		return node;
	}else
	{
		it->second->_stat |=stat;
		if(tid != -1) it->second->_tid = tid;
		return it->second;
	}
}

unsigned int 
DictNode:: getStat()
{
    return _stat;
}

int 
DictNode:: getTid()
{
	return _tid;
}

DictNodePtr
DictNode:: getChild(unsigned char key)
{
    Char2NodeMap::iterator it = _node.find(key);
    if(it != _node.end()) return it->second;
    else return DictNodePtr();
}

bool
DictNode:: empty()
{
    return _node.empty();
}

bool 
DictNode:: contains(unsigned char key)
{
    Char2NodeMap::iterator it = _node.find(key);
    return it != _node.end();
}
