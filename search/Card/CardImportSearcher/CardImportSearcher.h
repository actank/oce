#ifndef __CardSearcherI_h__
#define __CardSearcherI_h__

#include <CardSearcher.h>
#include <CardCache.h>
#include "ServiceI.h"
#include "DbCxxPool/src/QueryRunner.h"
#include "OceCxxAdapter/src/CardRelationCacheAdapter.h"
#include "OceCxxAdapter/src/CardImportCacheAdapter.h"
#include "OceCxxAdapter/src/BuddyByNameReplicaAdapter.h"
#include "OceCxxAdapter/src/UserCacheAdapter.h"

namespace xce
{

  namespace cardsearch
  {
    const std::string DB_FRIENDINPUT_PINYIN = "friendinput_pinyin";

    int CARD_IMPORT_SEARCHER = 0;
    size_t MAXBATCH= 500;
    int OUTTIME = 15; 

    class findImportInfo {
      public:
        findImportInfo(unsigned char way, std::string detail, std::string name):_way(way),_detail(detail),_name(name) {}
        bool operator()(const xce::cardcache::ImportValuePtr ptr) {
          if ((ptr->importWay == _way) && (ptr->importDetail == _detail) && (ptr->name == _name))
            return true;
          return false;
        }
      private:
        unsigned char _way;
        std::string _detail;
        std::string _name;
    };

    class findImportInfoNew {
      public:
        findImportInfoNew(unsigned char way, std::string detail, std::string name):_way(way),_detail(detail),_name(name) {}
        bool operator()(const xce::cardcache::ImportValueNewPtr ptr) {
          if ((ptr->importWay == _way) && (ptr->importDetail == _detail) && (ptr->name == _name))
            return true;
          return false;
        }
      private:
        unsigned char _way;
        std::string _detail;
        std::string _name;
    };


    /// @brief 定义CardSearcher的方法
    class CardImportSearcherI : virtual public CardImportSearcher,public MyUtil::Singleton<CardImportSearcherI>
    {
      public:
        CardImportSearcherI();
        /// @brief search 搜索的接口 
        /// 
        /// @param ::Ice::Int userId 用户ID
        /// @param ::std::string 搜索条件
        /// @param ::Ice::Int 返回的结果数量
        /// @param Ice::Current
        /// 
        /// @return 好友ID与名片交换状态的map
        virtual ::xce::cardcache::ImportValueSeq searchImport(::Ice::Int, const ::std::string&, ::Ice::Int, const ::Ice::Current&);
        virtual ::xce::cardcache::ImportValueNewSeq searchImportNew(::Ice::Int, const ::std::string&, ::Ice::Int, const ::Ice::Current&);
        void loadPY();

        std::set<std::string> newSet;
        std::map<std::string,std::vector<std::string> > dict_;

    };

    class CardImportSearcherData : public Ice::Object {
      public:
        CardImportSearcherData(): indexTime(time(NULL)) {}
        void addIndex(const xce::cardcache::ImportInfoSeq&);
        void addPYIndex(const std::string,const xce::cardcache::ImportValuePtr&);

        void addIndexNew(const xce::cardcache::ImportInfoNewSeq&);
        void addPYIndexNew(const std::string,const xce::cardcache::ImportValueNewPtr&);
        /// @brief startsWith 判断用户查询的query是否存在
        /// 
        /// @param string base 索引中的信息
        /// @param string pre 用户传入的查询条件
        /// 
        /// @return 若存在则返回true
        bool startsWith(const string& ,const string&);

        /// @brief search 提供方法给CardSearcherI::Search调用
        /// 
        /// @param ::std::string query 用户查询条件
        /// @param limit 需要返回的结果大小
        /// 
        /// @return 返回用户ID与名片交换状态的map
        ::xce::cardcache::ImportValueSeq search(const ::std::string&, int limit);
        void searchPY(xce::cardcache::ImportValueSeq&, const string& ,int limit);

        bool isExist(const xce::cardcache::ImportValuePtr& ptr, const xce::cardcache::ImportValueSeq& values);

        ::xce::cardcache::ImportValueNewSeq searchNew(const ::std::string&, int limit);
        void searchPYNew(xce::cardcache::ImportValueNewSeq&, const string& ,int limit);

        bool isExistNew(const xce::cardcache::ImportValueNewPtr& ptr, const xce::cardcache::ImportValueNewSeq& values);
        /// @brief shouldOut 
        /// 
        /// @return 
        bool shouldOut();
      private:
        std::map<std::string,std::vector<xce::cardcache::ImportValuePtr> > dict;
        std::map<std::string,std::vector<xce::cardcache::ImportValuePtr> > postingsPY;
        int indexTime;

        std::map<std::string,std::vector<xce::cardcache::ImportValueNewPtr> > dictNew;
        std::map<std::string,std::vector<xce::cardcache::ImportValueNewPtr> > postingsPYNew;
        
        void splitWord(const std::string&, std::vector<std::string>&);

        /// @brief intersect_vec求集合A、B的交集, 交集赋值给集合A
        /// 
        /// @param A(in&out) 
        /// @param B (in)

        void intersect_vec(std::set<xce::cardcache::ImportValuePtr>& A,const std::set<xce::cardcache::ImportValuePtr>& B)
        {
          set<xce::cardcache::ImportValuePtr> C;
          set<xce::cardcache::ImportValuePtr>::iterator itA;
          for (std::set<xce::cardcache::ImportValuePtr>::const_iterator itB = B.begin(); itB != B.end(); ++itB) {
            unsigned char way = (*itB)->importWay;
            std::string detail = (*itB)->importDetail;
            std::string name = (*itB)->name;
            itA = std::find_if(A.begin(), A.end(), findImportInfo(way, detail, name));
            if (itA != A.end()) {
              C.insert(*itA);
            }
          }
          A.swap(C);
        }

        void intersect_vecNew(std::set<xce::cardcache::ImportValueNewPtr>& A,const std::set<xce::cardcache::ImportValueNewPtr>& B)
        {
          set<xce::cardcache::ImportValueNewPtr> C;
          set<xce::cardcache::ImportValueNewPtr>::iterator itA;
          for (std::set<xce::cardcache::ImportValueNewPtr>::const_iterator itB = B.begin(); itB != B.end(); ++itB) {
            unsigned char way = (*itB)->importWay;
            std::string detail = (*itB)->importDetail;
            std::string name = (*itB)->name;
            itA = std::find_if(A.begin(), A.end(), findImportInfoNew(way, detail, name));
            if (itA != A.end()) {
              C.insert(*itA);
            }
          }
          A.swap(C);
        }


        /// @brief getPrefix 在postingsPY中找到所有以query为前缀的拼音，将这些拼音对应的ID求并集然后返回
        /// 
        /// @param query 
        /// 
        /// @return
        set<xce::cardcache::ImportValuePtr> getPrefix(string query) {
          set<xce::cardcache::ImportValuePtr> tmp;
          map<string,vector<xce::cardcache::ImportValuePtr> >::iterator it=postingsPY.lower_bound(query);
          while(it!=postingsPY.end()) {
            if(startsWith(it->first,query))
              tmp.insert(it->second.begin(),it->second.end());
            else
              break;
            ++it;
          }
          return tmp;
        }

        set<xce::cardcache::ImportValueNewPtr> getPrefixNew(string query) {
          set<xce::cardcache::ImportValueNewPtr> tmp;
          map<string,vector<xce::cardcache::ImportValueNewPtr> >::iterator it=postingsPYNew.lower_bound(query);
          while(it!=postingsPYNew.end()) {
            if(startsWith(it->first,query))
              tmp.insert(it->second.begin(),it->second.end());
            else
              break;
            ++it;
          }
          return tmp;
        }

        /// @brief isPinYin 判断words中的字符是否都是[a, z]区间内的, 若都是，返回true，否则返回false
        /// 
        /// @param str(in) 初始字符串(可能为中文、英文或中英文混杂)
        /// @param words(in) 将str切分成最小单字单元(单汉字或者单字母)
        /// 
        /// @return 全部都是小写字母则返回true，否则返回false

        bool isPinYin(const std::string& str, const std::vector<std::string> words) {
          if(words.size()<str.size())
          {
            return false;
          }
          else {
            for(std::size_t i=0;i<words.size();++i) {
              if(words[i]<"a"||words[i]>"z")
              {
                return false;
              }
            }
            return true;
          }
        }

        void select(std::vector<xce::cardcache::ImportValuePtr>& res, const std::set<xce::cardcache::ImportValuePtr>& mySet,unsigned int limit);
        void selectNew(std::vector<xce::cardcache::ImportValueNewPtr>& res, const std::set<xce::cardcache::ImportValueNewPtr>& mySet,unsigned int limit);

        bool reChoose(const int& doc,const std::vector<std::string>& strs,const int& len);

        void splitPY(const std::string& pinyin,unsigned int start,int num,std::vector<string> subStr, set<xce::cardcache::ImportValuePtr>& value);
        void splitPYNew(const std::string& pinyin,unsigned int start,int num,std::vector<string> subStr, set<xce::cardcache::ImportValueNewPtr>& value);



    };
    typedef IceUtil::Handle<CardImportSearcherData> CardImportSearcherDataPtr;

    class CardImportSearcherFactoryI : public MyUtil::ServantFactory {
      public:
        /// @brief create 当某个用户的索引不存在时，调用该函数建立其索引
        /// 
        /// @param userId 用户ID
        /// 
        /// @return 返回object
        virtual Ice::ObjectPtr create(::Ice::Long userId);
    };

    class PinYinResultHandler : virtual public com::xiaonei::xce::ResultHandler,
    public MyUtil::Singleton<PinYinResultHandler> {
      public:
        PinYinResultHandler(std::set<std::string>& newSet, std::map<std::string,std::vector<std::string> >& dict):newSet(newSet), dict_(dict){}
      protected:
        virtual bool handle(mysqlpp::Row& res) const;
      private:
        std::set<std::string>& newSet;
        std::map<std::string,std::vector<std::string> >& dict_;
    };

  }

}

#endif
