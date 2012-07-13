
#include "IceLogger.h"
#include "util/cpp/String.h"
#include "DataWrapperI.h"

#include <boost/regex.hpp>

using namespace MyUtil;

//---------------database-------------
Str2StrMap
DataObject::getProperties()
{
	return _properties.getSetWrapper()->getProperties();
}
Str2StrMap
DataObject::getUpdates()
{
	return _updates.getSetWrapper()->getProperties();
}


void
DataObject::setProperties(const Str2StrMap& properties)
{
	/*for (Str2StrMap::const_iterator it = properties.begin();
		it != properties.end(); ++it) {
		_properties[it->first] = it->second;
	}*/
	SetWrapperPtr ptr = new MapWrapper(properties);
	_properties.set(ptr);
}

void
DataObject::setProperties(const RowWrapperPtr & properties)
{
    _properties.set(SetWrapperPtr::dynamicCast(properties));
}


//-------------------------------------

RowSetPtr
RowSetHelper::boxing(Str2StrMapSeq & seq)
{
    RowSetPtr res = new RowSet();
    res ->keys = new Column();

    for(size_t i =0; i< seq.size(); ++i)
    {
        int index = 0;
        RowPtr r = new Row();
        for(Str2StrMap::iterator it = seq.at(i).begin();
            it != seq.at(i).end();
            ++it)
            {
                if(!i)
                {
                    res->keys ->value[it->first] = index++;
                }
                r->value.push_back(it->second);
            }

         res->rows.push_back(r);
    }
    return res;
}

RowSetPtr
RowSetHelper::boxing(DataHandleSeq& seq)
{
    RowSetPtr res = new RowSet();
    res->keys = new Column();
    for(size_t i =0 ;i< seq.size(); ++i) {
    	Str2StrMap properties = seq.at(i)->getProperties();
        RowPtr r = new Row();
        int index = 0;
        for(Str2StrMap::iterator it = properties.begin();
            it != properties.end(); ++it) {
                if(!i)  {
                    res->keys ->value[it->first] = index++;
                }
                r->value.push_back(it->second);
        }
		res->rows.push_back(r);
    }
    return res;
}


RowWrapperPtr
RowSetHelper::getRow(RowSetPtr& rs, int index)
{
    if(rs->keys->value.empty() || rs->rows.empty())
    {
        throw RowSetNullHandleException();
    }
    if((size_t)index >= rs->rows.size()) throw RowSetNullHandleException();

    return new RowWrapper(rs->keys, rs->rows.at(index));
}

//-------RowWrapper----------------------

bool
RowWrapper::empty()
{
    if(_key == 0 || _row == 0) throw RowSetNullHandleException();

    return _key->value.empty() || _row->value.empty();
}

string&
RowWrapper::operator [](const string & key)
{
    if(_key->value.find(key) == _key->value.end())
    {
        _key->value[key] = _row->value.size();
        string r ="";
        _row->value.push_back(r);

    }
    return  _row->value[_key->value[key]];
}

//string
//RowWrapper::operator[](string & key)
//{
//    if(_key->value.find(key) == _key->value.end())
//    {
//        _key->value[key] = _row->value.size();
//        string r ="";
//        _row->value.push_back(r);
//
//    }
//    return  _row->value[_key->value[key]];
//}

int
RowWrapper::size()
{
    if(empty()) return 0;

    return _row->value.size();
}

Str2StrMap
RowWrapper::getProperties()
{
    if(empty()) return Str2StrMap();

    Str2StrMap res;

    for(Str2IntMap::iterator it = _key->value.begin();
        it != _key->value.end(); ++ it )
    {
        res[it->first] = _row->value[it->second];
    }
    return res;
}


//----------MapWrapper---------

bool
MapWrapper::empty()
{
    return _props.empty();
}

string&
MapWrapper::operator[](const string& key)
{
    return _props[key];
}

//string
//MapWrapper::operator[](string& key)
//{
//    return _props[key];
//}

int
MapWrapper::size()
{
    return _props.size();
}

Str2StrMap
MapWrapper::getProperties()
{
    return _props;
}

