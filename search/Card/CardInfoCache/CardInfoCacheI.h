#ifndef __CARD_INFO_CACHEI_H__
#define __CARD_INFO_CACHEI_H__

#include <CardCache.h>
#include <Ice/Ice.h>
#include <IceUtil/IceUtil.h>
#include <string>
#include <sstream>
#include "UtilCxx/src/Singleton.h"
#include "IceExt/src/ObjectCacheI.h"
#include "IceExt/src/TopicI.h"
#include "IceExt/src/ServiceI.h"
#include "DbCxxPool/src/QueryRunner.h"
#include "search/Card/share/ParsePrivacy.h"

namespace xce {
  namespace cardcache {

    /// @brief 定义需要访问的数据源和表名
    const Ice::Int CARD_INFO_CACHE = 0;
    const std::string CARD_INFO_CACHE_STR = "CINC";
    const std::string CARD_TABLE_NAME = "address_vcard";
    const std::string DB_DESC_ADDRESSBOOK = "addressbook";
    const std::string CONTACT_TABLE_NAME = "user_contact";
    const std::string DB_DESC_CONTACT = "user_contact";

    const std::string ADDRESS = "address";
    const std::string QQ = "qq";
    const std::string MOBILE = "mobile";
    const std::string EMAIL = "email";
    const std::string MSN = "msn";
    const std::string EXTRA = "extra";
    const std::string PRIVACY = "privacy";

    /// @brief 定义CardInfoCache的方法、数据成员
    class CardInfoCacheI : virtual public CardInfoCache, public MyUtil::Singleton<CardInfoCacheI> {
      public:
        CardInfoCacheI() {}
        virtual ~CardInfoCacheI() {}

        /// @brief get 从CardInfoCache中批量取用户的名片信息
        /// 
        /// @param ::MyUtil::IntList 传入一批用户ID
        /// @param Ice::Current
        /// 
        /// @return 返回用户名片信息的向量
        virtual ::xce::cardcache::CardInfoSeq get(const ::MyUtil::IntList&, const Ice::Current&);

        /// @brief setProperty 给UserLogic提供修改Contact相关信息和个人主页隐私的接口，方便数据同步 
        /// 
        /// @param userId
        /// @param props 需要修改的字段和相应的值
        /// @param Ice::Current
        virtual void setProperty(int userId, const MyUtil::Str2StrMap& props, const Ice::Current&);

        /// @brief getPrivacy 根据用户ID从Cache中取出用户的隐私设置
        /// 
        /// @param userId
        /// @param Ice::Current
        /// 
        /// @return 返回用户的隐私设置
        virtual std::string getPrivacy(Ice::Int userId, const Ice::Current&);

        /// @brief setPrivacy 修改Cache中的用户隐私
        /// 
        /// @param userId
        /// @param privacy
        /// @param Ice::Current
        /// 
        /// @return 若修改成功则返回true，否则返回false
        virtual bool setPrivacy(int userId, const std::string& privacy, const Ice::Current&);

        /// @brief getPart 从Cache中取出用户Id、qq、手机和隐私 
        /// 
        /// @param ::MyUtil::IntList 用户id的向量
        /// @param ::Ice::Current
        /// 
        /// @return 返回id、qq、手机和隐私的向量 
        virtual ::xce::cardcache::PartOfCardInfoSeq getPart(const ::MyUtil::IntList&, const ::Ice::Current&);

        /// @brief set 调用CardDBAdapte中的接口向数据库中更新名片信息并想Cache中更新
        /// 
        /// @param ::xce::cardcache::CardInfoSeq 名片信息向量
        /// @param ::Ice::Current
        /// 
        /// @return 若更新成功则返回true
        virtual bool set(const ::xce::cardcache::CardInfoSeq&, const ::Ice::Current&);

        /// @brief remove 删除名片信息（预留的接口） 
        /// 
        /// @param ::MyUtil::IntSeq
        /// @param Ice::Current
        virtual void remove(const ::MyUtil::IntSeq&, const Ice::Current&);
        virtual bool isValid(const Ice::Current&);
        virtual void setValid(bool newState, const Ice::Current&);
      private:
        ParsePrivacy parse;
        IceUtil::RWRecMutex _validMutex;
        bool _isValid;
    };

    /// @brief 
    class CardInfoCacheFactory : public MyUtil::ServantFactory {
      public:

        /// @brief create 当向Cache中取不到对象时调用create添加一个对 
        /// 
        /// @param userId
        /// 
        /// @return 
        virtual Ice::ObjectPtr create(Ice::Long userId);

        /// @brief readCardInfo 从数据库中读取名片信息放入CarcInfoPtr中
        /// 
        /// @param userId (in)
        /// @param CardInfoPtr (out)
        /// 
        /// @return 成功返回true
        virtual bool readCardInfo(int userId, CardInfoPtr&);

        /// @brief readUserContactInfo 从数据库中读取用户之间的关系放入CardInfoPtr中
        /// 
        /// @param userId (in)
        /// @param CardInfoPtr (out)
        /// 
        /// @return 成功返回true
        virtual bool readUserContactInfo(int userId, CardInfoPtr& );

        /// @brief readPrivacy 从数据库中读取用户的隐私放入CardInfoPtr中
        /// 
        /// @param userId (in)
        /// @param CardInfoPtr (out)
        /// 
        /// @return 成功返回true
        virtual bool readPrivacy(int userId, CardInfoPtr&);
      private:
        ParsePrivacy privacy_;
    };

    class CardInfoCacheEntry;
    typedef IceUtil::Handle<CardInfoCacheEntry> CardInfoCacheEntryPtr;

    class CardInfoCacheEntry : public Ice::Object {
      public:
        /// @brief CardInfoCacheEntry 构造函数，把一个CardInfo对象复制给另一个对象
        CardInfoCacheEntry() {}
        CardInfoCacheEntry(const CardInfoCacheEntryPtr& entry);
        CardInfoCacheEntry(const CardInfoPtr& info);

        /// @brief  set用户名片信息
        void set_address(std::string address) {address_ = address;}
        void set_qq(std::string qq) {qq_ = qq;}
        void set_mobile(std::string mobile) {mobile_ = mobile;}
        void set_email(std::string email) {email_ = email;}
        void set_msn(std::string msn) {msn_ = msn;}
        void set_extra(std::string extra) {extra_ = extra;}
        void set_privacy(std::string privacy) {privacy_ = privacy;}

        /// @brief  读取用户名片信息
        std::string get_address() {return address_;}
        std::string get_qq() {return qq_;}
        std::string get_mobile() {return mobile_;}
        std::string get_email() {return email_;}
        std::string get_msn() {return msn_;}
        std::string get_extra() {return extra_;}
        std::string get_privacy() {return privacy_;}
      private:
        std::string address_;
        std::string qq_;
        std::string mobile_;
        std::string email_;
        std::string msn_;
        std::string extra_;
        std::string privacy_;
    };

    /// @brief 处理从数据库中读出来的名片信息
    class LoadPersistentByCardInfoHandler: virtual public com::xiaonei::xce::ResultHandler {
      public:
        LoadPersistentByCardInfoHandler(CardInfoPtr& info) : _info(info) {}
      protected:
        virtual bool handle(mysqlpp::Row& res) const {
          _info->address = res["address"].c_str();
          _info->email = res["email"].c_str();
          _info->extra = res["extra"].c_str();
          return true;
        }
      private:
        CardInfoPtr& _info;
    };

    /// @brief 处理从数据库中读出来的用户关系信息
    class LoadPersistentByUserContactInfoHandler: virtual public com::xiaonei::xce::ResultHandler {
      public:
        LoadPersistentByUserContactInfoHandler(CardInfoPtr& info) : _info(info) {};
      protected:
        virtual bool handle(mysqlpp::Row& res) const {
          _info->msn = res["msn"].c_str();
          _info->mobile = res["mobilephone"].c_str();
          _info->qq = res["qq"].c_str();
          return true;
        }
      private:
        CardInfoPtr& _info;
    };

    /// @brief 处理从数据库中读出来的隐私信息
    class LoadProfilePrivacyHandler : virtual public com::xiaonei::xce::ResultHandler {
      public:
        LoadProfilePrivacyHandler(CardInfoPtr& info, ParsePrivacy& parse) : _info(info), _parse(parse) {}
      protected:
        virtual bool handle(mysqlpp::Row& res) const {
          std::string privacy = res["profile_privacy"].c_str();
          _info->privacy = _parse.getJsonStr(privacy);
          return true;	
        }
      private:
        CardInfoPtr& _info;
        ParsePrivacy& _parse;
    };

    class UpdateTask: public MyUtil::Task {
      public:
        UpdateTask(const CardInfoPtr& data, const Ice::Current& current):data_(data),current_(current) {}
        virtual void handle();
      private:
        void UpdateData(CardInfoCacheEntryPtr& data, const CardInfoPtr&);
        CardInfoPtr data_;
        Ice::Current current_;
    }; 

  }
}

#endif
