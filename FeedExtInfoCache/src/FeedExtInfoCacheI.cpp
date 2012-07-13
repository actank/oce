#include "FeedExtInfoCacheI.h"
#include <iostream>
#include <sstream>
#include <boost/algorithm/string.hpp>
#include "IceLogger.h"
#include "DbHelper.h"

using namespace std;
using namespace xce::feed::count;
using namespace com::xiaonei::xce;

void MyUtil::initialize() {
  ServiceI& service = ServiceI::instance();
  service.getAdapter()->add(&FeedExtInfoCacheI::instance(), service.createIdentity("M", ""));
  TaskManager::instance().scheduleRepeated(&ClearDirtyCacheTimer::instance()); 
}

namespace xce {

namespace feed {

namespace count {

  ostream& operator<<(ostream& os, const DbItem& item) {
    os << "new_item: " << " source:" << item.source() << " actor:" << item.actor() << " stype:" << item.stype() << endl;
    std::map< ::Ice::Byte, ::Ice::Int > m =  item.count_map();
    os << "count:" << endl;
    for (std::map< ::Ice::Byte, ::Ice::Int >::iterator it = m.begin(); it != m.end(); it++) {
      short s = it->first;
      os << s << " " << it->second <<endl;

    }
    return os;

  }


/*  ostringstream& operator<<(ostringstream& os, const DbItem& item) {
    os << "new_item: " << " source:" << item.source() << " actor:" << item.actor() << " stype:" << item.stype() << endl;
    std::map< ::Ice::Byte, ::Ice::Int > m =  item.count_map();
    os << "count:" << endl;
    for (std::map< ::Ice::Byte, ::Ice::Int >::iterator it = m.begin(); it != m.end(); it++) {
      int s = it->first;
      os << s << " " << it->second <<endl;

    }
    return os;

  }*/



void FeedExtInfoCacheI::SetCount(::Ice::Long source, ::Ice::Int stype, ::Ice::Int actor, ::Ice::Byte index, ::Ice::Int count, const ::Ice::Current&) {
  short s = index;
  MCE_INFO("FeedExtInfoCacheI::SetCount --> source: " << source << " stype:" << stype << " actor:" << actor << " index:" << s << " count:" << count);
  if (index > kMaxCountPerFeed || index < 0) {
    MCE_WARN("FeedExtInfoCacheI::SetCount --> index: " << s << " out of range!");
    return ;
  }
    
  DbItem db_item(source, stype, actor, index, count);
  dirty_cache_.Update(db_item);
    
  short ss = index;
  EvictorObjPtr obj_ptr = new EvictorObj(actor, count, index);
  
  {
    IceUtil::Mutex::Lock lock(evictor_map_mutex_);
    if (!rwmutex_map_.count(stype)) {
      MCE_INFO("FeedExtInfoCacheI::SetCount --> stype:" << stype << " no rwmutex!");
      IceUtil::RWRecMutex* mutex_ptr= new IceUtil::RWRecMutex; // 该stype所对应的锁 
      rwmutex_map_.insert(make_pair(stype, mutex_ptr));
    }

  }
 
  {
    IceUtil::RWRecMutex::WLock lock(*rwmutex_map_[stype]);

    map< ::Ice::Int, CountEvictor* >::iterator  it = evictor_map_.find(stype);
      
    if (it == evictor_map_.end()) {
      CountEvictor* evictor_ptr = new CountEvictor(kDefaultEvictorSize); 
      evictor_map_.insert(make_pair(stype, evictor_ptr));
      evictor_ptr->add(obj_ptr, source);
      MCE_DEBUG("FeedExtInfoCacheI::SetCount --> new evictor. source: " << source << " stype:" << stype << " actor:" << actor << " index:" << s << " count:" << count);
      return ;
    } 

    
    CountEvictor* evictor_ptr = it->second;
    EvictorObjPtr  value = evictor_ptr->find(source, NULL);

    if (value) {   
      EvictorObjPtr obj_list_ptr = value;
      while (obj_list_ptr) {
        if (obj_list_ptr->actor() == actor && obj_list_ptr->index() == index) { 
          obj_list_ptr->set_count(count);
          evictor_ptr->locate(source, NULL);  //最近访问，把value链表提到访问队列最前端
          MCE_DEBUG("FeedExtInfoCacheI::SetCount --> update obj. source: " << source << " stype:" << stype << " actor:" << actor << " index:" << s << " count:" << count);
          return ;
        }
        obj_list_ptr = obj_list_ptr->next();
      }
      obj_ptr->set_next(value);
      evictor_ptr->add(obj_ptr, source);   //添加到链表最前端，重新插入evictor
      MCE_DEBUG("FeedExtInfoCacheI::SetCount --> new obj. source: " << source << " stype:" << stype << " actor:" << actor << " index:" << s << " count:" << count);
  
    } else {
      evictor_ptr->add(obj_ptr, source);
      MCE_DEBUG("FeedExtInfoCacheI::SetCount --> new obj list. source: " << source << " stype:" << stype << " actor:" << actor << " index:" << s << " count:" << count);
    }
  } //里面被锁住了
}


void FeedExtInfoCacheI::SetCount(DbItem& item) {
  Ice::Long source = item.source();
  Ice::Int stype = item.stype();
  Ice::Int actor = item.actor();
  map< ::Ice::Byte, ::Ice::Int > count_map = item.count_map();
  ostringstream oss;
  
  oss << "new_item: " << " source:" << item.source() << " actor:" << item.actor() << " stype:" << item.stype() << endl;
  map< ::Ice::Byte, ::Ice::Int > m =  item.count_map();
  oss << "count map:" << endl;
  for (std::map< ::Ice::Byte, ::Ice::Int >::iterator it = m.begin(); it != m.end(); it++) {
    int s = it->first;
    oss << s << " " << it->second <<endl;

  }

  {
    IceUtil::Mutex::Lock lock(evictor_map_mutex_);
    if (!rwmutex_map_.count(stype)) {
      MCE_INFO("FeedExtInfoCacheI::SetCount(DbItem&) --> stype:" << stype << " no rwmutex!");
      IceUtil::RWRecMutex* mutex_ptr= new IceUtil::RWRecMutex; // 该stype所对应的锁 
      rwmutex_map_.insert(make_pair(stype, mutex_ptr));
    }

  }


  {
    IceUtil::RWRecMutex::WLock lock(*rwmutex_map_[stype]);
    MCE_INFO("FeedExtInfoCacheI::SetCount(DbItem&) --> " << oss.str());

    EvictorObjPtr obj_list_start = NULL;
    for (map< ::Ice::Byte, ::Ice::Int > :: iterator it = count_map.begin(); it != count_map.end(); it++) {

      EvictorObjPtr obj_ptr = new EvictorObj(actor, it->second, it->first);
      
      if (!obj_list_start) {
        obj_list_start = obj_ptr; 
        continue;
      } 
      obj_ptr->set_next(obj_list_start);
      obj_list_start = obj_ptr; 
    }    

     
    map< ::Ice::Int, CountEvictor* >::iterator it = evictor_map_.find(stype);
      
    if (it == evictor_map_.end()) {
      MCE_INFO("FeedExtInfoCacheI::SetCount(DbItem&) --> new evictor."); 
      CountEvictor* evictor_ptr = new CountEvictor(kDefaultEvictorSize); 
      evictor_map_.insert(make_pair(stype, evictor_ptr));
      evictor_ptr->add(obj_list_start, source);
      return ;
    } 

      
    CountEvictor* evictor_ptr = it->second;
    EvictorObjPtr value = evictor_ptr->find(source, NULL);
    EvictorObjPtr inserted_ptr = NULL;
    EvictorObjPtr new_start = obj_list_start; 
    if (value) {
      while (value) {
        EvictorObjPtr ptr_of_new_list = obj_list_start;
        bool add_new_obj = true;
        while (ptr_of_new_list) {
          if (value->actor() == ptr_of_new_list->actor() &&
              value->index() == ptr_of_new_list->index()) {
            add_new_obj = false;
            short ind = value->index();
            MCE_DEBUG("FeedExtInfoCacheI::SetCount(DbItem&) --> same actor:" << value->actor() << " same index:" << ind); 
            break;  
          }
          ptr_of_new_list = ptr_of_new_list->next();
        } 
       
        inserted_ptr = value;
        value = value->next();
        inserted_ptr->set_next(NULL); 
        
        if (add_new_obj) {
          inserted_ptr->set_next(new_start);
          new_start = inserted_ptr;
          short inde = new_start->index();
          MCE_DEBUG("FeedExtInfoCacheI::SetCount(DbItem&) --> add new obj actor:" << new_start->actor() << " index:" << inde); 
        } 
      }  
      evictor_ptr->add(new_start, source);
     
    } else {
      //cout << "SetCount new evictorObj list" << endl;
      MCE_DEBUG("FeedExtInfoCacheI::SetCount(DbItem&) --> new obj list."); 
      evictor_ptr->add(obj_list_start, source);
    }
  } //锁
}





::Ice::Int FeedExtInfoCacheI::QueryDbAndUpdateEvictor(::Ice::Long source, ::Ice::Int stype, ::Ice::Int actor, ::Ice::Byte index) { 
  short s = index;
  MCE_INFO("FeedExtInfoCacheI::QueryDbAndUpdateEvictor --> source:" << source << " stype:" << stype << " actor:" << actor << " index:" << s);
  pair<bool, DbItem> rt = QueryDb(source, stype, actor);
  if (rt.first) {
    DbItem item = rt.second;
    SetCount(item);      
    map< ::Ice::Byte, ::Ice::Int > rt_map = item.count_map();
    return rt_map[index];
  } else {
    return 0;

  }

}


::Ice::Int FeedExtInfoCacheI::GetCount(::Ice::Long source, ::Ice::Int stype, ::Ice::Int actor, ::Ice::Byte index, const ::Ice::Current&) {
   

//////////////////////////////////
  for (std::map< ::Ice::Int, CountEvictor* >::iterator it = evictor_map_.begin(); it != evictor_map_.end(); it++) {
    MCE_DEBUG("stype:" << "stype:" <<  it->first << "===========================");
    CountEvictor* evic_ptr = it->second;
    MCE_DEBUG("size:" << evic_ptr->size());
  }
  short s = index;
  MCE_DEBUG("FeedExtInfoCacheI::GetCount --> stype:" << stype << "  source:" << source << " actor:" << actor << " index:" << s);

  /////////////////
  {
    // 重新启动服务后，需要重新插入锁
    IceUtil::Mutex::Lock lock(evictor_map_mutex_);
    if (!rwmutex_map_.count(stype)) {
      MCE_INFO("FeedExtInfoCacheI::GetCount --> stype:" << stype << " no rwmutex!");
      IceUtil::RWRecMutex* mutex_ptr= new IceUtil::RWRecMutex; // 该stype所对应的锁 
      rwmutex_map_.insert(make_pair(stype, mutex_ptr));
    }

  }

  {
    IceUtil::RWRecMutex::RLock lock(*rwmutex_map_[stype]);

    map< ::Ice::Int, CountEvictor* >::iterator it = evictor_map_.find(stype);
    if (it != evictor_map_.end()) {
      CountEvictor* evictor_ptr = it->second;
      if(!evictor_ptr) {
        MCE_WARN("FeedExtInfoCacheI::GetCount --> stype: " << stype << " evictor_ptr == NULL !");
        return 0; 
      }
   
      EvictorObjPtr  value = evictor_ptr->find(source, NULL);
      ///////////////
      EvictorObjPtr obj_ptr= value;
      MCE_DEBUG("FeedExtInfoCacheI::GetCount --> stype: " << stype  << " source:" << source);
      while(obj_ptr) {
        short inde = obj_ptr->index();

        MCE_DEBUG("obj:" << obj_ptr->actor() << " " << inde << " " << obj_ptr->count() << " || ");
        obj_ptr = obj_ptr->next();
      }
        /////////
      while(value) {
        if (value->actor() == actor && value->index() == index) {
          evictor_ptr->locate(source, NULL);  // 最近访问，把value链表提到访问队列最前端
          return value->count();
        }
        value = value->next();
      }
      // ::Ice::Int rt_count = QueryDbAndUpdateEvictor(source, stype, actor, index);
        
    } else {
      MCE_DEBUG("FeedExtInfoCacheI::GetCount --> new evictor. stype: " << stype);
    }
  }
  ::Ice::Int rt_count = QueryDbAndUpdateEvictor(source, stype, actor, index);
  return rt_count;
}

void DirtyCache::pf() {
  MCE_INFO("DirtyCache::pf --> cache size:" << dirty_map_.size());
  for (map< short,  DbItemMap >::iterator it = dirty_map_.begin(); it!=dirty_map_.end();it++) {
    MCE_INFO("DirtyCache::pf --> bucket:" << it->first << "-------------");
    DbItemMap m = it->second;
    for (DbItemMap::iterator it2 = m.begin(); it2!=m.end(); it2++) {
      MCE_INFO("DirtyCache::pf --> source:" << it2->first);
      vector<DbItem> vec = it2 ->second;
      for (vector<DbItem>::iterator it3 = vec.begin(); it3!=vec.end(); it3++) {
        DbItem item = *it3;
        MCE_INFO("DirtyCache::pf --> new_item source:" << item.source() << " actor:" << item.actor() << " stype:" << item.stype());
        std::map< ::Ice::Byte, ::Ice::Int > m =  item.count_map();
        MCE_INFO("DirtyCache::pf --> count map");
        for (std::map< ::Ice::Byte, ::Ice::Int >::iterator it = m.begin(); it != m.end(); it++) {
          short s = it->first;
          MCE_INFO("DirtyCache::pf -->" <<  s << " " << it->second);

        }
      }

    }

  }

}


  void DirtyCache::Update(const DbItem& new_item) {
     IceUtil::Mutex::Lock lock(dirty_mutex_);
     MCE_DEBUG("DirtyCache::Update --> source:" << new_item.source() 
         << " stype:" << new_item.stype() << " actor:" 
         << new_item.actor());
     ::Ice::Long item_source = new_item.source(); 

     if (dirty_map_.count(item_source % DbHelper::kTableCount)) {
      
       std::map< short,  DbItemMap >::iterator map_it = dirty_map_.find(item_source % DbHelper::kTableCount);   
       
       if ((map_it->second).count(item_source)) {
            DbItemMap::iterator it = (map_it->second).find(item_source);
            vector<DbItem>& item_vec = it->second;
            int i = 0;
            for (; i < item_vec.size(); i++) {
               DbItem& item = item_vec[i];
               if (item.actor() == new_item.actor() && item.stype() == new_item.stype()) {
                 map< ::Ice::Byte, ::Ice::Int > new_count_map = new_item.count_map();
                 for (map<Ice::Byte, Ice::Int>::iterator count_map_it = new_count_map.begin();
                      count_map_it != new_count_map.end(); count_map_it++) {
                    
                   item.SetCount(count_map_it->first, count_map_it->second);
                   
                 }
                 MCE_DEBUG("DirtyCache::Update --> update vec. source:" << item_source);
                 break;
               }
            }       
            if (i == item_vec.size()) {
              MCE_DEBUG("DirtyCache::Update --> push into vec. source:" << item_source);
              item_vec.push_back(new_item);
            }
            
       } else {
         MCE_DEBUG("DirtyCache::Update --> new vec.");
         vector<DbItem> vec;
         vec.push_back(new_item);
         (map_it->second).insert(make_pair(item_source, vec));
       }    
   
     } else {
     //  cout << "dirty map new bucket:" << item_source % DbHelper::kTableCount<< endl;
       MCE_DEBUG("DirtyCache::Update --> new bucket. source:" << item_source);
       vector<DbItem> vec;
       vec.push_back(new_item);
       dirty_map_[item_source % DbHelper::kTableCount][item_source] = vec;
 
     }

    //pf();

  }


void DirtyCache::Clear() {
  MCE_INFO("DirtyCache::Clear");
  map<short, DbItemMap> tmp_map;
  {
    IceUtil::Mutex::Lock lock(dirty_mutex_);
    dirty_map_.swap(tmp_map);
  }
    
  for (map<short, DbItemMap>::iterator it = tmp_map.begin(); it != tmp_map.end(); it++) {
    MCE_INFO("DirtyCache::Clear --> table no:" << it->first << " map size:" << it->second.size()); 

    SaveDb(it->first, it->second);
  }

   

} 

pair<bool, DbItem> FeedExtInfoCacheI::QueryDb(Ice::Long source, Ice::Int stype, Ice::Int actor) {
  MCE_INFO("FeedExtInfoCacheI::QueryDb --> source:" << source << " stype:" << stype << " actor:" << actor);

  Statement sql;
  sql << "SELECT * FROM " << DbHelper::GetTableName(source) << " WHERE " << DbHelper::kColSource
      << " = " << source << " AND " << DbHelper::kColStype 
      << " = " << stype << " AND " << DbHelper::kColActor 
      << " = " << actor;
    
  mysqlpp::StoreQueryResult res;
  
  try {
    res = QueryRunner(DbHelper::kDbLogicName, CDbRServer, DbHelper::GetExpr(source)).store(sql);
  } catch (const Ice::Exception& e) {
    MCE_WARN("FeedExtInfoCacheI::QueryDb --> sql ice exception:" << e);
  } catch (const std::exception& e) {
    MCE_WARN("FeedExtInfoCacheI::QueryDb --> sql exception:" << e.what());
  }

  if (!res) {
    MCE_WARN("FeedExtInfoCacheI::QueryDb --> res null:" << source);
    return make_pair(false, DbItem());
  }
  
  MCE_DEBUG("FeedExtInfoCacheI::QueryDb --> rows:" << res.num_rows()); 
    
  if (res.num_rows()>0) {
    DbItem db_item(source, stype, actor);
    for (int i = 0; i < res.num_rows(); i++) {
      mysqlpp::Row& row = res.at(i);
      Ice::Int count = row[DbHelper::kColCount.c_str()];
      Ice::Byte index = row[DbHelper::kColIndex.c_str()];  
      short s = index;
      MCE_DEBUG("FeedExtInfoCacheI::QueryDb --> index:" << s << " count:" << count);
      db_item.SetCount(index, count);
    }
      
    return make_pair(true,db_item);      

  } else {
    return make_pair(false,DbItem());
  }

}

void DirtyCache::SaveDb(short db_table_no, const DbItemMap& db_item_map) {
  MCE_INFO("FeedExtInfoCacheI::SaveDb --> db_table_no:" << db_table_no);

  //map<Ice::Int, std::vector<DbItem> > 
  DbItemMap::const_iterator it = db_item_map.begin();
  while (it != db_item_map.end()) {
    Statement sql;
    
    string table_name = DbHelper::GetTableName(db_table_no);
    string expr = DbHelper::GetExpr(db_table_no);
    sql << "insert into " << table_name << " (" << DbHelper::kColSource
        << "," << DbHelper::kColActor << "," << DbHelper::kColStype 
        << "," << DbHelper::kColIndex << ","
        << DbHelper::kColCount  << ") values ";


    int i = 0;
    for (; it != db_item_map.end() && i < 1000; it++) {
      short db_table_no = it->first;
      const vector<DbItem>& item_vec = it->second;
      for (vector<DbItem>::const_iterator vec_it = item_vec.begin(); vec_it != item_vec.end(); vec_it++) {
        map< ::Ice::Byte, ::Ice::Int > index2count_map = vec_it->count_map();
        for (map< ::Ice::Byte, ::Ice::Int >::iterator count_map_it = index2count_map.begin();
          count_map_it != index2count_map.end(); count_map_it++, i++) {
          if (i) {
            sql << ",";  
          }
          short index = count_map_it->first;
           
          sql << "(" << vec_it->source() << ","
              << vec_it->actor() << "," << vec_it->stype()  << "," << index << ","
              << count_map_it->second << ")";         
        }
      }
    }

    sql << " on duplicate key update " << DbHelper::kColCount<< "=values("
        << DbHelper::kColCount << ")";

    try {
      QueryRunner(DbHelper::kDbLogicName, CDbWServer, expr).schedule(sql);
    } catch (const Ice::Exception& e) {
      MCE_INFO("FeedInteractionCacheI::SaveDb --> sql ice exception:" << e);
    } catch (const std::exception& e) {
      MCE_INFO("FeedInteractionCacheI::SaveDb --> sql exception:" << e.what());
    }
 
  }



}
    




bool FeedExtInfoCacheI::SetEvictorSize(::Ice::Int stype, ::Ice::Int evictor_size, const ::Ice::Current&) {

  {
    IceUtil::Mutex::Lock lock(evictor_map_mutex_);
    if (!rwmutex_map_.count(stype)) {
      MCE_INFO("FeedExtInfoCacheI::SetEvictorSize --> stype:" << stype << " no rwmutex!");
      IceUtil::RWRecMutex* mutex_ptr= new IceUtil::RWRecMutex; // 该stype所对应的锁 
      rwmutex_map_.insert(make_pair(stype, mutex_ptr));
    }

  }

  IceUtil::RWRecMutex::WLock lock(*rwmutex_map_[stype]);
  std::map< ::Ice::Int, CountEvictor* >::iterator it = evictor_map_.find(stype);
  if (it == evictor_map_.end()) {
    MCE_INFO("FeedExtInfoCacheI::GetEvictorSize --> No evictor.  stype:" << stype);
    return false;
  }

  CountEvictor* evictor_ptr = it->second;
  if (evictor_ptr) {
    evictor_ptr->size(evictor_size);
    MCE_INFO("FeedExtInfoCacheI::GetEvictorSize --> stype:" << stype << " evictor size:" << evictor_ptr->size());
    return true;
  } 
  return false;

}

 
Ice::Long FeedExtInfoCacheI::GetEvictorSize(::Ice::Int stype,  const ::Ice::Current&) {
  {
    IceUtil::Mutex::Lock lock(evictor_map_mutex_);
    if (!rwmutex_map_.count(stype)) {
      MCE_INFO("FeedExtInfoCacheI::GetEvictorSize --> stype:" << stype << " no rwmutex!");
      IceUtil::RWRecMutex* mutex_ptr= new IceUtil::RWRecMutex; // 该stype所对应的锁 
      rwmutex_map_.insert(make_pair(stype, mutex_ptr));
    }

  }

  IceUtil::RWRecMutex::RLock lock(*rwmutex_map_[stype]);
  std::map< ::Ice::Int, CountEvictor* >::iterator it = evictor_map_.find(stype);
  if (it == evictor_map_.end()) {
    MCE_INFO("FeedExtInfoCacheI::GetEvictorSize --> No evictor.  stype:" << stype); 
    return 0;
  }

  CountEvictor* evictor_ptr = it->second;
  if (evictor_ptr) {
    MCE_INFO("FeedExtInfoCacheI::GetEvictorSize --> stype:" << stype << " evictor size:" << evictor_ptr->size());
    return  evictor_ptr->size();
  }  
  return 0;

}

void FeedExtInfoCacheI::ClearDirtyCache() {
  dirty_cache_.Clear();
}

void ClearDirtyCacheTimer::handle() {
  MCE_INFO("ClearDirtyCacheTimer::handle");
  FeedExtInfoCacheI::instance().ClearDirtyCache();
}


}

}

}


int main() {
// cout <<"size long:" <<sizeof(long long) << endl;
typedef IceUtil::Handle<FeedExtInfoCacheI> FeedExtInfoCacheIPtr;
FeedExtInfoCacheIPtr  fi = new xce::feed::count::FeedExtInfoCacheI();




for (int source =1; source <= 1999999;source++) {
 fi->SetCount(source, 1001, 500, 1, source);
}

cout << "count:" <<  fi->GetCount(1000000,1001,500,1) << endl;
fi->SetCount(2000000, 1001, 500, 1, 2000000);

cout << "count:" <<  fi->GetCount(1000000,1001,500,1) << endl;
cout << "count:" <<  fi->GetCount(1000001,1001,500,1) << endl;
cout << "count:" <<  fi->GetCount(1000002,1001,500,1) << endl;
/*
  fi->SetCount(10000, 1001, 500, 1, 22);
fi->SetCount(10001, 1001, 500, 1, 33);
fi->SetCount(10001, 1021, 500, 1, 44);
fi->SetCount(10001, 1001, 600, 1, 55);
fi->SetCount(10001, 1001, 500, 2, 66);
fi->SetCount(10001, 1001, 600, 1, 77);

fi->SetCount(10002, 1001, 500, 1, 88);
fi->SetCount(10003, 1001, 500, 1, 99);  */
//  fi->SetCount(source, 1001, 500, 2, source);


/*
cout << "count:" <<  fi->GetCount(10000,1001,500,1) << endl;
cout << "count:" <<  fi->GetCount(10001,1001,500,1) << endl;
cout << "count:" <<  fi->GetCount(10001,1001,500,2) << endl;
cout << "count:" <<  fi->GetCount(10001,1021,500,1) << endl;
cout << "count:" <<  fi->GetCount(10001,1001,600,1) << endl;
cout << "count:" <<  fi->GetCount(10002,1001,500,1) << endl;
cout << "count:" <<  fi->GetCount(10003,1001,500,1) << endl;  */
//fi->SetCount(19000003,1001,500,1,45678);

//for (int source =20000001; source <= 20000010;source++) {
  //fi->SetCount(source, 1001, 500, 1, source);

//}



//cout << "count:" <<  fi->GetCount(2,1001,500,1) << endl;
//cout << "count:" <<  fi->GetCount(19000004,1001,500,1) << endl;
//cout << "count:" <<  fi->GetCount(19000002,1001,500,1) << endl;
//cout << "count:" <<  fi->GetCount(19000003,1001,500,1) << endl;
//fi->SetCount(1000002, 1001, 500, 1, 888);
//fi->SetCount(1000002, 1001, 500, 2, 999);
//fi->SetCount(1000002, 1001, 501, 1, 777);

cout << "please enter a word:" << endl;
string str;
cin >> str;

cout << "input:" << str << endl;


//bool ret = fi->SetEvictorSize(1,1001,3);

//cout << "1001 evictor set size:" << ret << endl;

/*
fi->SetCount(210, 1001, 500, 1, 10);
bool b = fi->SetEvictorSize(1,1001,3);
cout << "bool:" << b << endl;

fi->SetCount(211, 1001, 510, 1, 5);
fi->SetCount(211, 1001, 520, 1, 6);
fi->SetCount(211, 1001, 510, 2, 7);

fi->SetCount(212, 1001, 810, 2, 4);

cout << "count:" <<  fi->GetCount(210,1001,500,1) << endl;
cout << "count:" << fi->GetCount(211,1001,520,1) << endl;
fi->SetCount(213, 1001, 910, 1, 12);

fi->SetCount(310, 1002, 610, 1, 100);
fi->SetCount(310, 1002, 710, 1, 200);
fi->SetCount(310, 1002, 810, 1, 300);

cout << "count:" << fi->GetCount(211,1001,520,1) << endl;
cout << "count:" << fi->GetCount(211,1001,510,2) << endl;
cout << "count:" << fi->GetCount(310,1002,610,1) << endl;
cout << "count:" << fi->GetCount(310,1002,610,1) << endl;
cout << "count:" << fi->GetCount(210,1001,500,1) << endl;
cout << "count:" << fi->GetCount(211,1001,510,1) << endl;
cout << "count:" << fi->GetCount(211,1001,520,1) << endl;
cout << "count:" << fi->GetCount(212,1001,810,2) << endl;

fi->SetCount(214,1001,910,2,13);
cout << "count:" << fi->GetCount(214,1001,910,2) << endl;
cout << "count:" << fi->GetCount(212,1001,810,2) << endl;
cout << "count:" << fi->GetCount(210,1001,500,1) << endl;
cout << "count:" << fi->GetCount(212,1001,810,2) << endl;
cout << "count:" << fi->GetCount(213,1001,910,1) << endl;  */
//xce::feed::count::FeedExtInfoCacheI::instance();
return 0;
}



