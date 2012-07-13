#ifndef __DICT_TREE_H__
#define __DICT_TREE_H__

#include "DictNode.h"
#include <string>


namespace com
{
namespace xiaonei
{
namespace talk
{
namespace cache
{
namespace loader
{

using namespace std;
    
class DictTree;
typedef shared_ptr<DictTree> DictTreePtr;

class DictTree
{
protected:
    DictNodePtr _root;//(new DictNode()); 

public:
    DictTree():_root(new DictNode())
    {
        //DictNodePtr tmp(new DictNode());
        //_root = tmp;
        //_root(new DictNode());
    };
    void addPhrase(unsigned char phrase[], unsigned int stat, int tid);
    DictNodePtr getHeadNode();
    DictNodePtr getChildNode(DictNodePtr node, unsigned char key);
};

};  
};
};
};
};

#endif


