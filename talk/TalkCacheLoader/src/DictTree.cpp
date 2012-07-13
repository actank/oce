#include "DictTree.h"

using namespace com::xiaonei::talk::cache::loader;
using namespace std;
using namespace boost;

void
DictTree:: addPhrase(unsigned char phrase[], unsigned int stat, int tid)
{
    DictNodePtr node = _root;
    int len = strlen((char *)phrase);
    for(int i=0; i<len; ++i)
    {
        DictNodePtr tmp = node->getChild(phrase[i]);
        if((tmp != NULL)&& (i< len-1))
        {
            node = tmp;
            continue;
        }
        if( i < len-1 )
        {
            node = node->add(phrase[i]);
        }else
        {
            node = node->add(phrase[i], stat, tid);
        };
    };
}

DictNodePtr 
DictTree:: getHeadNode()
{
    return _root;
}

DictNodePtr
DictTree:: getChildNode(DictNodePtr node, unsigned char ch)
{
    return node->getChild(ch);
}

