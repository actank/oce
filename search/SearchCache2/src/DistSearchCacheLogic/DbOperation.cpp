/*
 * =====================================================================================
 *
 *       Filename:  DbOperation.cpp
 *
 *    Description:  Basic Operation of reading db
 *
 *        Version:  1.0
 *        Created:  2010年12月04日 17时13分55秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  lijie (), lijie@opi-corp.com
 *        Company:  Opi-Corp
 *
 * =====================================================================================
 */
#include "DistSearchLogicI.h"
#include "DbOperation.h"
#include "execinfo.h"
#include "DistSearchCacheAdapter.h"
#include "ZoieUpdater.h"
#include "search/tripodsearchcache/TripodSearchUpdateAdapter.h"

//***************************************************************************

//全局函数将更新时需要查询mysql的多个语句模块化
namespace xce{
  namespace searchcache{
    namespace dboperation{
      using namespace IceUtil;
      using std::queue;
      using namespace std;
      using namespace mysqlpp;
      using namespace xce::dist::searchcache;
      using namespace oce::searchcache;
      using namespace search::update;
      using namespace search::tripodupdate;
      //***************************************************************************

      string SearchCacheMemData::splitProfilePrivacy(std::string profile_privacy)  {
        string bew_privacy = "";//仅判断basic,education和workplace三种隐私
        string::size_type indexB = profile_privacy.find("b");
        string::size_type indexE = profile_privacy.find("e");
        string::size_type indexW= profile_privacy.find("w");
        string::size_type indexWait;
        bew_privacy += "{";
        if(indexB != string::npos)  {
          indexWait = profile_privacy.find(",",indexB);
          if(indexWait == string::npos)  {
            indexWait =profile_privacy.find("}",indexB);
          }
          bew_privacy += profile_privacy.substr(indexB, indexWait - indexB);
          bew_privacy += ",";
        }
        if(indexE != string::npos)  {
          indexWait = profile_privacy.find(",", indexE);
          if(indexWait == string::npos)  {
            indexWait =profile_privacy.find("}",indexB);
          }
          bew_privacy += profile_privacy.substr(indexE, indexWait - indexE);
          bew_privacy += ",";
        }
        if(indexW != string::npos)  {
          indexWait = profile_privacy.find(",", indexW);
          if(indexWait == string::npos)  {
            indexWait =profile_privacy.find("}",indexB);
          }
          bew_privacy += profile_privacy.substr(indexW, indexWait - indexW);
        }
        bew_privacy += "}";
        return bew_privacy;
      }

      void SearchCacheMemData::setUserPassportProperties(mysqlpp::Row& row)  {
        const char* status = "status";
        const char* safe_status = "safe_status";
        RWRecMutex::WLock lock(_dataMutex);
        int status_tmp, safe_status_tmp;
        status_tmp = (int) row[status];
        safe_status_tmp = (int) row[safe_status];
        set_status(status_tmp);
        set_safestatus(safe_status_tmp);
        in_tables_++;
      }

      void SearchCacheMemData::setConfigProperties(mysqlpp::Row& row) {
        const char* browse_config = "browse_config";
        const char* status_config = "status_config";
        const char* basic_config = "basic_config";
        const char* profile_privacy = "profile_privacy";
        RWRecMutex::WLock lock(_dataMutex);
        int browse_config_tmp, status_config_tmp, basic_config_tmp;
        std::string profile_privacy_tmp;
        browse_config_tmp = (int) row[browse_config];
        status_config_tmp = (int) row[status_config];
        basic_config_tmp = (int) row[basic_config];
        profile_privacy_tmp = splitProfilePrivacy((std::string) row[profile_privacy]);
        set_browseconfig(browse_config_tmp);
        set_statusconfig(status_config_tmp);
        set_basicconfig(basic_config_tmp);
        set_profileprivacy(profile_privacy_tmp);
        in_tables_++;
      }

      void SearchCacheMemData::setUserUrlProperties(mysqlpp::Row& row) {
        RWRecMutex::WLock lock(_dataMutex);
        const char* headurl = "headurl";
        const char* tinyurl = "tinyurl";
        std::string headurl_tmp, tinyurl_tmp;
#ifndef NEWARCH
        headurl_tmp = row[headurl].get_string();
        tinyurl_tmp = row[tinyurl].get_string();
#else
        headurl_tmp = row[headurl].c_str();
        tinyurl_tmp = row[tinyurl].c_str();
#endif
        set_headurl(headurl_tmp);
        set_tinyurl(tinyurl_tmp);
      }

      void SearchCacheMemData::setUserStateProperties(mysqlpp::Row& row){
        const char* state = "state";
        int state_tmp;
        RWRecMutex::WLock lock(_dataMutex);
        state_tmp = (int) row[state];
        set_state(state_tmp);
      }

      void SearchCacheMemData::setUserStatusProperties(mysqlpp::Row& row) {
        const char* status = "status";
        const char* safe_status = "safe_status";
        int status_tmp, safe_status_tmp;
        RWRecMutex::WLock lock(_dataMutex);
        status_tmp = (int)row[status];
        safe_status_tmp = (int)row[safe_status];
        set_status(status_tmp);
        set_safestatus(safe_status_tmp);
      }

      void SearchCacheMemData::setUserStageProperties(mysqlpp::Row& row){
        const char* stage= "stage";
        int stage_tmp;
        RWRecMutex::WLock lock(_dataMutex);
        stage_tmp = (int) row[stage];
        set_stage(stage_tmp);
      }

      void SearchCacheMemData::setUserNameProperties(mysqlpp::Row& row){
        const char* name = "name";
        RWRecMutex::WLock lock(_dataMutex);
        std::string  name_tmp;
#ifndef NEWARCH
        name_tmp = row[name].get_string();
#else
        name_tmp = row[name].c_str();
#endif
        set_name(name_tmp);
      }

      void SearchCacheMemData::setUserBornProperties(mysqlpp::Row& row) {
        const char* gender = "gender";
        const char* home_city = "home_city";
        const char* home_province = "home_province";
        const char* astrology = "astrology";
        const char* birth_year = "birth_year";
        const char* birth_month = "birth_month";
        const char* birth_day = "birth_day";
        std::string gender_tmp;
        std::string home_city_tmp;
        std::string home_province_tmp;
        std::string astrology_tmp;
        int birth_year_tmp;
        int birth_month_tmp;
        int birth_day_tmp;
        RWRecMutex::WLock lock(_dataMutex);
#ifndef NEWARCH
        gender_tmp = row[gender].get_string();
        home_city_tmp = row[home_city].get_string();
        home_province_tmp = row[home_province].get_string();
        astrology_tmp = row[astrology].get_string();
#else
        gender_tmp = row[gender].c_str();
        home_city_tmp = row[home_city].c_str();
        home_province_tmp = row[home_province].c_str();
        astrology_tmp = row[astrology].c_str();
#endif
        try  {
          birth_year_tmp = (int)row[birth_year];
        }  catch (mysqlpp::Exception& e)  {
          birth_year_tmp = 0;
        }
        try  {
          birth_month_tmp = (int)row[birth_month];
        }  catch(mysqlpp::Exception& e)  {
          birth_month_tmp = 0;
        }
        try  {
          birth_day_tmp = (int)row[birth_day];
        }  catch(mysqlpp::Exception& e)  {
          birth_day_tmp = 0;
        }
        set_gender(gender_tmp);
        set_homecity(home_city_tmp);
        set_homeprovince(home_province_tmp);
        set_astrology(astrology_tmp);
        set_birthyear(birth_year_tmp);
        set_birthmonth(birth_month_tmp);
        set_birthday(birth_day_tmp);
      }

      void SearchCacheMemData::setCollegePropertiesAccordingVector(vector<Row>& rows) {
        const char* college_id = "college_id";
        const char* enroll_year = "enroll_year";
        const char* department = "department";
        const char* college_name = "college_name";

        RWRecMutex::WLock lock(_dataMutex);
        clear_collegeschools();
        for (vector<Row>::const_iterator iter = rows.begin();
            iter!= rows.end();
            ++iter) {
          int id, year;
          string name, department_tmp;
          id = (int) (*iter)[college_id];
          year = (int) (*iter)[enroll_year];
#ifndef NEWARCH
          name = (*iter)[college_name].get_string();
          department_tmp = (*iter)[department].get_string();
#else
          name = (*iter)[college_name].c_str();
          department_tmp = (*iter)[department].c_str();
#endif
          if(name == "")
            continue;

          CollegeInfo * tmp = add_collegeschools();

          tmp->set_id(id);

          tmp->set_year(year);

          tmp->set_name(name);

          tmp->set_department(department_tmp);

        }
      }

      void SearchCacheMemData::setUniversityPropertiesAccordingVector(vector<Row>& rows) {
        const char* university_id = "university_id";
        const char* enroll_year = "enroll_year";
        const char* university_name = "university_name";
        const char* department = "department";
        const char* dorm = "dorm";
        RWRecMutex::WLock lock(_dataMutex);
        clear_universityschools();
        for (vector<Row>::const_iterator iter = rows.begin();
            iter!= rows.end();
            ++iter) {
          int id, year;
          string name, department_tmp, dorm_tmp;
          id = (int) (*iter)[university_id];
          year = (int) (*iter)[enroll_year];
#ifndef NEWARCH
          name = (*iter)[university_name].get_string();
          department_tmp = (*iter)[department].get_string();
          dorm_tmp = (*iter)[dorm].get_string();
#else
          name = (*iter)[university_name].c_str();
          department_tmp = (*iter)[department].c_str();
          dorm_tmp = (*iter)[dorm].c_str();
#endif
          if(name == "")
            continue;

          UniversityInfo * tmp = add_universityschools();

          tmp->set_id(id);

          tmp->set_name(name);
          tmp->set_year(year);

          tmp->set_department(department_tmp);

          tmp->set_dorm(dorm_tmp);
        }
      }

      void SearchCacheMemData::setWorkplacePropertiesAccordingVector(vector<Row>& rows) {
        const char* workplace_id = "workplace_id";
        const char* workplace_name = "workplace_name";
        RWRecMutex::WLock lock(_dataMutex);
        clear_workplaces();
        for (vector<Row>::const_iterator iter = rows.begin();
            iter!= rows.end();
            ++iter) {
          int id;
          string name;
          id = (int) (*iter)[workplace_id];
#ifndef NEWARCH
          name = (*iter)[workplace_name].get_string();
#else
          name = (*iter)[workplace_name].c_str();
#endif
          if(name == "")
            continue;
          WorkplaceInfo * tmp = add_workplaces(); 
          tmp->set_id(id);
          tmp->set_name(name);
        }
      }


      void SearchCacheMemData::setRegionPropertiesAccordingVector(vector<Row>& rows) {
        const char* region_id = "region_id";
        const char* city_name = "city_name";
        const char* province_name = "province_name";

        RWRecMutex::WLock lock(_dataMutex);
        clear_regions();
        for (vector<Row>::const_iterator iter = rows.begin();
            iter!= rows.end();
            ++iter) {
          int id;
          string city, province;
          id = (int) (*iter)[region_id];
#ifndef NEWARCH
          city = (*iter)[city_name].get_string();
          province = (*iter)[province_name].get_string();
#else
          city = (*iter)[city_name].c_str();
          province = (*iter)[province_name].c_str();
#endif
          if(city == "" && province == "")
            continue;
          RegionInfo * tmp = add_regions();
          tmp->set_id(id);
          tmp->set_city(city);
          tmp->set_province(province);
        }
      }

      void SearchCacheMemData::setNetworkPropertiesAccordingVector(vector<Row>& rows) {
        const char* network_id = "network_id";
        const char* stage = "stage";
        const char* network_name = "network_name";
        RWRecMutex::WLock lock(_dataMutex);
        clear_networks();
        for (vector<Row>::const_iterator iter = rows.begin();
            iter!= rows.end();
            ++iter) {
          int id, stage_tmp;
          string name;
          id = (int) (*iter)[network_id];
          stage_tmp = (int) (*iter)[stage];
#ifndef NEWARCH
          name = (*iter)[network_name].get_string();
#else
          name = (*iter)[network_name].c_str();
#endif
          if(name == "")
            continue;
          NetWorkInfo * tmp = add_networks();
          tmp->set_id(id);
          tmp->set_stage(stage_tmp);
          tmp->set_name(name);
        }
      }

      void SearchCacheMemData::setHighSchoolPropertiesAccordingVector(vector<Row>& rows) {
        const char* high_id = "high_school_id";
        const char* high_year = "enroll_year";
        const char* high_name = "high_school_name";
        const char* h_class1 = "h_class1";
        const char* h_class2 = "h_class2";
        const char* h_class3 = "h_class3";

        RWRecMutex::WLock lock(_dataMutex);
        clear_highschools();

        for (vector<Row>::const_iterator iter = rows.begin();
            iter!= rows.end();
            ++iter) {
          int id, year;
          string name, class1, class2, class3;
          id = (int) (*iter)[high_id];
          year = (int) (*iter)[high_year];
#ifndef NEWARCH
          name = (*iter)[high_name].get_string();
          class1 = (*iter)[h_class1].get_string();
          class2 = (*iter)[h_class2].get_string();
          class3 = (*iter)[h_class3].get_string();
#else
          name = (*iter)[high_name].c_str();
          class1 = (*iter)[h_class1].c_str();
          class2 = (*iter)[h_class2].c_str();
          class3 = (*iter)[h_class3].c_str();
#endif
          if(name == "")
            continue;
          HighSchoolInfo * tmp = add_highschools();
          tmp->set_id(id);

          tmp->set_name(name);
          tmp->set_year(year);

          tmp->set_class1(class1);

          tmp->set_class2(class2);

          tmp->set_class3(class3);
        }
      }


      void SearchCacheMemData::setJuniorSchoolPropertiesAccordingVector(vector<Row>& rows) {
        const char* junior_high_id ="junior_high_school_id";
        const char* junior_high_year ="junior_high_school_year";
        const char* junior_high_name ="junior_high_school_name";

        RWRecMutex::WLock lock(_dataMutex);
        clear_juniorschools();
        for (vector<Row>::const_iterator iter = rows.begin();
            iter!= rows.end();
            ++iter) {
          int id, year;
          string name;
          id = (int) (*iter)[junior_high_id];
          year = (int) (*iter)[junior_high_year];
#ifndef NEWARCH 
          name = (*iter)[junior_high_name].get_string();
#else
          name = (*iter)[junior_high_name].c_str();
#endif
          if(name == "")
            continue;
          JuniorSchoolInfo * tmp = add_juniorschools();
          tmp->set_id(id);
          tmp->set_year(year);
          tmp->set_name(name);
        }
      }

      void SearchCacheMemData::setElementarySchoolPropertiesAccordingVector(vector<Row>& rows) {
        const char* elementary_id ="elementary_school_id";
        const char* elementary_year ="elementary_school_year";
        const char* elementary_name ="elementary_school_name";

        RWRecMutex::WLock lock(_dataMutex);
        clear_elementaryschools();
        for (vector<Row>::const_iterator iter = rows.begin();
            iter!= rows.end();
            ++iter) {
          int id, year;
          string name;
          id = (int) (*iter)[elementary_id];
          year = (int) (*iter)[elementary_year];
#ifndef NEWARCH
          name = (*iter)[elementary_name].get_string();
#else
          name = (*iter)[elementary_name].c_str();
#endif
          if(name == "")
            continue;
          ElementarySchoolInfo * tmp = add_elementaryschools();
          tmp->set_id(id);

          tmp->set_year(year);

          tmp->set_name(name);

        }
      }


      void SearchCacheMemPageData::setPageDataProperties(mysqlpp::Row& row)  {
        const char* name = "name";
        const char* head_url = "headUrl";
        const char* tiny_url = "tinyUrl";
        const char* fans_count = "fans_count";
        const char* sub_type = "sub_type";
        const char* p_type = "p_type";

        RWRecMutex::WLock lock(_dataMutex);
        int fans_count_tmp,  p_type_tmp;
        std::string name_tmp, sub_type_tmp, head_url_tmp, tiny_url_tmp;

        fans_count_tmp = (int) row[fans_count];
        sub_type_tmp = (string) row[sub_type];
        p_type_tmp = (int) row[p_type];
#ifndef NEWARCH
        name_tmp = row[name].get_string();
        head_url_tmp = row[head_url].get_string();
        tiny_url_tmp = row[tiny_url].get_string();
#else
        name_tmp = row[name].c_str();
        head_url_tmp = row[head_url].c_str();
        tiny_url_tmp = row[tiny_url].c_str();
#endif
        set_name(name_tmp);
        set_headurl(head_url_tmp);
        set_tinyurl(tiny_url_tmp);
        set_fanscount(fans_count_tmp);
        set_subtype(sub_type_tmp);
        set_ptype(p_type_tmp);
      }


      //***************************************************************************
      BatchResultHandlerI::BatchResultHandlerI(std::string id, void (SearchCacheData::*p)(mysqlpp::Row& row), map<long, SearchCacheDataPtr>& id2TT):_id(id), _pFunc(p), _id2TT(id2TT) {
      }

      bool BatchResultHandlerI::handle(mysqlpp::Row& row) const {
        int id = (int) row[_id.c_str()];
        if(NULL != _id2TT[id].get())  {
          ((_id2TT[id].get())->*_pFunc)(row);
        }
        return true;
      }

      //***************************************************************************
      BatchVectorResultHandlerI::BatchVectorResultHandlerI(std::string idField, map<long, vector<mysqlpp::Row> > &id2RowsMap):
        _idField(idField), _id2RowsMap(id2RowsMap) {
        }

      bool BatchVectorResultHandlerI::handle(mysqlpp::Row& row) const {
        int id = (int) row[_idField.c_str()];
        _id2RowsMap[id].push_back(row);
        return true;
      }
      //***************************************************************************
      BatchBasicStatusResultHandlerI::BatchBasicStatusResultHandlerI(Ice::Int& maxId, map<long, SearchCacheDataPtr>& id2TT) :
        _maxId(maxId), _id2TT(id2TT) {
        }

      bool BatchBasicStatusResultHandlerI::handle(mysqlpp::Row& row) const {
        int id = (int) row["id"];
        _maxId = id > _maxId ? id : _maxId;
        SearchCacheDataPtr obj =  new (std::nothrow) SearchCacheData;
        obj->setUserStatusProperties(row);
        obj->_id = id;
        _id2TT[id] = obj; 
        return true;
      }

      //***************************************************************************
      BatchZoieUpdateHandlerI::BatchZoieUpdateHandlerI(Ice::Int& id, Ice::Int& update_operation, bool& unique_id_operation) :
        _id(id), _update_operation(update_operation), _unique_id_operation(unique_id_operation) {
        }

      bool BatchZoieUpdateHandlerI::handle(mysqlpp::Row& row) const {
        int id = (int) row["user_id"];
        int operation = (int) row["operation"];
        if(_id == id && _update_operation == operation)  {
          _unique_id_operation = true;
        }  
        return true;
      }
      //***************************************************************************


      UpdateTask::UpdateTask() {
      }

      void UpdateTask::handle(queue<boost::shared_ptr<UpdateUnit> >& update_units_ptr_queue) {
        //程序入口
        try {
          //存放待更新的id
          MyUtil::LongSeq updateIds;
          //分别存放只有表名和表名字段名都有的更新单元
          UpdateUnitSeq updateTables, updateFieldMap;
          set<string>fieldTable, vectorTable;
          //以下表名信息都是vector类型的，即在数据库中，对应同一个用户id，可能有多条记录
          //例如 在多个学校上过大学, 故UserLogic只能给到形如 id:table 的更改信息 
          vectorTable.insert(TABLE_NETWORK_HISTORY); 
          vectorTable.insert(TABLE_ELEMENTARY_SCHOOL); 
          vectorTable.insert(TABLE_JUNIOR_SCHOOL); 
          vectorTable.insert(TABLE_HIGH_SCHOOL); 
          vectorTable.insert(TABLE_COLLEGE_INFO); 
          vectorTable.insert(TABLE_UNIVERSITY_INFO); 
          vectorTable.insert(TABLE_WORKPLACE_INFO); 
          vectorTable.insert(TABLE_REGION_INFO); 

          //以下表中更改的信息 UserLogic将给出形如 id:table:字段名:字段值 的形式
          fieldTable.insert(TABLE_USER_PASSPORT); 
          fieldTable.insert(TABLE_USER_NAME);
          fieldTable.insert(TABLE_USER_BORN);
          fieldTable.insert(TABLE_USER_URL);
          fieldTable.insert(TABLE_USER_STATE);
          fieldTable.insert(TABLE_USER_STAGE);
          fieldTable.insert(TABLE_USER_CONFIG);
          fieldTable.insert(TABLE_USER_RIGHT);
          fieldTable.insert(TABLE_USER_FOND);
          fieldTable.insert(TABLE_USER_VIEWCOUNT);
          //遍历update_units_ptr_queue，把更新单元分发到updateIds、updateTables和updateFieldMap中
          while (!update_units_ptr_queue.empty()) {
            boost::shared_ptr<UpdateUnit> ptr = update_units_ptr_queue.front();
            update_units_ptr_queue.pop();
            if (ptr->table.compare("") == 0) {
              updateIds.push_back(ptr->id);
            } else if (vectorTable.find(ptr->table)!=vectorTable.end()) {
              updateTables.push_back(*ptr);
            } else if (fieldTable.find(ptr->table)!=fieldTable.end()) {
              updateFieldMap.push_back(*ptr);
            }
            MCE_DEBUG("[UpdateTask::handle] userId:" << ptr->id);
            TripodSearchUpdateAdapter::instance().invoke(ptr->id);
          }
          MCE_INFO("[UpdateTask::handle] Ids size: " << updateIds.size() << " table size: " << updateTables.size() << " field map size " << updateFieldMap.size());

          UpdatePCAccordingIdAndTableName(updateTables); //通过Id和表名的更新
          UpdatePCAccordingFieldMap(updateFieldMap); //通过Id、表名和字段名的更新
          UpdatePCAccordingIds(updateIds); //通过Id的更新

        } catch (IceUtil::Exception& e) {
          MCE_ERROR("UpdateTask::handle Exception: " << e.what());
        } catch (std::exception& e) {
          MCE_ERROR("UpdateTask::handle Exception: " << e.what());
        } catch (...) {
          MCE_ERROR("UpdateTask::handle unknown Exception ");
        }
      }

      //提供ZoieUpdater用于往最近修改的表中插入数据
      bool UpdateTask::RecordUpdateData(int id, int update_operation)  {
        ZoieUpdater::instance().insert(id,update_operation);
        return true;
        /*
           bool ret = true;
           buffer_[id] = update_operation;

           if(buffer_.size() > 1000 || time(NULL) - last_update_ >= 10){
           Statement sql_select, sql_update;
           MCE_INFO("update last_registry_user size : "<< buffer_.size());
           sql_update<<"insert into "<<TABLE_LAST_REGISTRY<<"(user_id, operation) values ";
           for(map<int,int>::iterator it=buffer_.begin();it!=buffer_.end();++it){
           if(it!=buffer_.begin())
           sql_update<<",";
           sql_update<<"("<<it->first<<","<<it->second<<")";
           }
           try  {
           TimeCost tc = TimeCost::create("update last_registry_user");
           mysqlpp::SimpleResult res = QueryRunner(DB_ZOIE_UPDATE, CDbWServer).execute(sql_update);
           if(!res)  {
           MCE_WARN("write table last_registry_user error!");
           ret = false;
           }
           }  catch (std::exception& e) {
           MCE_WARN("write table last_registry_user exception : " << e.what());
           ret = false;
           } catch (...) {
           MCE_WARN("write table last_registry_user unknown exception.");
           ret = false;
           }
           buffer_.clear();
           last_update_ = time(NULL);
           }
           return ret;
           */
      }

      void UpdateTask::UpdatePCAccordingIds(MyUtil::LongSeq& uids) {
        MCE_DEBUG("[UpdateTask::UpdatePCAccordingIds] uids.size:" << uids.size()); 
        if (uids.empty()) {
          MCE_DEBUG("[UpdateTask::UpdatePCAccordingIds] uids is empty , return");
          return;
        }
        MyUtil::LongSeq& ids = uids;
        sort(ids.begin(), ids.end());   //  排序
        MyUtil::LongSeq::iterator end = unique(ids.begin(), ids.end());   //去重，只有通过排序后才能进行此操作
        ids.resize(end - ids.begin()); 
        /*for(MyUtil::LongSeq::iterator iter = uids.begin(); iter != uids.end(); iter++) {
          MCE_INFO("[UpdatePCAccordingIds] ID = "<<*iter);
          }*/
        try{
          //查询多个数据库
          UpdateSqlBatch(ids);
        } catch(Ice::Exception& e) {
          MCE_FATAL("UpdateSqlBatch ERROR!"<<e);
        } catch (...) {
          MCE_FATAL("UpdateAccordingIds() run ERROR!");
        }
      }

      // 根据参数生成Sql语句
      Statement UpdateTask::generateSql(const string& idlist, const string& tableName, const string& resultFields, const string& condition) {
        Statement sql;
        sql << "select " << resultFields << " from " << tableName << " where "<<condition<<" in "<<idlist;
        return sql;
      }

      // 通过多个id构造如（id1，id2）的串，用于拼接sql语句
      void UpdateTask::generateCondition(const MyUtil::LongSeq& uids, string& idlist)  {
        ostringstream oss;
        MyUtil::LongSeq::const_iterator it = uids.begin();

        if (uids.size() == 1 && *it == 0) {
          return;
        }
        oss << "(" ;
        if (*it != 0) {
          oss << *it;
        }
        else {
          oss <<*(++it);
        }
        ++it;
        for (; it != uids.end(); ++it) {
          if(*it != 0)  {
            oss << ", " << *it;
          }  else  {
            continue;
          }
        }
        oss << ")";
        idlist = oss.str();
      }

      void UpdateTask::UpdatePCAccordingIdAndTableName(UpdateUnitSeq& updateTables) {
        MCE_DEBUG("[UpdateTask::UpdatePCAccordingIdAndTableName] updateTables.size:" << updateTables.size());
        if (updateTables.empty()) {
          MCE_DEBUG("[UpdateTask::UpdatePCAccordingIdAndTableName] updateTables is empty return");
          return;
        }

        UpdateUnitSeq::const_iterator iter;
        MyUtil::LongSeq network_history_ids, elementary_school_info_ids, junior_high_school_info_ids, 
          high_school_info_ids, college_info_ids, university_info_ids, workplace_info_ids, region_info_ids;

        //遍历updateTables，把每个id分发到不同的类别中
        for(iter=updateTables.begin(); iter!=updateTables.end(); iter++) {
          if (0 == iter->id) {
            continue;
          }
          MCE_DEBUG("[UpdateTask::UpdatePCAccordingIdAndTableName] ID = "<<iter->id); 
          if (0 == iter->table.compare(TABLE_NETWORK_HISTORY)) {
            network_history_ids.push_back(iter->id);
          } else if (0 == iter->table.compare(TABLE_ELEMENTARY_SCHOOL)) {
            elementary_school_info_ids.push_back(iter->id);
          } else if (0 == iter->table.compare(TABLE_JUNIOR_SCHOOL)) {
            junior_high_school_info_ids.push_back(iter->id);
          } else if (0 == iter->table.compare(TABLE_HIGH_SCHOOL)) {
            high_school_info_ids.push_back(iter->id);
          } else if (0 == iter->table.compare(TABLE_COLLEGE_INFO)) {
            college_info_ids.push_back(iter->id);
          } else if (0 == iter->table.compare(TABLE_UNIVERSITY_INFO)) {
            university_info_ids.push_back(iter->id);
          } else if (0 == iter->table.compare(TABLE_WORKPLACE_INFO)) {
            workplace_info_ids.push_back(iter->id);
          } else if (0 == iter->table.compare(TABLE_REGION_INFO)) {
            region_info_ids.push_back(iter->id);
          }
        }
        //对每个类别分别进行处理，从DB中读取需要更新的数据
        if(network_history_ids.size() > 0)
        {
          Statement sql;
          string suffix = "";
          generateCondition(network_history_ids, suffix);
          sql = generateSql(suffix, TABLE_NETWORK_HISTORY, NETWORK_HISTORY_FIELD, " userid ");
          UpdateSpecialTable(network_history_ids, sql, &SearchCacheData::setNetworkPropertiesAccordingVector, &SearchCacheMemData::setNetworkPropertiesAccordingVector);
        }
        if(elementary_school_info_ids.size() > 0)
        {
          Statement sql;
          string suffix = "";
          generateCondition(elementary_school_info_ids, suffix);
          sql = generateSql(suffix, TABLE_ELEMENTARY_SCHOOL, ELEMENTARY_SCHOOL_INFO_FIELD, " userid ");
          UpdateSpecialTable(elementary_school_info_ids, sql, &SearchCacheData::setElementarySchoolPropertiesAccordingVector,  
              &SearchCacheMemData::setElementarySchoolPropertiesAccordingVector);
        }
        if(junior_high_school_info_ids.size() > 0)
        {
          Statement sql;
          string suffix = "";
          generateCondition(junior_high_school_info_ids, suffix);
          sql = generateSql(suffix, TABLE_JUNIOR_SCHOOL, JUNIOR_HIGH_SCHOOL_INFO_FIELD, " userid ");
          UpdateSpecialTable(junior_high_school_info_ids, sql, &SearchCacheData::setJuniorSchoolPropertiesAccordingVector,
              &SearchCacheMemData::setJuniorSchoolPropertiesAccordingVector);
        }
        if(high_school_info_ids.size() > 0)
        {
          Statement sql;
          string suffix = "";
          generateCondition(high_school_info_ids, suffix);
          sql = generateSql(suffix, TABLE_HIGH_SCHOOL, HIGH_SCHOOL_INFO_FIELD, " userid ");
          UpdateSpecialTable(high_school_info_ids, sql, &SearchCacheData::setHighSchoolPropertiesAccordingVector,
              &SearchCacheMemData::setHighSchoolPropertiesAccordingVector);
        }
        if(college_info_ids.size() > 0)
        {
          Statement sql;
          string suffix = "";
          generateCondition(college_info_ids, suffix);
          sql = generateSql(suffix, TABLE_COLLEGE_INFO, COLLEGE_INFO_FIELD, " userid "); 
          UpdateSpecialTable(college_info_ids, sql, &SearchCacheData::setCollegePropertiesAccordingVector,
              &SearchCacheMemData::setCollegePropertiesAccordingVector);
        }
        if(university_info_ids.size() > 0)
        {
          Statement sql;
          string suffix = "";
          generateCondition(university_info_ids, suffix);
          sql = generateSql(suffix, TABLE_UNIVERSITY_INFO, UNIVERSITY_INFO_FIELD, " userid ");
          UpdateSpecialTable(university_info_ids, sql, &SearchCacheData::setUniversityPropertiesAccordingVector,
              &SearchCacheMemData::setUniversityPropertiesAccordingVector);
        }
        if(workplace_info_ids.size() > 0)
        {
          Statement sql;
          string suffix = "";
          generateCondition(workplace_info_ids, suffix);
          sql = generateSql(suffix, TABLE_WORKPLACE_INFO, WORKPLACE_INFO_FIELD, " userid ");
          UpdateSpecialTable(workplace_info_ids, sql, &SearchCacheData::setWorkplacePropertiesAccordingVector,
              &SearchCacheMemData::setWorkplacePropertiesAccordingVector);
        }
        if(region_info_ids.size() > 0)
        {
          Statement sql;
          string suffix = "";
          generateCondition(region_info_ids, suffix);
          sql = generateSql(suffix, TABLE_REGION_INFO, REGION_INFO_FIELD, " userid ");
          UpdateSpecialTable(region_info_ids, sql, &SearchCacheData::setRegionPropertiesAccordingVector,
              &SearchCacheMemData::setRegionPropertiesAccordingVector);
        }
      }

      //TODO: LastLoginTime更新太频繁，暂时不做更新
      void UpdateTask::UpdatePCAccordingFieldMap(UpdateUnitSeq& updateFieldMap) {
        MCE_DEBUG("[UpdateTask::UpdatePCAccordingFieldMap] updateFieldMap.size:" << updateFieldMap.size());
        if (updateFieldMap.empty()) {
          MCE_DEBUG("[UpdateTask::UpdatePCAccordingFieldMap] updateFieldMap is empty, return");
          return;
        }
        map<string, int> field2count;
        try{
          UpdateUnitSeq::const_iterator updateUnitIter;
          map<string, int*> mapString2IntField;
          map<string, string*> mapString2StringField; 

          mapString2IntField["user_passport:STATUS"] = NULL;
          mapString2StringField["user_names:NAME"] = NULL;
          mapString2StringField["user_born:GENDER"] = NULL;
          mapString2StringField["user_names:NICKNAME"] = NULL;
          //mapString2StringField["user_time:LASTLOGINTIME"] = NULL;
          mapString2StringField["user_url:TINYURL"] = NULL;
          mapString2StringField["user_url:HEADURL"] = NULL;
          mapString2StringField["user_born:ASTROLOGY"] = NULL;
          mapString2StringField["user_born:HOME_CITY"] = NULL;
          mapString2StringField["user_born:HOME_PROVINCE"] = NULL;
          mapString2StringField["user_fond:INTEREST"] = NULL;
          mapString2StringField["user_fond:MUSIC"] = NULL;
          mapString2StringField["user_fond:MOVIE"] = NULL;
          mapString2StringField["user_fond:GAME"] = NULL;
          mapString2StringField["user_fond:COMIC"] = NULL;
          mapString2StringField["user_fond:SPORT"] = NULL;
          mapString2StringField["user_fond:BOOK"] = NULL;
          mapString2StringField["user_fond:SOCIETY"] = NULL;
          mapString2StringField["user_config:PROFILE_PRIVACY"] = NULL;

          mapString2IntField["user_state:STAR"] = NULL;
          mapString2IntField["user_stage:STAGE"] = NULL;
          mapString2IntField["user_state:STATE"] = NULL;
          mapString2IntField["user_born:BIRTH_YEAR"] = NULL;
          mapString2IntField["user_born:BIRTH_MONTH"] = NULL;
          mapString2IntField["user_born:BIRTH_DAY"] = NULL;
          mapString2IntField["user_config:BROWSE_CONFIG"] = NULL;
          mapString2IntField["user_config:STATUS_CONFIG"] = NULL;
          mapString2IntField["user_config:BASIC_CONFIG"] = NULL;
          mapString2IntField["user_right:AUTH"] = NULL;
          mapString2IntField["view_count:VIEWCOUNT"] = NULL;

          set<string> fieldSet;

          //用于统计计数, 分析哪个字段更新的最频繁
          field2count[FIELD_PASSPORT_STATUS] = 0;
          field2count[FIELD_PASSPORT_SAFESTATUS] = 0;
          field2count[FIELD_USER_NAME] = 0;
          field2count[FIELD_URL_TINYURL] = 0;
          field2count[FIELD_URL_HEADURL] = 0;
          field2count[FIELD_BORN_GENDER] = 0;
          field2count[FIELD_USER_STAGE] = 0;
          field2count[FIELD_BROWSE_CONFIG] = 0;
          field2count[FIELD_STATUS_CONFIG] = 0;
          field2count[FIELD_BASIC_CONFIG] = 0;
          field2count[FIELD_USER_STATE] = 0;

          //用户自动化修改
          fieldSet.insert(FIELD_PASSPORT_STATUS);
          fieldSet.insert(FIELD_PASSPORT_SAFESTATUS);
          fieldSet.insert(FIELD_USER_NAME);
          fieldSet.insert(FIELD_URL_TINYURL);
          fieldSet.insert(FIELD_URL_HEADURL);
          fieldSet.insert(FIELD_BORN_GENDER);
          fieldSet.insert(FIELD_USER_STAGE);
          fieldSet.insert(FIELD_USER_STATE);
          fieldSet.insert(FIELD_BORN_ASTROLOGY);
          fieldSet.insert(FIELD_BORN_HOMECITY);
          fieldSet.insert(FIELD_BORN_HOMEPROVINCE);
          fieldSet.insert(FIELD_BORN_BIRTH_DAY);
          fieldSet.insert(FIELD_BORN_BIRTH_YEAR);
          fieldSet.insert(FIELD_BORN_BIRTH_MONTH);

          fieldSet.insert(FIELD_BROWSE_CONFIG);
          fieldSet.insert(FIELD_STATUS_CONFIG);
          fieldSet.insert(FIELD_BASIC_CONFIG);
          fieldSet.insert(FIELD_PROFILE_PRIVACY);
          fieldSet.insert(FIELD_USER_NICKNAME);
          //fieldSet.insert(FIELD_USER_LASTLOGINTIME);
          fieldSet.insert(FIELD_FOND_INTEREST);
          fieldSet.insert(FIELD_FOND_MUSIC);
          fieldSet.insert(FIELD_FOND_MOVIE);
          fieldSet.insert(FIELD_FOND_GAME);
          fieldSet.insert(FIELD_FOND_COMIC);
          fieldSet.insert(FIELD_FOND_SPORT);
          fieldSet.insert(FIELD_FOND_BOOK);
          fieldSet.insert(FIELD_FOND_SOCIETY);
          fieldSet.insert(FIELD_RIGHT_AUTH);
          fieldSet.insert(FIELD_VIEWCOUNT);
          for(updateUnitIter=updateFieldMap.begin(); updateUnitIter!=updateFieldMap.end(); updateUnitIter++) {
            //先遍历看是否有关心的字段
            {
              bool has_valid_filed = false;
              string tableName = updateUnitIter->table;
              transform(tableName.begin(), tableName.end(), tableName.begin(), ::tolower);
              for(map<string, string>::const_iterator fieldIter=updateUnitIter->fieldPairMap.begin(); fieldIter!=updateUnitIter->fieldPairMap.end(); fieldIter++) {
                //关键字段若为空，则不进行更新
                MCE_DEBUG("[UpdateTask::UpdatePCAccordingFieldMap] id = "<<updateUnitIter->id<<" table = "<<tableName<<" FieldMapKey = "<<fieldIter->first<<" FieldMapValue = "<<fieldIter->second << " TABLE_USER_NAME = " << TABLE_USER_NAME << " TABLE_USER_PASSPORT = " << TABLE_USER_PASSPORT);
                //如果需要更新的有user_names表，但是fieldPairMap中NAME对应的值为空，则不更新这个updateUnit
                if (0 == tableName.compare(TABLE_USER_NAME) && 0 == (fieldIter->first).compare("NAME") && 0 == (fieldIter->second).compare("")) {
                  MCE_DEBUG("[UpdateTask::UpdatePCAccordingFieldMap] tableName " << tableName << " TABLE_USER_NAME " << TABLE_USER_NAME);
                  break;
                }
                //如果需要更新的有user_passport表，但是STATUS对应的值大于等于3，则从tt和searchcache中删除该用户对应的数据
                if(0 == tableName.compare(TABLE_USER_PASSPORT) && 0 == (fieldIter->first).compare("STATUS") && atoi((fieldIter->second).c_str()) >= STATUS_LIMIT)  {
                  MCE_INFO("user_id = "<<updateUnitIter->id<<" is banned!");
                  SearchCacheMemDataPtr obj =  new SearchCacheMemData;
                  DistSearchCacheAdapter::instance().setData(updateUnitIter->id, obj);     
                  boost::shared_ptr<IDbCache> db_handler = SearchLogicManagerI::instance().getDbHandler(updateUnitIter->id);
                  if(db_handler)  {
                    if(!db_handler->Remove(updateUnitIter->id))  {
                      MCE_INFO("Failed of removing banned id = "<<updateUnitIter->id<<" in TT!");
                    }
                  }  else  {
                    MCE_INFO("user_id = "<<updateUnitIter->id<<" is banned!"<<"\t"<<" TT getDbHandler error!");
                  }
                  RecordUpdateData(updateUnitIter->id, ZOIE_DELETE);
                  continue;
                }
                //如果需要更新的有user_passport表，但是STATUS对应的值小于3，
                //则通过UpdatePCAccordingIds把该用户的所有信息都加到TT和searchcache中
                if(0 == tableName.compare(TABLE_USER_PASSPORT) && 0 == (fieldIter->first).compare("STATUS") && atoi((fieldIter->second).c_str()) < STATUS_LIMIT)  {
                  MCE_INFO("user_id = "<<updateUnitIter->id<<" is activated!");
                  MyUtil::LongSeq updateIds;
                  updateIds.push_back(updateUnitIter->id); 
                  UpdatePCAccordingIds(updateIds);
                  continue;         
                }
                string table_name_and_field = tableName + ":" + fieldIter->first;
                MCE_DEBUG("[UpdateTask::UpdatePCAccordingFieldMap] table_name_and_field " << table_name_and_field);
                set<string>::iterator stringFieldIter = fieldSet.find(table_name_and_field);
                if (stringFieldIter != fieldSet.end()) {
                  MCE_DEBUG("[UpdateTask::UpdatePCAccordingFieldMap] find " << table_name_and_field << " in fieldSet ");
                  //计数加一
                  ++field2count[table_name_and_field];
                  has_valid_filed = true;
                  continue;
                } 
              }
              //该updateUnit中没有合法的需要更新的字段
              if (!has_valid_filed) {
                continue;
              }
            }
            //从searchcache中获取该用户对应的信息
            SearchCacheMemDataPtr obj = DistSearchCacheAdapter::instance().getData(updateUnitIter->id);
            SearchCacheDataPtr obj_tt = new (std::nothrow) SearchCacheData;
            boost::shared_ptr<IDbCache> db_handler = SearchLogicManagerI::instance().getDbHandler(updateUnitIter->id);
            if(db_handler)  {
              MCE_DEBUG("[UpdateTask::UpdatePCAccordingFieldMap] db_handler is true");
              if(!db_handler->Query(updateUnitIter->id,obj_tt.get()))  {
                MCE_INFO("[UpdateFieldMap] Failed of querying id = "<<updateUnitIter->id<<" in TT!");
                continue;//查询失败，禁止下面更改个别字段
              }
              MCE_DEBUG("[UpdateTask::UpdatePCAccordingFieldMap] db_handler->Query success");
            }
            //TODO:判断&obj_tt->_status>=3，删除tt和memCache
            if (obj_tt->_status >= STATUS_LIMIT) {
              MCE_DEBUG("[UpdateTask::UpdatePCAccordingFieldMap] obj_tt->_status:" << obj_tt->_status);
              SearchCacheMemDataPtr tmp = new SearchCacheMemData;
              if (db_handler) {
                if(!db_handler->Remove(updateUnitIter->id))  {
                  MCE_INFO("[UpdateTask::UpdatePCAccordingFieldMap] Failed of removing banned id = "<<updateUnitIter->id<<" in TT!");
                }
                MCE_DEBUG("[UpdateTask::UpdatePCAccordingFieldMap] remove banned id = "<<updateUnitIter->id<<" in TT!");
              }
              DistSearchCacheAdapter::instance().setData(updateUnitIter->id, tmp);
              continue;
            }
            //把tt中的各项值的指针赋给mapString2IntField和mapString2StringField
            mapString2IntField[FIELD_PASSPORT_STATUS] = &obj_tt->_status;
            mapString2StringField[FIELD_USER_NAME] = &obj_tt->_name;
            mapString2StringField[FIELD_BORN_GENDER] = &obj_tt->_gender;
            mapString2StringField[FIELD_USER_NICKNAME] = &obj_tt->_signature;
            mapString2StringField[FIELD_USER_LASTLOGINTIME] = &obj_tt->_lastlogintime;
            mapString2StringField[FIELD_URL_TINYURL] = &obj_tt->_tinyurl;
            mapString2StringField[FIELD_URL_HEADURL] = &obj_tt->_headurl;
            mapString2StringField[FIELD_BORN_ASTROLOGY] = &obj_tt->_astrology;
            mapString2StringField[FIELD_BORN_HOMECITY] = &obj_tt->_home_city;
            mapString2StringField[FIELD_BORN_HOMEPROVINCE] = &obj_tt->_home_province;
            mapString2StringField[FIELD_FOND_INTEREST] = &obj_tt->_interest;
            mapString2StringField[FIELD_FOND_MUSIC] = &obj_tt->_music;
            mapString2StringField[FIELD_FOND_MOVIE] = &obj_tt->_movie;
            mapString2StringField[FIELD_FOND_GAME] = &obj_tt->_game;
            mapString2StringField[FIELD_FOND_COMIC] = &obj_tt->_comic;
            mapString2StringField[FIELD_FOND_SPORT] = &obj_tt->_sport;
            mapString2StringField[FIELD_FOND_BOOK] = &obj_tt->_book;
            mapString2StringField[FIELD_FOND_SOCIETY] = &obj_tt->_society;
            mapString2StringField[FIELD_PROFILE_PRIVACY] = &obj_tt->_profile_privacy;

            mapString2IntField[FIELD_USER_STAR] = &obj_tt->_star;
            mapString2IntField[FIELD_USER_STAGE] = &obj_tt->_stage;
            mapString2IntField[FIELD_USER_STATE] = &obj_tt->_state;
            mapString2IntField[FIELD_BORN_BIRTH_YEAR] = &obj_tt->_birthday_year;
            mapString2IntField[FIELD_BORN_BIRTH_MONTH] = &obj_tt->_birthday_month;
            mapString2IntField[FIELD_BORN_BIRTH_DAY] = &obj_tt->_birthday_day;
            mapString2IntField[FIELD_BROWSE_CONFIG] = &obj_tt->_browse_config;
            mapString2IntField[FIELD_STATUS_CONFIG] = &obj_tt->_status_config;
            mapString2IntField[FIELD_BASIC_CONFIG] = &obj_tt->_basic_config;
            mapString2IntField[FIELD_RIGHT_AUTH] = &obj_tt->_auth;
            mapString2IntField[FIELD_VIEWCOUNT] = &obj_tt->_viewcount;

            {
              string tableName = updateUnitIter->table;
              //依次遍历各updateUnit，对需要更改的表的字段及值进行更新
              for(map<string, string>::const_iterator fieldIter=updateUnitIter->fieldPairMap.begin(); fieldIter!=updateUnitIter->fieldPairMap.end(); fieldIter++) {
                map<string, string*>::iterator stringFieldIter = mapString2StringField.find(tableName+":"+fieldIter->first);
                map<string, int*>::iterator intFieldIter = mapString2IntField.find(tableName+":"+fieldIter->first);
                //更新值为String类型的数据
                if (stringFieldIter != mapString2StringField.end()) {
                  //如果更新的值有PROFILE_PRIVACY，则对该值进行分割，值获取搜索需要用的部分
                  if(0 == fieldIter->first.compare("PROFILE_PRIVACY") && 0 == tableName.compare("user_config"))  {
                    boost::shared_ptr<SearchCacheData> sc_data = boost::shared_ptr<SearchCacheData>(new SearchCacheData());
                    obj_tt->_profile_privacy = sc_data->splitProfilePrivacy(fieldIter->second);
                    obj->set_profileprivacy(sc_data->splitProfilePrivacy(fieldIter->second));//searchcache中只存profile_privacy部分字段
                  }  else {
                    *(stringFieldIter->second) = fieldIter->second;//更新obj_tt中的数据
                    MCE_DEBUG("[UpdateTask::UpdatePCAccordingFieldMap] Id = "<<updateUnitIter->id<<" stringFieldIterValue is "<<fieldIter->second);
                    //对其他值进行更新
                    if(stringFieldIter->first == FIELD_USER_NAME)  {
                      obj->set_name(fieldIter->second);
                    }  else if(stringFieldIter->first == FIELD_BORN_GENDER)  {
                      obj->set_gender(fieldIter->second);
                    } else if(stringFieldIter->first == FIELD_URL_TINYURL)  {
                      obj->set_tinyurl(fieldIter->second);
                    } else if(stringFieldIter->first == FIELD_URL_HEADURL)  {
                      obj->set_headurl(fieldIter->second);
                    } else if(stringFieldIter->first == FIELD_BORN_ASTROLOGY)  {
                      obj->set_astrology(fieldIter->second);
                    } else if(stringFieldIter->first == FIELD_BORN_HOMECITY)  {
                      obj->set_homecity(fieldIter->second);
                    } else if(stringFieldIter->first == FIELD_BORN_HOMEPROVINCE)  {
                      obj->set_homeprovince(fieldIter->second);
                    }
                  }
                } else if (intFieldIter != mapString2IntField.end()) {//更新值为Int型的数据
                  *(intFieldIter->second) = String2Int(fieldIter->second);//更新obj_tt中的数据
                  MCE_DEBUG("[UpdateTask::UpdatePCAccordingFieldMap] Id = "<<updateUnitIter->id<<" intFieldIterValue is "<<fieldIter->second);
                  if(intFieldIter->first == FIELD_PASSPORT_STATUS)  {
                    obj->set_status(String2Int(fieldIter->second));
                  } else if(intFieldIter->first == FIELD_PASSPORT_SAFESTATUS)  {
                    obj->set_safestatus(String2Int(fieldIter->second));
                  } else if(intFieldIter->first == FIELD_USER_STATE)  {
                    obj->set_state(String2Int(fieldIter->second));
                  } else if(intFieldIter->first == FIELD_USER_STAGE)  {
                    obj->set_stage(String2Int(fieldIter->second));
                  } else if(intFieldIter->first == FIELD_BROWSE_CONFIG)  {
                    obj->set_browseconfig(String2Int(fieldIter->second));
                  } else if(intFieldIter->first == FIELD_STATUS_CONFIG)  {
                    obj->set_statusconfig(String2Int(fieldIter->second));
                  } else if(intFieldIter->first == FIELD_BASIC_CONFIG)  {
                    obj->set_basicconfig(String2Int(fieldIter->second));
                  } else if(intFieldIter->first == FIELD_BORN_BIRTH_YEAR)  {
                    obj->set_birthyear(String2Int(fieldIter->second));
                  } else if(intFieldIter->first == FIELD_BORN_BIRTH_MONTH)  {
                    obj->set_birthmonth(String2Int(fieldIter->second));
                  } else if(intFieldIter->first == FIELD_BORN_BIRTH_DAY)  {
                    obj->set_birthday(String2Int(fieldIter->second));
                  }
                }
              }
            }
            //把修改的值写回searchcache
            DistSearchCacheAdapter::instance().setData(updateUnitIter->id, obj);
            if(NULL != obj_tt.get())  {
              //把修改后的值写回ttserver中
              boost::shared_ptr<IDbCache> db_handler = SearchLogicManagerI::instance().getDbHandler(updateUnitIter->id);
              if(db_handler)  {
                if(!db_handler->Insert(updateUnitIter->id,*obj_tt.get()))  {
                  MCE_INFO("[UpdateFieldMap] Failed of inserting id = "<<updateUnitIter->id<<" in TT!");
                }  else  {//Insert TT success, then write date to last_registry_user for zoie
                  if(0 != updateUnitIter->table.compare(TABLE_USER_STATE))  {
                    MCE_DEBUG("Insert Into last_registry_user.................................");
                    RecordUpdateData(updateUnitIter->id, ZOIE_ADD);
                  }
                }
              }
            }
          }
        } catch (const Ice::Exception& e) {
          MCE_WARN(" UpdateAccordingFieldMap Ice::Exception:"<<e.what() );
        } catch (std::exception& e) {
          MCE_WARN("UpdateAccordingFieldMap std::exception:" << e.what());
        } catch(...) {
          MCE_WARN(" UpdateAccordingFieldMap unknown exception");
        }
        MCE_DEBUG("[TEMPLE] step out of UpdatePCAndMCAccordingFieldMap");
      }

      //从tt中读取用户的信息，保存在SearchCacheDataPtr obj中，成功返回true，否则返回false
      bool UpdateTask::ReadDataFromTT(long userId, xce::searchcache::SearchCacheDataPtr obj,boost::shared_ptr<IDbCache> db_handler) {
        if (userId < 0 || !db_handler || !obj) {
          return false;
        }

        if (!db_handler->Query(userId, obj.get())) { 
          MCE_INFO("db_handler->Query("<<userId<<") Failed");
          return false;
        } else {
          return true;
        }
      }

      //UpdatePCAccordingIdAndTableName调用，用户对updateUnit中通过id和表名更新的各个表进行操作
      void UpdateTask::UpdateSpecialTable(MyUtil::LongSeq& uids, Statement& sql, void (SearchCacheData::*pFuncDb)(vector<mysqlpp::Row>& rows_db), 
          void (SearchCacheMemData::*pFuncCache)(vector<mysqlpp::Row>& rows_cache)) {
        if (uids.empty()) {
          return;
        }
        MyUtil::LongSeq& ids = uids;
        sort(ids.begin(), ids.end());

        MyUtil::LongSeq::const_iterator end = unique(ids.begin(), ids.end());
        ids.resize(end - ids.begin());
        map<long, SearchCacheMemDataPtr> mem_data = DistSearchCacheAdapter::instance().getData(ids);
        map<long, SearchCacheDataPtr> tt_data;
        MyUtil::LongSeq  updateIds; //保存通过UpdatePCAccordingIds更新的用户id
        set<long> unUsedIds;//保存不需要处理的用户id
        //遍历传入的用户id,从tt中读取数据
        for(MyUtil::LongSeq::iterator iter = uids.begin(); iter != uids.end(); iter++)  {
          xce::searchcache::SearchCacheDataPtr tmp = new (std::nothrow)SearchCacheData;
          if(!ReadDataFromTT(*iter, tmp, SearchLogicManagerI::instance().getDbHandler(*iter)))  {
            //tt中不存在该用户的信息，把该用户放入updateIds中，通过UpdatePCAccordingIds加载用户信息
            updateIds.push_back(*iter); 
            //同时把该用户放入unUsedIds中，不需要通过表名来更新部分信息
            unUsedIds.insert(*iter);
          }  else {
            //取到，则以用户id为key，取到的数据为value，放入tt_data中
            tt_data[*iter] = tmp;
          }   
        }

        map<long, vector<mysqlpp::Row> > id2RowsMap;
        //遍历从searchcache中取出的数据
        for(map<long, SearchCacheMemDataPtr>::iterator iter= mem_data.begin(); iter != mem_data.end(); iter++) {
          vector<mysqlpp::Row>  row_vector;
          row_vector.clear();
          /* if("" == iter->second->name() && "" == iter->second->headurl()) { 
             iter->second = new SearchCacheMemData;
             DistSearchCacheAdapter::instance().setData(iter->first, iter->second);
             continue;     
             }*/
          if((NULL == (iter->second).get() || "" == iter->second->name()) && unUsedIds.find(iter->first) == unUsedIds.end())  {
            //若不存在该用户的信息，或该用户的名字为空，则把该用户放入updateIds中，通过UpdatePCAccordingIds加载用户信息
            MCE_DEBUG("[UpdateTask::UpdateSpecialTable] user_id = "<<iter->first<<" data in searchcache is empty!");
            updateIds.push_back(iter->first);
            continue;
          }  else if(unUsedIds.find(iter->second) != unUsedIds.end())  {
            //如果该用户的id为不需要处理的id，则跳过
            continue;
          }
          id2RowsMap[iter->first] = row_vector;  //以用户id为key，mysqlpp::Row为value，放入id2RowsMap中
        }
        if(!updateIds.empty())  {
          //加载updateIds中的用户资料
          UpdatePCAccordingIds(updateIds);
        }
        try {
          BatchVectorResultHandlerI handler("userid", id2RowsMap);
          QueryRunner(DB_SEARCH2_USERBASIC, CDbRServer).query(sql, handler);
        } catch (mysqlpp::Exception& e) {
          MCE_FATAL("UpdateSpecialTable mysqlpp Exception: "<< e.what());
        } catch (Ice::Exception& e) {
          MCE_FATAL("UpdateSpecialTable Ice::Exception : " << e.what());
        } catch (std::exception& e) {
          MCE_FATAL("UpdateSpecialTable std::exception : " << e.what());
        } catch (...) {
          MCE_WARN("ERROR! in Func  UpdateTask::UpdateSpecialTable, unkonwn exception");
        }
        for (map<long, vector<mysqlpp::Row> >::iterator iter = id2RowsMap.begin();
            iter != id2RowsMap.end(); 
            ++iter) {
          if(tt_data.find(iter->first) == tt_data.end())  continue;
          
          boost::shared_ptr<IDbCache> db_handler = SearchLogicManagerI::instance().getDbHandler(iter->first);
          //TODO: 判断TT中的status是否>=3，是则删除TT同时删除memcache
          SearchCacheDataPtr data_tt = tt_data[iter->first];
          if (data_tt->_status >= STATUS_LIMIT) {
            MCE_DEBUG("[UpdateTask::UpdateSpecialTable] data_tt->_status:" << data_tt->_status);
            SearchCacheMemDataPtr tmp = new SearchCacheMemData;
            if (db_handler) {
              if(!db_handler->Remove(iter->first))  {
                MCE_INFO("[UpdateTask::UpdateSpecialTable] Failed of removing banned id = "<<iter->first<<" in TT!");
              }
              MCE_DEBUG("[UpdateTask::UpdateSpecialTable] remove banned id = "<<iter->first<<" in TT!");
            }
            //删除memcache（置空）
            mem_data[iter->first] = tmp;
            continue;
          }
          //调用TT的数据类型的相关set方法，修改值data_tt的值
          ((data_tt.get())->*pFuncDb)(iter->second);
          RecordUpdateData(iter->first, ZOIE_ADD);
          MCE_DEBUG("[UpdateTask::UpdateSpecialTable] Insert Into last_registry_user------------------------"); 
          if(mem_data.find(iter->first) == mem_data.end() || NULL == mem_data.find(iter->first)->second.get()) {
            continue;
          }
          MCE_DEBUG("[UpdateTask::UpdateSpecialTable] mem_data.find(" << iter->first << ") is not NULL");
          SearchCacheMemDataPtr data_searchcache = mem_data.find(iter->first)->second;
          //调用DistSearchCache的相关set方法，修改值data_searchcache的值
          ((data_searchcache.get())->*pFuncCache)(iter->second); 
          if(db_handler)  {
            MCE_DEBUG("[UpdateTask::UpdateSpecialTable] db_handler is true");
            //把修改后的值写入tt
            if(!db_handler->Insert(iter->first,*data_tt.get()))  {
              MCE_INFO("[Updating special tables(network,elementary...)] Failed of updating id = "<<iter->first<<" in TT!");
            } 
            MCE_DEBUG("[UpdateTask::UpdateSpecialTable] db_handler->Insert success");
          } 
        }
        //把修改后的值写入searchcache
        DistSearchCacheAdapter::instance().setData(mem_data); 
      }

      int UpdateTask::String2Int(const string& strInt)  {
        stringstream ss(strInt);
        int intValue;
        ss>>intValue; 
        return intValue;
      }

      //通过tt中的数据，生成searchcache中需要的数据
      void UpdateTask::transformData(xce::searchcache::SearchCacheDataPtr original, xce::searchcache::dboperation::SearchCacheMemDataPtr destination)  {
        if(NULL == original.get())  {
          destination = new SearchCacheMemData;
          return;
        } 
        destination->set_name(original->_name);
        destination->set_gender(original->_gender);
        destination->set_status(original->_status);
        destination->set_stage(original->_stage);
        destination->set_state(original->_state);
        destination->set_tinyurl(original->_tinyurl);
        destination->set_headurl(original->_headurl);
        destination->set_browseconfig(original->_browse_config);
        destination->set_statusconfig(original->_status_config);
        destination->set_basicconfig(original->_basic_config);
        destination->set_profileprivacy(destination->splitProfilePrivacy(original->_profile_privacy));
        destination->set_homecity(original->_home_city); 
        destination->set_homeprovince(original->_home_province);
        destination->set_astrology(original->_astrology);
        destination->set_birthyear(original->_birthday_year);
        destination->set_birthmonth(original->_birthday_month);
        destination->set_birthday(original->_birthday_day);


        for(int i = 0; i < original->_networks.size(); i++)  {
          NetWorkInfo * network = destination->add_networks();
          network->set_id(original->_networks.at(i).id); 
          network->set_name(original->_networks.at(i).name); 
          network->set_stage(original->_networks.at(i).stage); 
        }
        for(int i = 0; i < original->_elementarySchools.size(); i++)  {
          ElementarySchoolInfo * elementaryschool = destination->add_elementaryschools();
          elementaryschool->set_id(original->_elementarySchools.at(i).id);
          elementaryschool->set_year(original->_elementarySchools.at(i).year);
          elementaryschool->set_name(original->_elementarySchools.at(i).name);
        }

        for(int i = 0; i < original->_juniorSchools.size(); i++)  {
          JuniorSchoolInfo * juniorschool = destination->add_juniorschools();
          juniorschool->set_id(original->_juniorSchools.at(i).id);
          juniorschool->set_year(original->_juniorSchools.at(i).year);
          juniorschool->set_name(original->_juniorSchools.at(i).name);
        }

        for(int i = 0; i < original->_highSchools.size(); i++)  {
          HighSchoolInfo * highschool = destination->add_highschools();
          highschool->set_id(original->_highSchools.at(i).id);
          highschool->set_year(original->_highSchools.at(i).year);
          highschool->set_name(original->_highSchools.at(i).name);
          highschool->set_class1(original->_highSchools.at(i).class1);
          highschool->set_class2(original->_highSchools.at(i).class2);
          highschool->set_class3(original->_highSchools.at(i).class3);
        }

        for(int i = 0; i < original->_colleges.size(); i++)  {
          CollegeInfo * collegeschool = destination->add_collegeschools();
          collegeschool->set_id(original->_colleges.at(i).id);
          collegeschool->set_year(original->_colleges.at(i).year);
          collegeschool->set_name(original->_colleges.at(i).name);
          collegeschool->set_department(original->_colleges.at(i).department);
        }

        for(int i = 0; i < original->_universities.size(); i++)  {
          UniversityInfo * universityschool = destination->add_universityschools();
          universityschool->set_id(original->_universities.at(i).id);
          universityschool->set_year(original->_universities.at(i).year);
          universityschool->set_name(original->_universities.at(i).name);
          universityschool->set_department(original->_universities.at(i).department);
          universityschool->set_dorm(original->_universities.at(i).dorm);
        }

        for(int i = 0; i < original->_workplaces.size(); i++)  {
          WorkplaceInfo * workplace = destination->add_workplaces();
          workplace->set_id(original->_workplaces.at(i).id);
          workplace->set_name(original->_workplaces.at(i).name);
        }

        for(int i = 0; i < original->_regions.size(); i++)  {
          RegionInfo * region = destination->add_regions();
          region->set_id(original->_regions.at(i).id);
          region->set_city(original->_regions.at(i).city);
          region->set_province(original->_regions.at(i).province);
        }
        return;

      }


      //批量从多个数据库里读出数据
      bool UpdateTask::UpdateSqlBatch(const vector<long>& uids) {
        MCE_DEBUG("[UpdateTask::UpdateSqlBatch] uids.size:" << uids.size());
        ostringstream oss;
        string strSuffix;
        MyUtil::LongSeq::const_iterator it = uids.begin();

        if (uids.size() == 1 && *it == 0) {
          return true;
        }

        oss << "(" ;
        if (*it != 0) {
          oss << *it;
        }
        else {
          oss <<*(++it);
        }
        ++it;
        for (; it != uids.end(); ++it) {
          oss << ", " << *it;
        }
        oss << ")";


        strSuffix = oss.str();
        if(uids.size() <= 0)  {
          return false;
        }
        generateCondition(uids, strSuffix);

        map<long, SearchCacheMemDataPtr> id2cache;
        map<long, SearchCacheDataPtr> id2tt;
        {
          Statement status_sql;
          status_sql = generateSql(strSuffix, TABLE_USER_PASSPORT, USER_STATUS_FIELD, " id ");
          int maxId;
          BatchBasicStatusResultHandlerI status_handler(maxId, id2tt);
          QueryRunner(DB_SEARCH2_USERBASIC_STATUS, CDbRServer).query(status_sql, status_handler);
        }
        {

          Statement sql;
          sql = generateSql(strSuffix, TABLE_USER_BORN, USER_BORN_FIELD_TT, " id "); 
          BatchResultHandlerI handler("id", &SearchCacheData::setUserBornProperties, id2tt);
          QueryRunner(DB_SEARCH2_USERBORN, CDbRServer).query(sql, handler);
        }
        {
          Statement sql;
          sql = generateSql(strSuffix, TABLE_USER_URL, USER_URL_FIELD, " id ");
          BatchResultHandlerI handler("id", &SearchCacheData::setUserUrlProperties, id2tt);
          QueryRunner(DB_SEARCH2_USERURL, CDbRServer).query(sql, handler);
        }
        {
          Statement sql;
          sql = generateSql(strSuffix, TABLE_USER_STATE, USER_STATE_FIELD_TT, " id ");
          BatchResultHandlerI handler("id", &SearchCacheData::setUserStateProperties, id2tt);
          QueryRunner(DB_SEARCH2_USERSTATE, CDbRServer).query(sql, handler);
        }
        {
          Statement sql;
          sql = generateSql(strSuffix, TABLE_USER_CONFIG, USER_CONFIG_FIELD, " id ");
          BatchResultHandlerI handler("id", &SearchCacheData::setConfigProperties, id2tt);
          QueryRunner(DB_SEARCH2_USERCONFIG, CDbRServer).query(sql, handler);
        }

        {
          Statement sql;
          sql = generateSql(strSuffix, TABLE_USER_NAME, USER_NAME_FIELD_TT, " id ");
          BatchResultHandlerI handler("id", &SearchCacheData::setUserNameProperties, id2tt);
          QueryRunner(DB_SEARCH2_USERNAME, CDbRServer).query(sql, handler);
        }
        {
          Statement sql;
          sql = generateSql(strSuffix, TABLE_USER_STAGE, USER_STAGE_FIELD, " id ");
          BatchResultHandlerI handler("id", &SearchCacheData::setUserStageProperties, id2tt);
          QueryRunner(DB_SEARCH2_USERSTAGE, CDbRServer).query(sql, handler);
        }
        /*  {
            Statement sql;
            sql = generateSql(strSuffix, TABLE_USER_VIEW, USER_VIEWCOUNT_FIELD_TT, " id ");
            BatchResultHandlerI handler("id", &SearchCacheData::setViewCountProperties, id2tt);
            QueryRunner(DB_SEARCH2_USERSTAGE, CDbRServer).query(sql, handler);
            }
            {
            Statement sql;
            sql = generateSql(strSuffix, TABLE_USER_TIME, USER_TIME_FIELD_TT, " id ");
            BatchResultHandlerI handler("id", &SearchCacheData::setLastlogintimeProperties, id2tt);
            QueryRunner(DB_SEARCH2_USERSTAGE, CDbRServer).query(sql, handler);
            }*/
        {
          for(int i=0; i<100; i++) {
            ostringstream table_name;
            table_name << "user_fond_" << i;
            Statement sql;
            sql = generateSql(strSuffix, table_name.str(), USER_FOND_FIELD_TT, " user_id ");
            BatchResultHandlerI handler("user_id", &SearchCacheData::setUserFondProperties, id2tt);
            QueryRunner(DB_SEARCH2_USERFOND, CDbRServer).query(sql, handler);
          }
        }

        {
          Statement sql;
          sql = generateSql(strSuffix, TABLE_NETWORK_HISTORY, NETWORK_HISTORY_FIELD, " userid ");
          map<long, vector<mysqlpp::Row> > id2RowsMap;
          BatchVectorResultHandlerI handler("userid", id2RowsMap);
          QueryRunner(DB_SEARCH2_USERBASIC, CDbRServer).query(sql, handler);

          for (map<long, vector<mysqlpp::Row> >::iterator iter = id2RowsMap.begin();
              iter != id2RowsMap.end(); 
              ++iter) {
            if (NULL != id2tt[iter->first].get()) {
              id2tt[iter->first]->setNetworkPropertiesAccordingVector(iter->second);
            }
          }
        }
        {
          Statement sql;
          sql = generateSql(strSuffix, TABLE_ELEMENTARY_SCHOOL, ELEMENTARY_SCHOOL_INFO_FIELD, " userid "); 
          map<long, vector<mysqlpp::Row> > id2RowsMap;
          BatchVectorResultHandlerI handler("userid", id2RowsMap);
          QueryRunner(DB_SEARCH2_USERBASIC, CDbRServer).query(sql, handler);

          for (map<long, vector<mysqlpp::Row> >::iterator iter = id2RowsMap.begin();
              iter != id2RowsMap.end(); 
              ++iter) {
            if (NULL != id2tt[iter->first].get()) {
              id2tt[iter->first]->setElementarySchoolPropertiesAccordingVector(iter->second);
            }
          }
        }
        {
          Statement sql;
          sql = generateSql(strSuffix, TABLE_JUNIOR_SCHOOL, JUNIOR_HIGH_SCHOOL_INFO_FIELD, " userid ");
          map<long, vector<mysqlpp::Row> > id2RowsMap;
          BatchVectorResultHandlerI handler("userid", id2RowsMap);
          QueryRunner(DB_SEARCH2_USERBASIC, CDbRServer).query(sql, handler);
          for (map<long, vector<mysqlpp::Row> >::iterator iter = id2RowsMap.begin();
              iter != id2RowsMap.end(); 
              ++iter) {
            if (NULL != id2tt[iter->first].get()) {
              id2tt[iter->first]->setJuniorSchoolPropertiesAccordingVector(iter->second);
            }
          }
        }
        {
          Statement sql;
          sql = generateSql(strSuffix, TABLE_HIGH_SCHOOL, HIGH_SCHOOL_INFO_FIELD, " userid ");
          map<long, vector<mysqlpp::Row> > id2RowsMap;
          BatchVectorResultHandlerI handler("userid", id2RowsMap);
          QueryRunner(DB_SEARCH2_USERBASIC, CDbRServer).query(sql, handler);
          for (map<long, vector<mysqlpp::Row> >::iterator iter = id2RowsMap.begin();
              iter != id2RowsMap.end(); 
              ++iter) {
            if (NULL != id2tt[iter->first].get()) {
              id2tt[iter->first]->setHighSchoolPropertiesAccordingVector(iter->second);
            }
          }
        }
        {
          Statement sql;
          sql = generateSql(strSuffix, TABLE_COLLEGE_INFO, COLLEGE_INFO_FIELD, " userid ");
          map<long, vector<mysqlpp::Row> > id2RowsMap;
          BatchVectorResultHandlerI handler("userid", id2RowsMap);
          QueryRunner(DB_SEARCH2_USERBASIC, CDbRServer).query(sql, handler);

          for (map<long, vector<mysqlpp::Row> >::iterator iter = id2RowsMap.begin();
              iter != id2RowsMap.end(); 
              ++iter) {
            if (NULL != id2tt[iter->first].get()) {
              id2tt[iter->first]->setCollegePropertiesAccordingVector(iter->second);
            }
          }
        }
        {
          Statement sql;
          sql = generateSql(strSuffix, TABLE_UNIVERSITY_INFO, UNIVERSITY_INFO_FIELD, " userid ");
          map<long, vector<mysqlpp::Row> > id2RowsMap;
          BatchVectorResultHandlerI handler("userid", id2RowsMap);
          QueryRunner(DB_SEARCH2_USERBASIC, CDbRServer).query(sql, handler);

          for (map<long, vector<mysqlpp::Row> >::iterator iter = id2RowsMap.begin();
              iter != id2RowsMap.end(); 
              ++iter) {
            if (NULL != id2tt[iter->first].get()) {
              id2tt[iter->first]->setUniversityPropertiesAccordingVector(iter->second);
            }
          }
        }
        {
          Statement sql;
          sql = generateSql(strSuffix, TABLE_WORKPLACE_INFO, WORKPLACE_INFO_FIELD, " userid ");
          map<long, vector<mysqlpp::Row> > id2RowsMap;
          BatchVectorResultHandlerI handler("userid", id2RowsMap);
          QueryRunner(DB_SEARCH2_USERBASIC, CDbRServer).query(sql, handler);

          for (map<long, vector<mysqlpp::Row> >::iterator iter = id2RowsMap.begin();
              iter != id2RowsMap.end(); 
              ++iter) {
            if (NULL != id2tt[iter->first].get()) {
              id2tt[iter->first]->setWorkplacePropertiesAccordingVector(iter->second);
            }
          }
        }
        {
          Statement sql;
          sql = generateSql(strSuffix, TABLE_REGION_INFO, REGION_INFO_FIELD, " userid ");
          map<long, vector<mysqlpp::Row> > id2RowsMap;
          BatchVectorResultHandlerI handler("userid", id2RowsMap);
          QueryRunner(DB_SEARCH2_USERBASIC, CDbRServer).query(sql, handler);

          for (map<long, vector<mysqlpp::Row> >::iterator iter = id2RowsMap.begin();
              iter != id2RowsMap.end(); 
              ++iter) {
            if (NULL != id2tt[iter->first].get()) {
              id2tt[iter->first]->setRegionPropertiesAccordingVector(iter->second);
            }
          }
        }
        vector<long> update_zoie_ids;
        for(map<long, SearchCacheDataPtr>::iterator iter = id2tt.begin(); iter != id2tt.end(); iter++)  { 
          SearchCacheMemDataPtr tmp = new SearchCacheMemData;
          id2cache[iter->first] = tmp;
          boost::shared_ptr<IDbCache> db_handler = SearchLogicManagerI::instance().getDbHandler(iter->first);
          if(NULL == iter->second.get()) {
            MCE_DEBUG("[UpdateTask::UpdateSqlBatch] iter->second is null");
            continue;
          }
          //TODO status>=3 从TT和SearchCache中删除
          if(NULL != iter->second.get() && iter->second->_status >= STATUS_LIMIT) {
            MCE_DEBUG("[UpdateTask::UpdateSqlBatch] iter->second->_status:" << iter->second->_status << " >= " << STATUS_LIMIT);
            if(!db_handler->Remove(iter->first))  {
              MCE_INFO("[UpdateTask::UpdateSqlBatch] Failed of removing banned id = "<<iter->first<<" in TT!");
            }
            MCE_DEBUG("[UpdateTask::UpdateSqlBatch] remove banned id = "<<iter->first<<" in TT!");
            continue;
          }
          update_zoie_ids.push_back(iter->first);
          transformData(iter->second, tmp);
          if(db_handler)  {
            if(!db_handler->Insert(iter->first,*(iter->second).get()))  {
              MCE_INFO("Failed of updating id = "<<iter->first<<" in TT!");
            }
          }
        }
        MCE_DEBUG("[UpdateTask::UpdateSqlBatch] insert");
        DistSearchCacheAdapter::instance().setData(id2cache);
        ZoieUpdater::instance().insertBatch(update_zoie_ids, ZOIE_ADD);
        return true;
      }

    }
  }
}
