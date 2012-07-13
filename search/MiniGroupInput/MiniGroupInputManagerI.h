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
#ifndef __MINIGROUP_INPUT_MANAGER_H__
#define __MINIGROUP_INPUT_MANAGER_H__

#include <PageInput.h>
#include "IceExt/src/ServiceI.h"
#include "UtilCxx/src/TaskManager.h"
#include "DbCxxPool/src/QueryRunner.h"
#include <IceUtil/IceUtil.h>

namespace xce {
namespace minigroup{

	const int MINIGROUPINPUT_DATA = 0;
	const std::string DB_FRIENDINPUT_PINYIN = "friendinput_pinyin";
	const std::string DB_MINIGROUP_NAME = "minigroup_";
	const std::string DB_FANS_MINIGROUPS = "member_minigroup_";
	const std::string DB_MINIGROUP_SOURCE = "minigroup";
	const size_t maxSize = 500;
  const int TIMEOUT_CACHE = 30;


	class MiniGroupInputManagerI :public xce::PageInput::PageInputManager,
	public MyUtil::Singleton<MiniGroupInputManagerI> {		
		public:
			MiniGroupInputManagerI();
			virtual ~MiniGroupInputManagerI(){MCE_INFO("~~MiniGroupInput");};
			virtual MyUtil::IntSeq getPageInput(Ice::Int hostID, const std::string& condition, Ice::Int limit, const Ice::Current& crt=Ice::Current());
			std::set<std::string> newSet;
			std::map<std::string,std::vector<std::string> > dict_;
//			std::map<int,std::string> getPageName();
		private:
//			std::map<int, std::string> id2Name_, id2NameNew_;
			friend class MyUtil::Singleton<MiniGroupInputManagerI>;
	};
	typedef IceUtil::Handle<MiniGroupInputManagerI> MiniGroupInputManagerIPtr;

	class MiniGroupInputFactoryI: public MyUtil::ServantFactory {
		public:
			MiniGroupInputFactoryI();
      virtual Ice::ObjectPtr create(Ice::Int id);
			void generateNameMap(int userId,std::map<int,std::string>&);
			void selectFansMiniGroups(int userId,std::vector<int>&);
			void reload();
			void reloadIds(const std::vector<int>&);
			pair<std::string,unsigned char> reloadId();
		private:
//maybe use timer better
		  class UpdateThread : public IceUtil::Thread{
				public:
					UpdateThread(MiniGroupInputFactoryI* factory):factory_(factory){};
					void run(){
						while(true){
							try{
								sleep(10*60);
								factory_->reload();
							}catch(std::exception& ex){
								MCE_WARN("Update Occurs Exception "<<ex.what());
							}catch(...){
								MCE_WARN("Update Occurs Unknown Exception");
							}	
						}
					}
				private:
					MiniGroupInputFactoryI* factory_;
			};
			IceUtil::RWRecMutex mutex_;
			std::map<int, std::pair<std::string, unsigned char> > id2Name_;
			UpdateThread* thread_;
	};

	class MiniGroupInputDataI : virtual public xce::PageInput::PageInputData {
		public:
			MiniGroupInputDataI(){};
			void add(std::map<int,std::string>& result);
			std::vector<int> search(const std::string& query,Ice::Int limit);

		  time_t getTime()  {
         return old_;
      }

		private:
			std::map<std::string,std::vector<int> > postingsHZ;
			std::map<std::string,std::vector<int> > postingsPY;
			std::map<int,std::string> friends;

			time_t old_;
			void splitWord(const std::string&, std::vector<std::string>&);

			bool startsWith(const std::string& base, const std::string& pre) ;
			void union_vec(std::vector<int>&A, const std::vector<int>& B) ;				
			void intersect_vec(std::vector<int>& A,const std::vector<int>& B);
			vector<int> getPrefix(string query) ;				
			bool isPinYin(const std::string& str, const std::vector<std::string> words) ;

			std::vector<int> select(const std::set<int>& mySet,unsigned int limit);

			bool reChoose(const int& doc,const std::vector<std::string>& strs,const int& len);
			bool reChooseHZ(int doc,const vector<string>& str);

			void splitPY(const std::string& pinyin,unsigned int start,int num,std::vector<string> subStr, set<int>& value);		

			void searchHZ(const vector<string>& words,set<int>& mySet);


	};
	typedef IceUtil::Handle<MiniGroupInputDataI> MiniGroupInputDataIPtr;


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
	class MiniGroupNameResultHandler : virtual public com::xiaonei::xce::ResultHandler,
			public MyUtil::Singleton<MiniGroupNameResultHandler> {
		public:
			MiniGroupNameResultHandler(std::map<int,std::pair<std::string,unsigned char> >& id2Name):id2Name_(id2Name){
			}
		protected:
			virtual bool handle(mysqlpp::Row& res) const;
		private:
			std::map<int,std::pair<std::string,unsigned char> >& id2Name_;

	};
	class FansMiniGroupsResultHandler : virtual public com::xiaonei::xce::ResultHandler,
			public MyUtil::Singleton<FansMiniGroupsResultHandler> {
		public:
			FansMiniGroupsResultHandler(std::vector<int>& ids):ids_(ids){
			}
		protected:
			virtual bool handle(mysqlpp::Row& res) const;
		private:
			std::vector<int>& ids_;

	};

};
};
#endif
