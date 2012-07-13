#ifndef __CardSearcherI_h__
#define __CardSearcherI_h__

#include <CardSearcher.h>
#include <CardCache.h>
#include "ServiceI.h"
#include "DbCxxPool/src/QueryRunner.h"
#include "OceCxxAdapter/src/CardRelationCacheAdapter.h"
#include "OceCxxAdapter/src/CardInfoCacheAdapter.h"
#include "OceCxxAdapter/src/BuddyByNameReplicaAdapter.h"
#include "OceCxxAdapter/src/UserCacheAdapter.h"

namespace xce
{

  namespace cardsearch
  {
    /// @brief 定义Str2StrMap的key
    std::string NAME = "name";
    std::string QQ = "qq";
    std::string MSN = "msn";
    std::string MOBILE = "mobile";
    std::string ADDRESS = "address";
    std::string EMAIL = "email";

    const std::string DB_FRIENDINPUT_PINYIN = "friendinput_pinyin";

    int CARD_SEARCHER = 0;
    size_t MAXBATCH= 500;
    int OUTTIME = 15; 

    class CardSearcherInfo {
      public:
        std::string name;
        std::string qq;
        std::string mobile;
        std::string msn;
        std::string email;
        std::string address;
    };
    typedef IceUtil::Handle<CardSearcherInfo> CardSearcherInfoPtr;

    /// @brief lessCardInfo 为lower_bound提供比较函数
    /// 
    /// @param A
    /// @param B
    /// 
    /// @return 
    bool lessCardInfo(const xce::cardcache::CardInfoPtr& A, const xce::cardcache::CardInfoPtr& B){
      return A->userId < B->userId;
    }

    bool lessPartOfCardInfo(const xce::cardcache::PartOfCardInfoPtr& A, const xce::cardcache::PartOfCardInfoPtr& B){
      return A->userId < B->userId;
    }

    /// @brief 定义CardSearcher的方法
    class CardSearcherI : virtual public CardSearcher,public MyUtil::Singleton<CardSearcherI>
    {
      public:
        CardSearcherI();
        /// @brief search 搜索的接口 
        /// 
        /// @param ::Ice::Int userId 用户ID
        /// @param ::std::string 搜索条件
        /// @param ::Ice::Int 返回的结果数量
        /// @param Ice::Current
        /// 
        /// @return 好友ID与名片交换状态的map
        virtual MyUtil::Int2IntMap search(::Ice::Int,
            const ::std::string&,
            ::Ice::Int,
            const Ice::Current&);
        
        /// @brief getFriendList 取交换名片、发送交换请求、收到交换请求和没有任何请求的好友信息
        /// 
        /// @param Ice::Int userId 用户ID 
        /// @param Ice::Current
        /// 
        /// @return 返回好友信息
        virtual FriendCardListPtr getFriendList(Ice::Int, const Ice::Current&);

        /// @brief getUnExchangeList 获取没有任何名片交换操作的好友信息
        /// 
        /// @param Ice::Int userId 用户ID
        /// @param Ice::Current
        /// 
        /// @return 返回好友ID
        virtual MyUtil::IntList getUnExchangeList(Ice::Int, const Ice::Current&);

        virtual ::xce::cardcache::PartNewInfoSeq getFriendsList(::Ice::Int, const ::Ice::Current&);

        /// @brief getUserCache 获取用户信息
        /// 
        /// @param MyUtil::IntList 用户好友ID向量 
        /// @param std::map 用户信息map
        void getUserCache(const MyUtil::IntList&, std::map<int,xce::usercache::UserCachePtr>&);

        void loadPY();

        std::set<std::string> newSet;
        std::map<std::string,std::vector<std::string> > dict_;

      private:
        /// @brief getPartOfCardInfo 获取用户的ID、qq、手机、隐私、名字
        ///  
        /// @param int userId 用户ID
        /// @param xce::cardcache::PartOfCardInfoSeq 用户名片信息
        /// @param string 用户姓名
        /// 
        /// @return 
        xce::cardcache::PartInfoPtr getPartOfCardInfo(int,const xce::cardcache::PartOfCardInfoSeq&,const string&);
    };

    class CardSearcherData : public Ice::Object {
      public:
        CardSearcherData(): indexTime(time(NULL)) {}

        /// @brief addBaseIndex 建立索引
        /// 
        /// @param userId 某用户的好友ID
        /// @param MyUtil::Str2StrMap ID对应的信息名与值的map
        /// @param type 名片交换状态
        void addBaseIndex(int userId,const MyUtil::Str2StrMap&, int type);
        void addPYIndex(int userId,const std::string, int type);

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
        MyUtil::Int2IntMap search(const ::std::string&, int limit);
        void searchPY(MyUtil::Int2IntMap&, const string& ,int limit);

        /// @brief shouldOut 
        /// 
        /// @return 
        bool shouldOut();
      private:
        std::map<std::string,std::vector<int> > dict;
        std::map<int,int> id2type;
        int indexTime;


        std::map<std::string,std::vector<int> > postingsPY;

        void splitWord(const std::string&, std::vector<std::string>&);

        void union_vec(std::vector<int>&A, const std::vector<int>& B) {
          vector<int> C;
          vector<int>::iterator itA=A.begin();
          vector<int>::const_iterator itB=B.begin();
          while(itA!=A.end()&&itB!=B.end()){
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
          while(itA!=A.end()){
            C.push_back(*itA);
            ++itA;
          }
          while(itB!=B.end()){
            C.push_back(*itB);
            ++itB;
          }
          A.swap(C);
        }

        /// @brief intersect_vec求集合A、B的交集, 交集赋值给集合A
        /// 
        /// @param A(in&out) 
        /// @param B (in)

        void intersect_vec(std::set<int>& A,const std::set<int>& B)
        {
          set<int> C;
          set<int>::iterator itA=A.begin();
          set<int>::const_iterator itB=B.begin();
          while(itA!=A.end()&&itB!=B.end()) {
            if((*itA)==(*itB)) {
              C.insert(*itA);
              ++itA;
              ++itB;
            }else if((*itA)>(*itB)) {
              ++itB;
            }else {
              ++itA;
            }
          }
          A.swap(C);
        }

        /// @brief getPrefix 在postingsPY中找到所有以query为前缀的拼音，将这些拼音对应的ID求并集然后返回
        /// 
        /// @param query 
        /// 
        /// @return
        set<int> getPrefix(string query) {
          set<int> tmp;
          map<string,vector<int> >::iterator it=postingsPY.lower_bound(query);
          while(it!=postingsPY.end()) {
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

        void select(std::vector<int>& res, const std::set<int>& mySet,unsigned int limit);

        bool reChoose(const int& doc,const std::vector<std::string>& strs,const int& len);

        void splitPY(const std::string& pinyin,unsigned int start,int num,std::vector<string> subStr, set<int>& value);



    };
    typedef IceUtil::Handle<CardSearcherData> CardSearcherDataPtr;

    class CardSearcherFactoryI : public MyUtil::ServantFactory {
      public:
        /// @brief create 当某个用户的索引不存在时，调用该函数建立其索引
        /// 
        /// @param userId 用户ID
        /// 
        /// @return 返回object
        virtual Ice::ObjectPtr create(::Ice::Long userId);

        /// @brief addIndex 调用CardSearcherData::addBaseIndex建立索引
        /// 
        /// @param CardSearcherDataPtr props
        /// @param std::vector ids 好友ID的向量
        /// @param map users 用户信息
        /// @param xce::cardcache::CardInfoSeq 用户名片信息
        /// @param type 名片交换状态
        void addIndex(CardSearcherDataPtr&, const std::vector<int>&, const map<int, xce::usercache::UserCachePtr>&, const xce::cardcache::CardInfoSeq&, int type);
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
