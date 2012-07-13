#ifndef __NOTIFY_CACHE_I_H__
#define __NOTIFY_CACHE_I_H__

#include "ServiceI.h"
#include "Dispatch.h"

namespace mop
{
namespace hi
{
namespace svc
{
namespace dispatch
{
using namespace MyUtil;

const Ice::Int TYPE_NOTE = 0;

//-------------------------------------------------------------------------
class NoteFactory : public ServantFactory
{
public:
	NoteFactory();
	virtual ~NoteFactory();
	
	virtual Ice::ObjectPtr create(Ice::Int id) { return 0; }
    virtual ObjectResultPtr create(const IntSeq& keys);
    
private:
	string _noteTable;
	string _database;
	string _databaseOld;
};




};
};
};
};
#endif

