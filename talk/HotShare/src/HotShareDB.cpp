#include "HotShareDB.h"
#include "QueryRunner.h"
#include "Config.h"
#include "LogWrapper.h"

using namespace std;
using namespace MyUtil;
using namespace com::xiaonei::xce;
using namespace xce::hotshare;

bool HotShareDB::InsertID(Ice::Long localID, Ice::Long srcID)
{
  Statement sql;
  sql << "INSERT INTO hotshare_video_id(local_id, src_id) VALUES (" << localID <<", "<< srcID <<")";
  try
  {
    QueryRunner("im", CDbWServer, "hotshare_video_id_test").store(sql);

  }
  catch (std::exception &e)
  {
    MCE_WARN("HotShareDB::InsertID: "<<e.what());
    return false;
  }
  catch (...)
  {
    MCE_WARN("HotShareDB::InsertID: exception");
    return false;
  }

  MCE_DEBUG("HotShareDB::InsertID  Message---->succeed to insert  srcID:"<<srcID<<"   localID:"<<localID);
  return true;
}


bool HotShareDB::DeleteID( Ice::Long localID)
{
  Statement sql;
  sql << "DELETE FROM hotshare_video_id WHERE local_id>"<<localID;
  try
  {
    QueryRunner("im", CDbWServer, "hotshare_video_id_test").store(sql);

  }
  catch (std::exception &e)
  {
    MCE_WARN("HotShareDB::DeleteID: "<<e.what());
    return false;
  }
  catch (...)
  {
    MCE_WARN("HotShareDB::DeleteID: exception");
    return false;
  }

  MCE_DEBUG("HotShareDB::DeleteID  Message----> succeed to delete  localID:"<<localID );
  return true;
}


bool HotShareDB::GetID(vector<IDInfo> &IDInfoVec, Ice::Long maxLocalID,  size_t size )
{

  IDInfo ii;

  Statement sql;  
  sql << "SELECT local_id, src_id FROM hotshare_video_id WHERE  local_id>" << maxLocalID <<" limit "<<size;
  try 
  {
    mysqlpp::StoreQueryResult res = QueryRunner( "im",CDbRServer,"hotshare_video_id_test").store(sql);
    for (size_t i=0; i<res.num_rows(); i++)
    {
      mysqlpp::Row row = res.at(i);
      ii.DBID = static_cast<Ice::Long>(row["src_id"]);
      ii.localID = static_cast<Ice::Long>(row["local_id"]);

      IDInfoVec.push_back(ii);
     }
    
  }
  catch (std::exception &e)
  {
    MCE_WARN("HotShareDB::GetID: "<< e.what());
    return false;
  }
  catch (...)
  {
    MCE_WARN("HotShareDB::GetID: exception");
    return false;
  }  

  MCE_DEBUG("HotShareDB::GetID  Message----> IDInfoVec.size:"<<IDInfoVec.size());

  return true;
}


bool HotShareDB::GetHotShare(vector<HotShareInfo> &hsVec, Ice::Long maxDBID, size_t size, Ice::Long &retMaxDBID)
{
  retMaxDBID = 0;

  HotShareInfo hs;
  Statement sql;  
  sql << "SELECT id, share_id, user_id,  title, url, summary, creation_date, user_name, thumb_url, main_url, head_url"
   " FROM hot_share WHERE stage=0 AND type=10 AND id>" << maxDBID <<" limit " << size;
  try 
  {
    mysqlpp::StoreQueryResult res = QueryRunner( "share_info_new",CDbRServer,"hot_share").store(sql);
    for (size_t i=0; i<res.num_rows(); i++)
    {
      mysqlpp::Row row = res.at(i);
      hs.id = static_cast<Ice::Long>(row["id"]);
      hs.share_id = static_cast<Ice::Long>(row["share_id"]);
      hs.user_id = static_cast<unsigned int>(row["user_id"]);
      row["title"].to_string(hs.title);
//      row["url"].to_string(hs.url);
      hs.url = "";
      row["summary"].to_string(hs.summary);
      row["creation_date"].to_string(hs.creation_date);
      row["user_name"].to_string(hs.user_name);
      row["thumb_url"].to_string(hs.thumb_url);
      row["main_url"].to_string(hs.main_url);
      row["head_url"].to_string(hs.head_url);
      hs.type = 0;
      

      hsVec.push_back(hs);

      if (hs.id > retMaxDBID)
        retMaxDBID = hs.id;
     }
    
  }
  catch (std::exception &e)
  {
    MCE_WARN("HotShareDB::GetHotShare: "<< e.what());
    return false;
  }
  catch (...)
  {
    MCE_WARN("HotShareDB::GetHotShare: exception");
    return false;
  }  
  
  MCE_DEBUG("HotShareDB::GetHotShare   Message---->   hsVec.size:"<<hsVec.size());

  return true;
}


