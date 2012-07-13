/*
 * =====================================================================================
 *
 *       Filename:  common.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  11/17/2010 05:47:50 PM
 *       Revision:  none
 *       Compiler:  g++
 *
 *         Author:  min.shang (Jesse Shang), min.shang@opi-corp.com
 *        Company:  Opi-Corp
 *
 * =====================================================================================
 */

#ifndef __COMMON_H__
#define __COMMON_H__

#include "Date.h"
#include "PageCommon.h"
#include "FansPage.h"
#include <string>
#include <QueryRunner.h>

using namespace ::com::xiaonei::xce;
using namespace std;

namespace xce {
namespace buddy {

const string DATA_SOURCE = "fans_pages_a";
const string TABLE_PREFIX = "fans_pages_";

struct CompPageEntry : public std::binary_function<PageEntryPtr, PageEntryPtr, bool> {
  bool operator()(const PageEntryPtr& ptr1, const PageEntryPtr& ptr2) {
    return ptr1->pageId < ptr2->pageId;
  }
};

struct CompPageEntryByStateAddTime {
  bool operator()(const PageEntryPtr& ptr1, const PageEntryPtr& ptr2) {
    return ptr1->state > ptr2->state || ptr1->state == ptr2->state && ptr1->addTime > ptr2->addTime;
  }
};

struct EqualPageEntry {
  bool operator()(const PageEntryPtr& ptr1, const PageEntryPtr& ptr2) {
    return ptr1->id == ptr2->id && ptr1->pageId == ptr2->pageId && ptr1->userId == ptr2->userId && ptr1->addTime == ptr2->addTime
      && ptr1->operatorId == ptr2->operatorId && ptr1->state == ptr2->state;
  }
};

class PageEntrySetI : virtual public Ice::Object, virtual public IceUtil::Mutex {
  public:
    PageEntrySetI(PageEntryList list, bool sorted = false);
    PageEntrySetI();
    void updateState(int pageId, int state);
    void updatePageEntry(const PageEntryPtr& page);
    void insertPageEntry(const PageEntryPtr& page);
    void insertPageEntryBatch(const PageEntryList& page);
    // pageId等于-1时表示删掉所有元素
    void deleteByPageId(int pageId = -1);
    //state = -1时表示不限状态
    PageEntryList getByState(int state = -1);
    int getCountByState(int state = -1);
    MyUtil::IntSeq getPageIdsByState(int state = -1);
    PageEntryList getPagesInIds(const MyUtil::IntSeq& pageIds);
    MyUtil::IntSeq getPageIdsInIds(const MyUtil::IntSeq& pageIds);
  private:
    vector<PageEntryPtr> pageEntries_;
};

typedef IceUtil::Handle<PageEntrySetI> PageEntrySetIPtr;

class PageHelper {
public:
  static string getTableName(int userId) {
    return TABLE_PREFIX + boost::lexical_cast<string>(userId % 100);
  }
  static bool isEqual(const PageEntryPtr& ptr1, const PageEntryPtr& ptr2) {
    return ptr1->id == ptr2->id && ptr1->pageId == ptr2->pageId && ptr1->userId == ptr2->userId && ptr1->addTime == ptr2->addTime
      && ptr1->operatorId == ptr2->operatorId && ptr1->state == ptr2->state;
  }
  static void assignPageEntry(const PageEntryPtr& ptr1, const PageEntryPtr& ptr2) { 
    ptr1->id = ptr2->id; 
    ptr1->pageId = ptr2->pageId; 
    ptr1->userId = ptr2->userId; 
    ptr1->addTime = ptr2->addTime; 
    ptr1->operatorId = ptr2->operatorId; 
    ptr1->state = ptr2->state; 
  }

};

class BatchPageEntryResultHandlerI : public com::xiaonei::xce::ResultHandler {
public:
  BatchPageEntryResultHandlerI(int& maxId, int& maxPageId, PageEntryList& list, const string& col = "id") : col_(col), maxId_(maxId), 
                              maxPageId_(maxPageId), list_(list) {
  }

  virtual bool handle(mysqlpp::Row& row) const {
    int userId = (int)row["user_id"];
    int id = (int)row["id"];
    int temp_id = (int)row[col_.c_str()];
    if(maxId_ < temp_id) {
      maxId_ = temp_id; 
    }
    int state = (int)row["state"];
    int addTime = (int)row["UNIX_TIMESTAMP(add_time)"];
    int oper = (int)row["operator"];
    int pageId = (int)row["page_id"];
    if(maxPageId_ < pageId) {
      maxPageId_ = pageId;
    }
    PageEntryPtr ptr = new PageEntry(id, pageId, userId, state, addTime, oper);
    list_.push_back(ptr);
    return true;
  }

private:
  string col_;
  int& maxId_;
  int& maxPageId_;
  PageEntryList& list_; 
};

PageEntrySetI::PageEntrySetI(PageEntryList list, bool sorted) {
  IceUtil::Mutex::Lock lock(*this);
  //pageEntries_.insert(pageEntries_.end(), list.begin(), list.end());
  //copy(list.begin(), list.end(), back_inserter(pageEntries_));
  pageEntries_.swap(list);
  if(!sorted) {
    sort(pageEntries_.begin(), pageEntries_.end(), CompPageEntry());
  }
}

PageEntrySetI::PageEntrySetI() {
}

void PageEntrySetI::updateState(int pageId, int state) {
  PageEntryPtr ptr = new PageEntry(-1, pageId, -1, -1, -1, -1);
  IceUtil::Mutex::Lock lock(*this);
  vector<PageEntryPtr>::iterator it = lower_bound(pageEntries_.begin(), pageEntries_.end(), ptr, CompPageEntry());
  if(it != pageEntries_.end() && (*it)->pageId == pageId) {
    (*it)->state = state;
  }
}

void PageEntrySetI::updatePageEntry(const PageEntryPtr& page) {
  IceUtil::Mutex::Lock lock(*this);
  vector<PageEntryPtr>::iterator it = lower_bound(pageEntries_.begin(), pageEntries_.end(), page, CompPageEntry());
  if(it == pageEntries_.end() || (*it)->pageId != page->pageId) {
    pageEntries_.insert(it, page);
  } else {
    PageHelper::assignPageEntry(*it, page);
  }
}

void PageEntrySetI::insertPageEntry(const PageEntryPtr& page) {
  IceUtil::Mutex::Lock lock(*this);
  vector<PageEntryPtr>::iterator it = lower_bound(pageEntries_.begin(), pageEntries_.end(), page, CompPageEntry());
  if(it == pageEntries_.end() || (*it)->pageId != page->pageId) {
    pageEntries_.insert(it, page);
  }
}

void PageEntrySetI::insertPageEntryBatch(const PageEntryList& list) {
  for(PageEntryList::const_iterator it = list.begin(); it != list.end(); ++it) {
    insertPageEntry(*it);
  }
}

// pageId等于-1时表示删掉所有元素
void PageEntrySetI::deleteByPageId(int pageId) {
  if(pageId == -1) {
    IceUtil::Mutex::Lock lock(*this);
    pageEntries_.clear();
  } else {
    PageEntryPtr ptr = new PageEntry(-1, pageId, -1, -1, -1, -1);
    IceUtil::Mutex::Lock lock(*this);
    vector<PageEntryPtr>::iterator it = lower_bound(pageEntries_.begin(), pageEntries_.end(), ptr, CompPageEntry());
    if(it != pageEntries_.end() && (*it)->pageId == pageId) {
      pageEntries_.erase(it);
    }
  }
}

//state = -1时表示不限状态
PageEntryList PageEntrySetI::getByState(int state) {
  PageEntryList list;
  IceUtil::Mutex::Lock lock(*this);
  for(vector<PageEntryPtr>::const_iterator itp = pageEntries_.begin(); itp != pageEntries_.end(); ++itp) {
    if(state == -1 || state == (*itp)->state) {
      list.push_back(*itp); 
    }
  }
  return list;
}

int PageEntrySetI::getCountByState(int state) {
  int count = 0;
  IceUtil::Mutex::Lock lock(*this);
  for(vector<PageEntryPtr>::iterator itp = pageEntries_.begin(); itp != pageEntries_.end(); ++itp) {
    if(state == -1 || state == (*itp)->state) {
      ++count; 
    }
  }
  return count; 
}

MyUtil::IntSeq PageEntrySetI::getPageIdsByState(int state) {
  MyUtil::IntSeq result;
  IceUtil::Mutex::Lock lock(*this);
  for(vector<PageEntryPtr>::iterator itp = pageEntries_.begin(); itp != pageEntries_.end(); ++itp) {
    if(state == -1 || state == (*itp)->state) {
      result.push_back((*itp)->pageId); 
    }
  }
  return result;
}

PageEntryList PageEntrySetI::getPagesInIds(const MyUtil::IntSeq& pageIds) {
  PageEntryList list;
  PageEntryPtr ptr = new PageEntry(-1, -1, -1, -1, -1, -1);
  for(MyUtil::IntSeq::const_iterator it = pageIds.begin(); it != pageIds.end(); ++it) {
    ptr->pageId = *it;
    IceUtil::Mutex::Lock lock(*this);
    vector<PageEntryPtr>::iterator itp = lower_bound(pageEntries_.begin(), pageEntries_.end(), ptr, CompPageEntry());
    if(itp != pageEntries_.end() && (*itp)->pageId == ptr->pageId) {
      list.push_back(*itp);
    }
  }
  return list;
}

MyUtil::IntSeq PageEntrySetI::getPageIdsInIds(const MyUtil::IntSeq& pageIds) {
  MyUtil::IntSeq list;
  PageEntryPtr ptr = new PageEntry(-1, -1, -1, -1, -1, -1);
  for(MyUtil::IntSeq::const_iterator it = pageIds.begin(); it != pageIds.end(); ++it) {
    ptr->pageId = *it;
    IceUtil::Mutex::Lock lock(*this);
    vector<PageEntryPtr>::iterator itp = lower_bound(pageEntries_.begin(), pageEntries_.end(), ptr, CompPageEntry());
    if(itp != pageEntries_.end() && (*itp)->pageId == ptr->pageId) {
      list.push_back((*itp)->pageId);
    }
  }
  return list;
}

}
}
#endif
