#include "DataProviderDB.h"
#include "QueryRunner.h"
#include "LogWrapper.h"

using namespace std;
using namespace MyUtil;
using namespace com::xiaonei::xce;
using namespace xce::dataprovider;

bool DataProviderDB::GetHotShare(Ice::Long &max_db_id,  std::vector<HotShareInfo> &hotshare_vector, size_t size) {
  HotShareInfo hs;
  Ice::Long temp_max_db_id = max_db_id;
  Statement sql;  
  sql << "SELECT id, share_id, user_id,  title, url, summary, creation_date, release_date, user_name, thumb_url, catagory FROM "
    << "hotshare" <<" WHERE id>" << (max_db_id+1) <<" limit " << size;
  try {
    mysqlpp::StoreQueryResult res = QueryRunner("im", CDbRServer, "hotshare").store(sql);
    for (size_t i=0; i<res.num_rows(); i++) {
      mysqlpp::Row row = res.at(i);
      hs.id = static_cast<Ice::Long>(row["id"]) - 1;
      hs.share_id = static_cast<Ice::Long>(row["share_id"]);
      hs.user_id = static_cast<size_t>(row["user_id"]);
      row["title"].to_string(hs.title);
      row["url"].to_string(hs.url);
      row["summary"].to_string(hs.summary);
      row["creation_date"].to_string(hs.creation_date);
      row["release_date"].to_string(hs.release_date);
      row["user_name"].to_string(hs.user_name);
      row["thumb_url"].to_string(hs.thumb_url);
      row["catagory"].to_string(hs.catagory);

      hotshare_vector.push_back(hs);
      if (hs.id > temp_max_db_id)
        temp_max_db_id = hs.id;
     }
    max_db_id = temp_max_db_id;
  }
  catch (std::exception &e) {
    MCE_WARN("DataProviderDB::GetHotShare: "<< e.what());
    return false;
  }
  catch (...) {
    MCE_WARN("DataProviderDB::GetHotShare: exception");
    return false;
  }  
  return true;
}

