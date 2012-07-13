/* 
 * File:   AddressBook.h
 * Author: jingkunliu
 *
 * Created on 2010年7月14日, 下午5:10
 */

#ifndef __ADDRESSBOOKI_H__
#define	__ADDRESSBOOKI_H__

#include<string>
#include<map>
#include<Ice/Ice.h>

#include <IceUtil/RWRecMutex.h>

#include "Singleton.h"
#include "AddressBook.h"


namespace talk {
    namespace AddressBook
    {
        using namespace std;
        using namespace MyUtil;

        const string TABLE_ADDRESSBOOK = "weight_factor";
        const string DB_INSTANCE = "feed_db";

       #define COL_NAME  "name"
       #define COL_EMAIL  "factor"

        class AddressBookI : public AddreeBook, public Singleton<AddressBookI>
        {
            
        public:
            virtual string getAddressBook(const ::std::string&, const ::Ice::Current& = ::Ice::Current());

            virtual bool addAddressBook(const ::std::string&, const ::std::string&, const ::Ice::Current& = ::Ice::Current());

            virtual bool updateAddressBook(const ::std::string&, const ::std::string&, const ::Ice::Current& = ::Ice::Current());

            virtual bool deleteAddressBook(const ::std::string&, const ::Ice::Current& = ::Ice::Current());

            int GetABCount();

        private:
            void AddAddressBook4Db(const string& strName, const string& strEmail);

            void UpdateAddressBook4Db(const string& strName, const string& strEmail);

            void DeleteAddressBook4Db(const string&  strName);
            
            bool GetAddressBook4Db(const string& strName,  int & strEmailGet);

            bool ExistAddressBook4Db(const string& strName);

            map<string, string>   m_addressbook;
            IceUtil::RWRecMutex m_admutex;
        };
    };
};
#endif	/* __ADDRESSBOOKI_H__ */

