#ifndef __SuggestionI_h__
#define __SuggestionI_h__

#include <Suggestion.h>
#include "IceExt/src/ServiceI.h"
#include "UtilCxx/src/TaskManager.h"
#include "DbCxxPool/src/QueryRunner.h"
#include <IceUtil/Mutex.h>
#include <IceUtil/RWRecMutex.h>
#include <IceUtil/IceUtil.h>
//#include <include/asm/atomic.h>
namespace com
{

namespace xiaonei
{

namespace services
{

namespace Suggestion
{
const std::string DB_FRIENDINPUT_PINYIN = "friendinput_pinyin";

class TypeHolder : public IceUtil::Shared {
public:
    void add(const Int2StrMap&, bool);
    std::map<int,std::string> searchPY(const std::string&, int);
    std::map<int,std::string> searchHZ(const std::string&, int);
private:
    void insert(int,const string&);
    void insertHZ(int,const std::vector<std::string>&);
    void insertPY(int,const std::vector<std::string>&);
    void splitWord(const std::string&, std::vector<std::string>& );
    void intersect(std::vector<int>&, const std::vector<int>& );
    bool startsWith(const std::string&,const std::string&);
    void splitPY(const string&, unsigned int,int, vector<string>, std::set<int>&);
    std::map<int,std::string> id2Name_;
    std::map<std::string, std::vector<int> > hz2Id_;
    std::map<std::string, std::vector<int> > py2Id_; 
    //IceUtil::RWRecMutex mutex_;
};
typedef IceUtil::Handle<TypeHolder> TypeHolderPtr;

class SuggestionManagerI : public SuggestionManager, 
	public MyUtil::Singleton<SuggestionManagerI>{
public:
    void initialize();
    virtual void buildIndex(IndexType type = App, const Ice::Current& ct = Ice::Current());
    virtual Int2StrMap search(IndexType, const ::std::string&, ::Ice::Int, const Ice::Current&);
    std::vector<std::string> getPinYin(const std::string&);

private:
    std::set<TypeHolderPtr> indexHolders;
    friend class MyUtil::Singleton<SuggestionManagerI>;
    std::map<std::string, std::vector<string> > dict_;
    void loadAppName(Int2StrMap&);
    std::map<IndexType,TypeHolderPtr> holders_;
    bool swapIndex;
    //atomic_t index_counter ATOMIC_INIT(0);
};

class PinYinResultHandler : virtual public com::xiaonei::xce::ResultHandler,
		public MyUtil::Singleton<PinYinResultHandler> {
public:
    PinYinResultHandler(std::map<std::string,std::vector<std::string> >& dict):  dict_(dict){}
protected:
    virtual bool handle(mysqlpp::Row& res) const;
private:
    std::map<std::string,std::vector<std::string> >& dict_;
};

class LoadAppNameHandler : virtual public com::xiaonei::xce::ResultHandler,
		public MyUtil::Singleton<LoadAppNameHandler> {
public:
    LoadAppNameHandler(std::map<int,std::string>& id2Name): id2Name_(id2Name){}
protected:
    virtual bool handle(mysqlpp::Row& res) const;
private:
    std::map<int,std::string >& id2Name_;
};

}

}

}
}
#endif
