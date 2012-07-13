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
#ifndef __PAGE_INPUT_MANAGER_H__
#define __PAGE_INPUT_MANAGER_H__

#include <PageInput.h>
#include "IceExt/src/ServiceI.h"
#include "UtilCxx/src/TaskManager.h"
#include "DbCxxPool/src/QueryRunner.h"
#include <IceUtil/IceUtil.h>

namespace xce {
namespace PageInput {

	const int PAGEINPUT_DATA = 0;
	const std::string DB_FRIENDINPUT_PINYIN = "friendinput_pinyin";
	const std::string DB_PAGE_NAME = "page";
	const std::string DB_FANS_PAGES= "fans_pages";
	const size_t maxSize = 500;

	class PageInputManagerI :public PageInputManager,
	public MyUtil::Singleton<PageInputManagerI> {		
		public:
			PageInputManagerI();
			virtual ~PageInputManagerI(){MCE_INFO("~~PageInput");};
			virtual MyUtil::IntSeq getPageInput(Ice::Int hostID, const std::string& condition, Ice::Int limit, const Ice::Current& crt=Ice::Current());
			std::set<std::string> newSet;
			std::map<std::string,std::vector<std::string> > dict_;
//			std::map<int,std::string> getPageName();
		private:
//			std::map<int, std::string> id2Name_, id2NameNew_;
			friend class MyUtil::Singleton<PageInputManagerI>;
	};
	typedef IceUtil::Handle<PageInputManagerI> PageInputManagerIPtr;

	class PageInputFactoryI: public MyUtil::ServantFactory {
		public:
			PageInputFactoryI();
      virtual Ice::ObjectPtr create(Ice::Int id);
			void generateNameMap(int userId,std::map<int,std::string>&);
			void selectFansPages(int userId,std::vector<int>&);
			void reload();
			void reloadIds(const std::vector<int>&);
			pair<std::string,unsigned char> reloadId();
		private:
//maybe use timer better
		  class UpdateThread : public IceUtil::Thread{
				public:
					UpdateThread(PageInputFactoryI* factory):factory_(factory){};
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
					PageInputFactoryI* factory_;
			};
			IceUtil::RWRecMutex mutex_;
			std::map<int, std::pair<std::string, unsigned char> > id2Name_;
			UpdateThread* thread_;
	};

	class PageInputDataI : virtual public PageInputData {
		public:
			PageInputDataI(){};
			void add(std::map<int,std::string>& result);
			std::vector<int> search(const std::string& query,Ice::Int limit);

		private:
			std::map<std::string,std::vector<int> > postingsHZ;
			std::map<std::string,std::vector<int> > postingsPY;
			std::map<int,std::string> friends;

			void splitWord(const std::string&, std::vector<std::string>&);

			bool startsWith(const std::string& base, const std::string& pre) ;
			void union_vec(std::vector<int>&A, const std::vector<int>& B) ;				
			void intersect_vec(std::vector<int>& A,const std::vector<int>& B);
			vector<int> getPrefix(string query) ;				
			bool isPinYin(const std::string& str, const std::vector<std::string> words) ;

			std::vector<int> select(const std::set<int>& mySet,unsigned int limit);

			bool reChoose(const int& doc,const std::vector<std::string>& strs,const int& len);

			void splitPY(const std::string& pinyin,unsigned int start,int num,std::vector<string> subStr, set<int>& value);		

			std::vector<int> searchHZ(const string& name,const int& limit);


	};
	typedef IceUtil::Handle<PageInputDataI> PageInputDataIPtr;


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
	class PageNameResultHandler : virtual public com::xiaonei::xce::ResultHandler,
			public MyUtil::Singleton<PageNameResultHandler> {
		public:
			PageNameResultHandler(std::map<int,std::pair<std::string,unsigned char> >& id2Name):id2Name_(id2Name){
			}
		protected:
			virtual bool handle(mysqlpp::Row& res) const;
		private:
			std::map<int,std::pair<std::string,unsigned char> >& id2Name_;

	};
	class FansPagesResultHandler : virtual public com::xiaonei::xce::ResultHandler,
			public MyUtil::Singleton<FansPagesResultHandler> {
		public:
			FansPagesResultHandler(std::vector<int>& ids):ids_(ids){
			}
		protected:
			virtual bool handle(mysqlpp::Row& res) const;
		private:
			std::vector<int>& ids_;

	};

};
};
#endif
