#ifndef _XCE_SEARCH_ACTION_H_
#define _XCE_SEARCH_ACTION_H_

#include "base/logging.h"
#include "cwf/action.h"
#include "cwf/cookie.h"
#include "UserBase.h"
#include "search/CwfSearch/ParseParam.h"
#include "SearchModel.h"
#include "SearchServer.h"
#include "search/SearchCache2/src/DistSearchCacheLogic/DistSearchCacheAdapter.h"
#include "OceCxxAdapter/src/BuddyByIdReplicaAdapter.h"
#include "OceCxxAdapter/src/LoginLogicAdapter.h"
#include "OceCxxAdapter/src/TicketAdapter.h"
#include "search/FriendOfFriendSearch2/adapter/include/PeopleSearchManagerAdapter.h"
#include "search/FriendOfFriendSearch2/adapter/include/PageSearchManagerAdapter.h"
#include "search/FriendOfFriendSearch2/adapter/include/AppSearchManagerAdapter.h"
#include "search/FriendOfFriendSearch2/adapter/include/SearchManagerAdapter.h"
#include "search/FriendOfFriendSearch2/adapter/include/ExternalSearchManagerAdapter.h"
#include "search/FriendOfFriendSearch2/adapter/include/ExternalPeopleSearchManagerAdapter.h"
#include "search/CwfSearch/Antispam/OzeinFilterAdapter.h"
#include "search/CwfSearch/SearchViewHelper/SearchViewHelper.h"
#include <ctemplate/template.h>
#include <sstream>
#include <boost/regex.hpp>
#include <sys/time.h>


namespace xce {
  namespace search{

    const static std::string SearchDefaultContentType("text/html; charset=utf-8");
    const static std::string QuestionMark("?");
    const static int ItemPerPage = 10;
    const static int AntispamType = 20;
    static boost::regex num_reg("^[0-9]{4,10}$");
    static boost::regex email_reg("^[\\w-]+(\\.[\\w-]+)*@[\\w-]+(\\.[\\w-]+)+$");

    class ResultAction : public cwf::BaseAction {
      public:
        ResultAction() {
          LOG(INFO) << "ResultAction START";
          xce::dist::searchcache::DistSearchCacheAdapter::instance().initialize();
        }

        bool Match(const std::string& url) const {
          //return boost::starts_with(url, "/searchCwf");
          return boost::starts_with(url, "/searchN.do");
        }

        static bool checkId(const string& query){
          return boost::regex_match(query,num_reg);
        }
        static bool checkEmail(const string& query){
          return boost::regex_match(query,email_reg);
        }

        void SetHeader(cwf::Response* res) {
          res->header().set_status_code(cwf::HC_OK, "OK");
          res->header().Add(cwf::HH_CONTENT_TYPE, SearchDefaultContentType);
          res->OutputHeader();
        }

        void ReturnEmpty(cwf::Response* res) {
          SetHeader(res);
          res->WriteRaw("");
        }

        std::string getCountString(int count) {
          std::stringstream countStr;
          if (count > 500) {
            countStr << "500+";
          } else {
            countStr << count;
          }
          return countStr.str();
        }

        std::string int2String(int n) {
          std::stringstream ss;
          std::string s;
          ss << n;
          ss >> s;
          return s;
        }

        long get_now(){
          struct timeval tv;
          memset(&tv, 0, sizeof(tv));
          gettimeofday(&tv, NULL);
          return tv.tv_sec*1000+tv.tv_usec/1000;
        }

        int hashCode(string s) {
          int h = 0;
          int off = 0;
          for (int i = 0; s[i] != '\0'; i++) {
            h = 31 * h + s[off++];
          }
          return h;
        }

        bool isExperiment(int state) {
          return 0 != (state & (1 << xce::userbase::CStateEXPERIMENT));
        }

        bool isAdmin(int id) {
          if (id == 201783098 || id == 223913057 || id == 220840375
              || id == 58742 || id == 202911262 || id == 237767538
              || id == 190136739 || id == 229738704 || id == 223500512
              || id == 234800486 || id == 272744155 || id == 67067835
              || id == 200000032 || id == 202911262) {
            return true;
          }
          return false;
        }

        virtual std::string SetNoResult(int host, const std::string& query, const std::string& param);
        virtual std::string SetAntispamResult(int host, const std::string& query, const std::string& param);
        virtual void SetUserResult(int hostId, ctemplate::TemplateDictionary& dict, const SearchUserViewPtr searchView, int totalCount, int index, const std::string& query, const std::string& param);
        virtual void SetPageResult(int hostId, ctemplate::TemplateDictionary& dict, const SearchPageViewPtr searchView, int totalCount, int index, const std::string& query);
        virtual void SetAppResult(ctemplate::TemplateDictionary& dict, const SearchAppViewPtr searchView, int totalCount, int index, const std::string& query);
        virtual std::map<std::string, std::string> getOpUser(xce::searchcache::dboperation::SearchCacheMemDataPtr data);
        virtual com::xiaonei::search2::model::IndexResultPtr searchIndex(int userId, int offset, const std::string& query, const std::string& param, const std::string& scope, xce::searchcache::dboperation::SearchCacheMemDataPtr data);
        virtual void searchId(const std::string& query, com::xiaonei::search2::model::IndexResultPtr result);
        virtual void searchEmail(const std::string& query, com::xiaonei::search2::model::IndexResultPtr result);
        virtual bool passAntispam(int userId, const std::string& query, const std::string& param, const std::string& remoteAddr);

        virtual cwf::HttpStatusCode Process(cwf::Request*, cwf::Response*);
        ~ResultAction() { }
    };
  }
}

#endif
