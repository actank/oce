/*
 * =====================================================================================
 *
 *       Filename:  FriendInputManagerI.h
 *
 *    Description:  :
 *
 *        Version:  1.0
 *        Created:  2009年07月29日 14时40分32秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  tanbokan (), bokan.tan@opi-corp.com
 *        Company:  opi-corp.com
 *
 * =====================================================================================
 */
#ifndef __FRIEND_INPUT_MANAGER_H__
#define __FRIEND_INPUT_MANAGER_H__

#include <FriendInput.h>
#include "IceExt/src/ServiceI.h"
#include "util/cpp/TimeCost.h"
#include "UtilCxx/src/TaskManager.h"
#include "DbCxxPool/src/QueryRunner.h"
#include <IceUtil/IceUtil.h>

namespace com {
namespace xiaonei {
namespace services {
namespace FriendInput {
  using namespace MyUtil;
	const int FRIENDINPUT_DATA = 0;
	const std::string DB_FRIENDINPUT_PINYIN = "friendinput_pinyin";

	const size_t maxSize = 300;
  const int TIMEOUT_CACHE = 30;

	class FriendInputManagerI :public FriendInputManager,
	public MyUtil::Singleton<FriendInputManagerI> {		
		public:
			FriendInputManagerI();
			virtual ~FriendInputManagerI(){MCE_INFO("~~FriendInput");};
			virtual MyUtil::IntSeq getFriendInput(Ice::Int hostID, const std::string& condition, Ice::Int limit, const Ice::Current& crt=Ice::Current());
			virtual void reLoadFriend(Ice::Int hostID, const Ice::Current& crt=Ice::Current());
			std::set<std::string> newSet;
			std::vector<std::string> dict(const string& );
		private:
			std::map<std::string,std::vector<std::string> > dict_;
			friend class MyUtil::Singleton<FriendInputManagerI>;
	};
	typedef IceUtil::Handle<FriendInputManagerI> FriendInputManagerIPtr;

	class FriendInputFactoryI: public MyUtil::ServantFactory {
		public:
			virtual Ice::ObjectPtr create(Ice::Int id);
			void generateNameMap(int userId,std::map<int,std::string>&);
	};

	class FriendInputDataI : virtual public FriendInputData {
		public:
			FriendInputDataI(int id):id_(id){ alphabetInclude = false;};
			void add(std::map<int,std::string>& result);
			std::vector<int> search(const std::string& query,Ice::Int limit);
      		time_t getTime()  {
        		return old_;
      		}

		private:
			int id_;
			std::map<std::string,std::vector<int> > postingsHZ;
      /// @brief 纯汉字的拼音到ID的映射
			std::map<std::string,std::vector<int> > postingsPY;
      /// 好友ID->好友姓名的映射
			std::map<int,std::string> friends;
      		time_t old_;
      
      bool alphabetInclude;  //好友名字中是否有字母
      std::vector<std::string> alphabetInName;  //好友名字中的字母
      std::map<std::string,std::vector<int> > postingsMix;  //好友名字中汉字拼音混杂的情况

			void splitWord(const std::string&, std::vector<std::string>&);

      /// @brief startsWith 判断字符串base是否以pre为前缀
      /// 
      /// @param base 
      /// @param pre 
      /// 
      /// @return 
			bool startsWith(const std::string& base, const std::string& pre) {
				if (base.size()<pre.size()) {
					return false;
				}
				int pc=pre.size();
				int po=0;
				while (--pc >= 0) {
					if (base[po] != pre[po++] ) {
						return false;
					}
				}
				return true;
			}
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
			void intersect_vec(std::vector<int>& A,const std::vector<int>& B)
			{
				vector<int> C;
				vector<int>::iterator itA=A.begin();
				vector<int>::const_iterator itB=B.begin();
				while(itA!=A.end()&&itB!=B.end()) {
					if((*itA)==(*itB)) {
						C.push_back(*itA);	
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
			vector<int> getPrefix(string query) {
				vector<int> tmp;
				map<string,vector<int> >::iterator it=postingsPY.lower_bound(query);
				while(it!=postingsPY.end()) {
					if(startsWith(it->first,query))
						union_vec(tmp,it->second);	
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
			bool isPinYin(const std::vector<std::string> words) {
				for(std::size_t i=0;i<words.size();++i) 
					if(words[i]<"a"||words[i]>"z")
						return false;
				return true;
			}

			std::vector<int> select(const std::map<int,int>& mySet,unsigned int limit);

			bool reChoose(int doc,const std::vector<std::string>& strs,const int& len,int& first);
			bool reChooseHZ(int doc,const std::vector<std::string>& strs,int& first);

			void splitPY(const std::string& pinyin,unsigned int start,int num,std::vector<string> subStr, map<int,int>& value);		

			void searchHZ(const vector<string>& words,map<int,int>& value);


	};
	typedef IceUtil::Handle<FriendInputDataI> FriendInputDataIPtr;


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
};
};
};
};
#endif
