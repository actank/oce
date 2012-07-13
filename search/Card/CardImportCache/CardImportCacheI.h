#ifndef __CARD_IMPORT_CACHEI_H__
#define __CARD_IMPORT_CACHEI_H__

#include <CardCache.h>
#include <Ice/Ice.h>
#include <IceUtil/IceUtil.h>
#include "UtilCxx/src/Singleton.h"
#include "IceExt/src/ObjectCacheI.h"
#include "IceExt/src/TopicI.h"
#include "IceExt/src/ServiceI.h"
#include "DbCxxPool/src/QueryRunner.h"
#include "OceCxxAdapter/src/BuddyByIdReplicaAdapter.h"

namespace xce {
  namespace cardcache {
    const Ice::Int CARD_IMPORT_CACHE = 1;
    const std::string CARD_IMPORT_CACHE_STR = "CIMCS";
    const std::string IMPORT_TABLE_DATA_SOURCE = "addressbook";
    const std::string IMPORT_TABLE_NAME = "address_book_import";

    class findImportInfo {
      public:
        findImportInfo(unsigned char way, std::string detail):_way(way),_detail(detail) {}
        bool operator()(const ImportInfoNewPtr ptr) {
          if ((ptr->importWay == _way) && (ptr->importDetail == _detail))
            return true;
          return false;
        }
      private:
        unsigned char _way;
        std::string _detail;
    };  

    class findDetail {
      public:
        findDetail(std::string detail):_detail(detail) {}
        bool operator()(const ImportInfoNewPtr ptr) {
          if (ptr->importDetail == _detail)
            return true;
          return false;
        }
      private:
        std::string _detail;
    };

    class findMsn {
      public:
        findMsn(std::string msn):_msn(msn) {}
        bool operator()(const ImportInfoNewPtr ptr) {
          if (ptr->msn == _msn)
            return true;
          return false;
        }
      private:
        std::string _msn;
    };

    class findQq {
      public:
        findQq(std::string qq):_qq(qq) {}
        bool operator()(const ImportInfoNewPtr ptr) {
          if (ptr->qq == _qq)
            return true;
          return false;
        }
      private:
        std::string _qq;
    };

    class findEmail {
      public:
        findEmail(std::string email):_email(email) {}
        bool operator()(const ImportInfoNewPtr ptr) {
          if (ptr->email == _email)
            return true;
          return false;
        }
      private:
        std::string _email;
    };

    static int CompareID(const ImportInfoNewPtr &a, const ImportInfoNewPtr &b) {
      if (a->id <= b->id) {
        return 1;
      } else {
        return 0;
      }
    }

    static int CompareID2(const ImportInfoNewPtr &a, const ImportInfoNewPtr &b) {
      if (a->id < b->id) {
        return 1;
      } else {
        return 0;
      }
    }

    // 实现CardImportCache类，并用单例实现
    class CardImportCacheI : virtual public CardImportCache, public MyUtil::Singleton<CardImportCacheI> {
      public:
        CardImportCacheI();
        virtual ~CardImportCacheI() {}
        // 获取Import信息
        // userId: 用户id
        // begin:  开始位置
        // limit： 限制个数
        virtual ::xce::cardcache::ImportInfoSeq get(::Ice::Int userId, ::Ice::Int limit, const Ice::Current&);
       
        virtual ::xce::cardcache::ImportInfoPtr getDetail(::Ice::Int, ::Ice::Byte, const ::std::string&, const ::Ice::Current&);

        virtual ::xce::cardcache::ImportValueSeq getList(::Ice::Int userId, const ::Ice::Current&);
        // 添加import信息
        virtual bool set(const ::xce::cardcache::ImportInfoSeq&, const ::Ice::Current&);
        virtual bool load(const ::xce::cardcache::ImportInfoSeq&, const ::Ice::Current&);
        // 删除Import信息
        // 参数：用户id集合
        virtual void remove(const ::xce::cardcache::ImportInfoSeq&, const ::Ice::Current&);

        virtual ::xce::cardcache::ImportInfoNewSeq getNew(::Ice::Int, ::Ice::Int, const ::Ice::Current&);
        virtual ::xce::cardcache::ImportInfoNewPtr getDetailNew(::Ice::Int, ::Ice::Byte, const ::std::string&, const ::Ice::Current&);
        virtual ::xce::cardcache::ImportValueNewSeq getListNew(::Ice::Int, const ::Ice::Current& );
        virtual bool setNew(const ::xce::cardcache::ImportInfoNewSeq&, const ::Ice::Current&);
        //导入成功返回true，否则返回false
        virtual bool loadNew(const ::xce::cardcache::ImportInfoNewSeq&, const ::Ice::Current&);
        //导入成功返回所导入的对象
        virtual ::xce::cardcache::ImportInfoNewSeq loadII(const ::xce::cardcache::ImportInfoNewSeq&, const ::Ice::Current&);
        virtual ::xce::cardcache::ImportIdImportDetailSeq getIdImportDetail(::Ice::Int, const ::Ice::Current&);
        virtual bool setSubscribeFlag(const ::xce::cardcache::ImportInfoNewSeq& , const ::Ice::Current&);
        virtual bool isContact(::Ice::Int, const ::std::string&, ::Ice::Int, const ::Ice::Current&);
        virtual ::xce::cardcache::ImportValueIISeq getListII(::Ice::Int, const ::Ice::Current& );

        virtual ::MyUtil::IntSeq getImportUser(::Ice::Int, const ::Ice::Current&);
        virtual ::xce::cardcache::ImportEmailSeq getImportEmail(::Ice::Int, ::Ice::Int, const ::Ice::Current&);
        virtual bool isImported(::Ice::Int, const ::Ice::Current&);
        virtual ::MyUtil::IntSeq friendsImported(const ::MyUtil::IntSeq&, const ::Ice::Current&);
        virtual ::xce::cardcache::ImportInfoNewPtr getDetailByEmail(::Ice::Int, const ::std::string&, const ::Ice::Current&);
        virtual ::MyUtil::IntSeq getImportUserId(::Ice::Int, ::Ice::Int, const ::Ice::Current&);
        //求集合并集
        void union_vec(const std::vector<int>& B) {
          MCE_INFO("begin to union importUser.size():" << importUser.size());
          IceUtil::Mutex::Lock lock(_importUserMutex);
          vector<int> C;
          vector<int>::iterator itA=importUser.begin();
          vector<int>::const_iterator itB=B.begin();
          while(itA!=importUser.end()&&itB!=B.end()){
            if((*itA)==(*itB)){
              C.push_back(*itA);
              ++itA;
              ++itB;
            }else if((*itA)<(*itB)){
              C.push_back(*itA);
              ++itA;
            }else{
              C.push_back(*itB);
              ++itB;
            }
          }
          while(itA!=importUser.end()){
            C.push_back(*itA);
            ++itA;
          }
          while(itB!=B.end()){
            C.push_back(*itB);
            ++itB;
          }
          importUser.swap(C);
          MCE_INFO("union done importUser.size():" << importUser.size());
        }

        std::vector<int> importUser;
        ImportEmailSeq importEmails;
      private:

        IceUtil::Mutex _importUserMutex;
    };

    // 构造工厂类，从数据库读取信息
    class CardImportCacheFactory : public MyUtil::ServantFactory {
      public:
        // userId: 用户id
        // 返回结果：该用户对应的import信息
        virtual Ice::ObjectPtr create(Ice::Long userId);
    };

    class CardImportData;
    typedef IceUtil::Handle<xce::cardcache::CardImportData> CardImportDataPtr;
    class CardImportData : public Ice::Object , virtual public IceUtil::Mutex {
      public:
        virtual xce::cardcache::ImportInfoSeq get(Ice::Int limit);
        virtual xce::cardcache::ImportInfoPtr getDetail(::Ice::Byte importWay, const ::std::string& importDetail);
        virtual ::xce::cardcache::ImportValueSeq getList();
        virtual bool set(const ImportInfoNewSeq&);
        virtual bool load(int userId,const ImportInfoNewSeq&);
        virtual int remove(const std::string&, unsigned char);
        
        virtual xce::cardcache::ImportInfoNewSeq getNew(Ice::Int limit);
        virtual xce::cardcache::ImportInfoNewPtr getDetailNew(Ice::Byte importWay, const std::string& importDetail);
        virtual ::xce::cardcache::ImportValueNewSeq getListNew();
        virtual bool setNew(const ImportInfoNewSeq&);
        virtual bool loadNew(int userId,const ImportInfoNewSeq&);
        virtual xce::cardcache::ImportIdImportDetailSeq getIdImportDetail();
        virtual bool setSubscribeFlag(const ImportInfoNewSeq&);
        virtual bool isContact(const std::string& email, int eType);
        virtual xce::cardcache::ImportInfoNewPtr getDetailByEmail(const std::string& email);

        virtual ::xce::cardcache::ImportValueIISeq getListII();
        ImportInfoNewSeq infos;
    };

    // 处理从数据库中读取的结果
    class LoadPersistentByAddressImportInfoHandler : virtual public com::xiaonei::xce::ResultHandler {
      public:
        LoadPersistentByAddressImportInfoHandler(CardImportDataPtr& obj) : _obj(obj) {}
      protected:
        virtual bool handle(mysqlpp::Row& res) const {
          ImportInfoNewPtr info = new ImportInfoNew;
          info->id = res["id"];
          info->userId = res["user_id"];
          info->importWay = (unsigned char)res["import_way"];
          info->importDetail = res["import_detail"].c_str();
          info->msn = res["msn"].c_str();
          info->email = res["email"].c_str();
          info->name = res["name"].c_str();
          info->address = res["address"].c_str();
          info->extra = res["extra"].c_str();
          info->mobile = res["phone"].c_str();
          info->qq = res["qq"].c_str();
          info->requestCount = res["request_count"];
          info->subscribeFlag = (unsigned char)res["subscribe_flag"];

          _obj->infos.push_back(info);

          return true;
        }
      private:
        CardImportDataPtr& _obj;
    };

    class ImportUserResultHandler : virtual public com::xiaonei::xce::ResultHandler {
      public:
        ImportUserResultHandler(std::vector<int>& importUser, int& lastId) : _importUser(importUser), _lastId(lastId) {}
      protected:
        virtual bool handle(mysqlpp::Row& res) const {
          int userId = res["user_id"];
          if (_lastId < userId) {
            _importUser.push_back(userId);
            _lastId = userId;
          }
          return true;
        }
      private:
        std::vector<int>& _importUser;
        int& _lastId;
    };

    class ImportEmailResultHandler : virtual public com::xiaonei::xce::ResultHandler {
      public:
        ImportEmailResultHandler(ImportEmailSeq& importEmails) : _importEmails(importEmails) {}
      protected:
        virtual bool handle(mysqlpp::Row& res) const {
          ImportEmailPtr  ptr = new ImportEmail;
          ptr->id = res["id"];
          ptr->userId = res["user_id"];
          ptr->email = res["email"].c_str();

          _importEmails.push_back(ptr);

          return true;

        }
      private:
        ImportEmailSeq& _importEmails;
    };

    class LoadImportTask : public MyUtil::Task {
      public :
        LoadImportTask(){}
        virtual void handle();
      private:
        void loadImportUser();
        std::vector<int> importUser_;
    };

  }
}

#endif
