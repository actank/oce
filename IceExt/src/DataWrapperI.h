#ifndef __DATA_WRAPPERI_H__
#define __DATA_WRAPPERI_H__

#include "DataWrapper.h"
#include "DataSet.h"

#include <IceUtil/IceUtil.h>
#include <vector>
#include <string>
#include <Ice/Ice.h>
#include <Ice/Exception.h>
#include <IceUtil/AbstractMutex.h>
#include <IceUtil/Mutex.h>

namespace MyUtil
{
using namespace std;

class SetWrapper : public IceUtil::Shared
{
public:
    //virtual string& operator[](const string& key){return _s;};
    virtual string& operator[](const string& key) = 0;
    //virtual string operator[](const string & key) =0;
    //virtual Str2StrMap getProperties(){return Str2StrMap();};
    virtual Str2StrMap getProperties() = 0;
    //virtual bool empty(){return true;};
    virtual bool empty() = 0;
    //virtual int size(){return 0;};
    virtual int size() = 0;
};
typedef IceUtil::Handle<SetWrapper> SetWrapperPtr;

class SetWrapperHandle
{
private:
    SetWrapperPtr _ptr;
public:
    //SetWrapperHandle(SetWrapperPtr & ptr) : _ptr(ptr){};
    void set(const SetWrapperPtr & ptr)
    {
        _ptr = ptr;
    };

    inline string& operator[] (const string& key)
    {
        return (*_ptr)[key];
    };

    inline SetWrapperPtr & getSetWrapper()
    {
        return _ptr;
    };
};


class RowWrapper : public SetWrapper
{
private:
    ColumnPtr _key;
    RowPtr _row;

public:
    RowWrapper(ColumnPtr& key , RowPtr & row):_key(key),_row(row){};
    RowWrapper(RowWrapper& rw)
    {
        _key = rw._key;
        _row = rw._row;
    };
    inline virtual bool empty();
    inline virtual string& operator[](const string & key);
   // virtual string operator[](string & key);
    inline virtual int size();
    virtual Str2StrMap getProperties();
};

typedef IceUtil::Handle<RowWrapper> RowWrapperPtr;


class MapWrapper : public SetWrapper
{
private:
    Str2StrMap _props;

public:
    MapWrapper(){};
    MapWrapper(const Str2StrMap & props): _props(props){};
    MapWrapper(const MapWrapper& mw)
    {
        _props = mw._props;
    };
    inline virtual bool empty();
    inline virtual string& operator[](const string& key);
    //virtual string operator[](string & key);
    inline virtual int size();
    virtual Str2StrMap getProperties();
};

typedef IceUtil::Handle<MapWrapper> MapWrapperPtr;



class RowSetHelper
{
public:

    static RowWrapperPtr getRow(RowSetPtr& rs, int index);
    static RowSetPtr boxing(Str2StrMapSeq& seq);
	static RowSetPtr boxing(DataHandleSeq& seq);
};


//-----------database--------------
class DataObject : public IceUtil::Shared
{
public:
    DataObject()
    {
        SetWrapperPtr ptr = new MapWrapper();
        _properties.set(ptr);
        ptr = new MapWrapper();
        _updates.set(ptr);
    }
	Str2StrMap getProperties();
	void setProperties(const Str2StrMap& properties);
	void setProperties(const RowWrapperPtr&  properties);
    Str2StrMap getUpdates();

protected:
	SetWrapperHandle _properties;
	SetWrapperHandle _updates;
};





typedef IceUtil::Handle<DataObject> DataObjectPtr;
typedef vector<DataObjectPtr> DataObjectSeq;





};
#endif

