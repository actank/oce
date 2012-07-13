#include "AddressBookI.h"
#include "ServiceI.h"
#include "QueryRunner.h"

using namespace com::xiaonei::xce;
using namespace ::talk::AddressBook;
using namespace MyUtil;

class LogCountTimer : public MyUtil::Timer {
public:

    LogCountTimer(int delay) : Timer(delay) {
    }

    virtual void handle() {
        MCE_INFO("LogCountTimer::handle() started.");
        int nCount = -1;
        try {
            Statement sql;
            sql << "SELECT COUNT(*) FROM " << TABLE_ADDRESSBOOK;
            mysqlpp::StoreQueryResult res = QueryRunner(DB_INSTANCE, CDbRServer, TABLE_ADDRESSBOOK).store(sql);
            if (!res) {
                MCE_INFO("LogCountTimer::handle(), Get Count error");
            }
            mysqlpp::Row & row = res.at(0);
            nCount = row.at(0);
        } catch (...) {
            MCE_INFO("LogCountTimer::handle(), Get Count Try error");
        }
        MCE_INFO("LogCountTimer::handle(), Get Count: " << nCount);
        MCE_INFO("LogCountTimer::handle() complete.");
    }
};

void MyUtil::initialize() {
    ServiceI& service = ServiceI::instance();
    service.getAdapter()->add(&AddressBookI::instance(), service.createIdentity("AddressBook", ""));

    TaskManager::instance().scheduleRepeated(new LogCountTimer(30 * 1000));
}

string AddressBookI::getAddressBook(const ::std::string& name, const ::Ice::Current&) {
    IceUtil::RWRecMutex::RLock lock(m_admutex);
    map<string, string>::iterator iter;
    iter = m_addressbook.find(name);
    if (iter != m_addressbook.end()) {
        return iter->second;
    } else {
        try {
            int strRtn;
            if (GetAddressBook4Db(name, strRtn)) {
                MCE_INFO("AddressBookI --> call  GetAddressBook4Db:" << strRtn);
                return "HelloWorld";
            }
        } catch (...) {
            MCE_WARN("AddressBookI --> call  GetAddressBook4Db  err");
        }
    }
    return string("Not Exist!");
}

bool AddressBookI::addAddressBook(const ::std::string& name, const ::std::string& email, const ::Ice::Current&) {
    IceUtil::RWRecMutex::WLock lock(m_admutex);
    m_addressbook[name] = email;
    try {
        if (ExistAddressBook4Db(name)) {
            MCE_INFO("AddressBookI --> call  addAddressBook  exist"<<name);
            UpdateAddressBook4Db(name, email);
        } else {
            MCE_INFO("AddressBookI --> call  addAddressBook  not exist"<<name);
            AddAddressBook4Db(name, email);
        }
    } catch (...) {
        MCE_WARN("AddressBookI --> call  addAddressBook  err");
    }
    return true;
}

bool AddressBookI::updateAddressBook(const ::std::string& name, const ::std::string& email, const ::Ice::Current&) {
    IceUtil::RWRecMutex::WLock lock(m_admutex);
    bool Rtn = false;
    map<string, string>::iterator iter;
    iter = m_addressbook.find(name);
    if (iter != m_addressbook.end()) {
        iter->second = email;
        Rtn = true;
    }
    if (Rtn) {
        try {
            if (ExistAddressBook4Db(name)) {
                UpdateAddressBook4Db(name, email);
            } else {
                AddAddressBook4Db(name, email);
            }
        } catch (...) {
            MCE_WARN("AddressBookI --> call  updateAddressBook  err");
        }
    }
    return Rtn;
}

bool AddressBookI::deleteAddressBook(const ::std::string& name, const ::Ice::Current&) {
    IceUtil::RWRecMutex::WLock lock(m_admutex);
    map<string, string>::iterator iter;
    iter = m_addressbook.find(name);
    if (iter != m_addressbook.end()) {
        m_addressbook.erase(iter);
        try {
            DeleteAddressBook4Db(name);
        } catch (...) {
            MCE_WARN("AddressBookI --> call  deleteAddressBook  err");
        }
    }
    return true;
}

void AddressBookI::AddAddressBook4Db(const string& strName, const string& strEmail) {
    Statement sql;
    sql << "INSERT INTO " << TABLE_ADDRESSBOOK << " (" << COL_NAME << ", " << COL_EMAIL << ") "
            << " VALUES (" <<mysqlpp::quote<< strName << "," <<mysqlpp::quote<<strEmail.length() << ")";
    QueryRunner(DB_INSTANCE, CDbWServer, TABLE_ADDRESSBOOK).store(sql);
}

void AddressBookI::UpdateAddressBook4Db(const string& strName, const string& strEmail) {

    Statement sql;
    sql << "UPDATE " << TABLE_ADDRESSBOOK << " SET "
            << COL_EMAIL << " = " << mysqlpp::quote << strEmail.length()
            << " WHERE " << COL_NAME << " = " << mysqlpp::quote << strName;
    QueryRunner(DB_INSTANCE, CDbWServer, TABLE_ADDRESSBOOK).store(sql);
}

void AddressBookI::DeleteAddressBook4Db(const string& strName) {
    Statement sql;
    sql << "DELETE FROM " << TABLE_ADDRESSBOOK << " WHERE " << COL_NAME << " = " << mysqlpp::quote << strName;
    QueryRunner(DB_INSTANCE, CDbWServer, TABLE_ADDRESSBOOK).store(sql);
}

bool AddressBookI::GetAddressBook4Db(const string& strName, int & strEmailGet) {
    Statement sql;
    bool rtn = false;
    sql << "SELECT * FROM " << TABLE_ADDRESSBOOK << " WHERE " << COL_NAME << " = " << mysqlpp::quote << strName;
    mysqlpp::StoreQueryResult res = QueryRunner(DB_INSTANCE, CDbRServer, TABLE_ADDRESSBOOK).store(sql);
    if (!res) {
        return rtn;
    }
    if (res.num_rows() > 0) {
        mysqlpp::Row row = res.at(0);
        MCE_WARN("AddressBookI --> call  GetAddressBook4Db row at ");
        strEmailGet = row[COL_NAME];
        MCE_WARN("AddressBookI --> call  GetAddressBook4Db col");
        rtn =  true;
    }
    return rtn;
}

bool AddressBookI::ExistAddressBook4Db(const string& strName) {
    Statement sql;
    sql << "SELECT * FROM " << TABLE_ADDRESSBOOK << " WHERE " << COL_NAME << " = " << mysqlpp::quote << strName;
    mysqlpp::StoreQueryResult res = QueryRunner(DB_INSTANCE, CDbRServer, TABLE_ADDRESSBOOK).store(sql);
    if (!res) {
        return false;
    }
    return res.num_rows() > 0;
}

int AddressBookI::GetABCount() {
    return m_addressbook.size();
}