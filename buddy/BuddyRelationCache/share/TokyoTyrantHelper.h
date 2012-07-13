/*
 * =====================================================================================
 *
 *       Filename:  TokyoTyrantHelper.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2010年09月15日 18时18分51秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  guanghe.ge (ggh), guanghe.ge@opi-corp.com
 *        Company:  XiaoNei
 *
 * =====================================================================================
 */

#ifndef __TOKYO_TYRANT_HELPER_H__
#define __TOKYO_TYRANT_HELPER_H__
#include <Ice/Ice.h>
#include "BuddyEntry.h"
#include <IceUtil/IceUtil.h>
#include "util/cpp/String.h"
#include "TaskManager.h"
#include <tcrdb.h>
#include "ReplicaCluster/ServerState/src/client/cxx/ServerStateSubscriber.h"
/*************************** TokyoTyrant **************************/
namespace xce {
namespace tokyo {

const static int TASK_LEVEL_LOADER = 20;

/**转换类，自己可以继承此类实现转换
 *
 **/
class TokyoTyrantTranslate : virtual public Ice::Object
{
public:
    virtual string objectToString(Ice::ObjectPtr obj) = 0;
    virtual Ice::ObjectPtr stringToObject(const string& value) = 0;
    virtual int getCluster() = 0;
    virtual TCRDB * getRDB(int index) = 0;
};
typedef IceUtil::Handle<TokyoTyrantTranslate> TokyoTyrantTranslatePtr;

/**对TokyoTyrant的读写类，主要用于接受服务的读写，直接使用
 * @cluster : 服务份数，为了更好的散数据
 * @translate : Ice对象和TT存储对象的转换 
 * */
class TokyoTyrantHelper : virtual public Ice::Object
{
public:
    TokyoTyrantHelper (int cluster, TokyoTyrantTranslatePtr translate) : _cacheCluster(cluster), _translate(translate) {}
    Ice::ObjectPtr read(uint32_t id) {
	try {
	    string str = idToKey(id);
	    const char* idKey = str.c_str();

	    string value(tcrdbget2(_translate->getRDB(idToIndex(id)), idKey));
	    return _translate->stringToObject(value);	
	} catch (std::exception& e) {
            MCE_WARN("TokyoTyrantHelper::read. Exception : " << e.what());
	}catch (...) {
	    MCE_WARN("TokyoTyrantHelper::read. Exception!");
	}
	return NULL;
    }

    void write(uint32_t id, Ice::ObjectPtr obj) {
	try {
	    string idStr = idToKey(id);
            const char* idKey = idStr.c_str();

	    string stringValue = _translate->objectToString(obj);
	    const char * value = stringValue.c_str();

	    if (NULL != value) {
	        tcrdbput2(_translate->getRDB(idToIndex(id)), idKey, value);
	    } else {
		MCE_WARN("TokyoTyrantHelper::write, id " << id << " value is NULL");
	    }
	} catch (std::exception& e) {
	    MCE_WARN("TokyoTyrantHelper::write, Exception " << e.what());
	} catch (...) {
	    MCE_WARN("TokyoTyrantHelper::write, Exception! ");
	}
    }
private:
    int _cacheCluster;
    TokyoTyrantTranslatePtr _translate;

    int idToIndex(uint32_t id) {
	return id/_cacheCluster%_translate->getCluster();
    }

    const string idToKey(uint32_t id) {
	ostringstream os;
        os << id;
	return os.str();
    }

};
typedef IceUtil::Handle<TokyoTyrantHelper> TokyoTyrantHelperPtr;

/**对TokyoTyrant的加载类，主要用于接受Cache服务加载，直接使用
 *@category : ObjectCache类型，便于知道加载到那里
 *@translate : Ice对象和TT存储对象的转换
 * */
class TokyoTyrantLoader;
typedef IceUtil::Handle<TokyoTyrantLoader> TokyoTyrantLoaderPtr;

class TokyoTyrantLoader : virtual public Ice::Object
{
public:
    TokyoTyrantLoader(TokyoTyrantTranslatePtr translate) :
       	_translate(translate) {
    }

    virtual void addObject(uint32_t id, Ice::ObjectPtr obj) = 0;
    void load(){
	MyUtil::TaskManager::instance().config(TASK_LEVEL_LOADER, MyUtil::ThreadPoolConfig(1,1));
	MyUtil::TaskManager::instance().execute(new LoaderTask(_translate, this));
    }
private:
    TokyoTyrantTranslatePtr _translate;

    class LoaderThread : virtual public IceUtil::Thread {
    public:
	LoaderThread(int index, TokyoTyrantTranslatePtr translate, TokyoTyrantLoaderPtr  loader) :
	       	_index(index), _translate(translate), _loader(loader) {}
    protected:
	virtual void run() {
	   TCRDB * rdb = _translate->getRDB(_index);
	   tcrdbiterinit(rdb);
	   char * key = NULL;
	   char * value = NULL;
	   MCE_INFO("Thread " << _index << " : start to load!");
	   while((key = tcrdbiternext2(rdb)) != NULL) {
	       try {
		   if (!isDigit(key)) {
	  	       MCE_DEBUG("Thread " << _index << " : key " << key << " is not digit, continue!");
		       continue;
		   }
		   value = tcrdbget2(rdb, key);
		   string stringValue(tcrdbget2(rdb, key));
		   if ("" != stringValue) {
		       MCE_DEBUG("loader->addObject, id " << atoi(key) << " value " << stringValue);
		       _loader->addObject(atoi(key), _translate->stringToObject(stringValue));
		   }
	       }catch (...) {
	           MCE_WARN("id " << key << " preloader is WARN!");
	       }
	       free(value);
	   }
	   free(key);
	}
    private:
	int _index;
	TokyoTyrantTranslatePtr _translate;
	TokyoTyrantLoaderPtr _loader;

	bool isDigit(const char * key) {
	    for(int i = 0; key[i]!='\0'; i++) {
		if (key[i] < '0' || key[i] > '9')
		    return false;
	    }
	    return true;
	}
    };
    typedef IceUtil::Handle<LoaderThread> LoaderThreadPtr;

    class LoaderTask : virtual public ::MyUtil::Task {
    public:
	LoaderTask (TokyoTyrantTranslatePtr translate, TokyoTyrantLoaderPtr  loader, Ice::Int LEVEL = TASK_LEVEL_LOADER) :
		::MyUtil::Task(LEVEL), _translate(translate), _loader(loader) {}
    protected:
	virtual void handle() {
	    for (int i = 0 ; i < _translate->getCluster(); i++) {
		    LoaderThreadPtr thread = new LoaderThread(i, _translate, _loader);
		    _threads.push_back(thread);
	    }

	    for (std::vector<LoaderThreadPtr>::iterator it = _threads.begin();
			    it != _threads.end(); ++it) {
		    (*it)->start();
	    }

	    for (std::vector<LoaderThreadPtr>::iterator it = _threads.begin();
			    it != _threads.end(); ++it) {
		    (*it)->getThreadControl().join();
	    }
	    //加载完成设置
	    xce::serverstate::ServerStateSubscriber::instance().setStatus(1);
	    MCE_INFO("TokyoTyrantLoader is done!!");
	}
    private:
	TokyoTyrantTranslatePtr _translate;
	TokyoTyrantLoaderPtr _loader;

	std::vector<LoaderThreadPtr> _threads;
    };

};

}
}

/*************************** relation cache **************************/
namespace xce {
namespace buddy {

class TokyoTyrantRelationTranslate : virtual public xce::tokyo::TokyoTyrantTranslate
{
public:
    TokyoTyrantRelationTranslate(int cluster = 10) : _cluster(cluster) {
	    MCE_INFO("TokyoTyrantRelationTranslate, Constructor");
	    for (int i = 1980; i < 1980 + _cluster; i++) {
		    TCRDB * rdb = tcrdbnew();
		    if (tcrdbopen(rdb, "127.0.0.1", i)) {
			_RDBSeq.push_back(rdb);
	                MCE_INFO("TCRDB, port " << i << " is open!");
		    } else {
	                MCE_INFO("TCRDB, port " << i << " not open!");
		    }
	    }
    }

    virtual ~TokyoTyrantRelationTranslate() {
	    for (vector<TCRDB*>::iterator it = _RDBSeq.begin();
			    it != _RDBSeq.end(); ++it) {
		    if(!tcrdbclose(*it)){
	               int ecode = tcrdbecode(*it);
		       MCE_WARN("RDB not close, ecode " << ecode);
		    }

		    /* delete the object */
		    tcrdbdel(*it);
	    }
    }

    virtual string objectToString(Ice::ObjectPtr obj) {
	try {
	    ostringstream result;
	    ::xce::buddy::EntryListHolderPtr object = EntryListHolderPtr::dynamicCast(obj);
	    result << object->getFrom() << "|";
	    ::xce::buddy::EntryList list = object-> getAll();
	    for(std::vector<xce::buddy::Entry>::iterator it = list.begin();
			    it != list.end(); ++it) {
		    result << ":" << (*it).to << "/" << (*it).desc;
	    }
	    MCE_DEBUG("TokyoTyrantRelationTranslate::objectToChar done, value : " << result.str() );
	    return result.str();
	} catch (std::exception& e) {
	    MCE_WARN("TokyoTyrantRelationTranslate, objectToChar Exception " << e.what());
	}
	return NULL;
    }

    virtual Ice::ObjectPtr stringToObject(const string& str) {
	if ("" == str) {
            return NULL;
	}

	try {
	    vector<std::string> sumStr = MyUtil::StrUtil::split(str, "|");
	    if (2 != sumStr.size()) {
		return NULL;
	    }

	    vector<std::string> entryList = MyUtil::StrUtil::split(sumStr[1], ":");
	    ::xce::buddy::EntryList list;
	    for (vector<std::string>::iterator it = entryList.begin();
			    it != entryList.end(); ++it) {
		    ::xce::buddy::Entry entry;
		    vector<std::string> entryStr = MyUtil::StrUtil::split(*it,"/");
		    if (2 == entryStr.size()) {
			    entry.to = atoi(entryStr[0].c_str());
			    entry.desc = atoi(entryStr[1].c_str());
			    list.push_back(entry);
		    }
	    }
	    MCE_DEBUG("TokyoTyrantRelationTranslate::stringToObject done, id " << atoi(sumStr[0].c_str()) << " value " << str);
	    ::xce::buddy::EntryListHolderPtr result = new xce::buddy::EntryListHolder(atoi(sumStr[0].c_str()), list);
	    return result;
	} catch (std::exception& e) {
	    MCE_WARN("TokyoTyrantRelationTranslate::stringToObject Exception " << e.what());
	}
	return NULL;
    }

    virtual int getCluster() {
	    return _cluster;
    }

    virtual TCRDB * getRDB(int index) {
	    if (index >= _cluster) {
		    return NULL;
	    }
	    return _RDBSeq[index%_cluster];
    }

private:
    int _cluster;
    std::vector<TCRDB *> _RDBSeq;
};

}
}
#endif
