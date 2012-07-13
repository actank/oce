/*
 * =====================================================================================
 *
 *       Filename:  FansPageReloaderAdapter.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  11/17/2010 03:37:22 PM
 *       Revision:  none
 *       Compiler:  g++
 *
 *         Author:  min.shang (Jesse Shang), min.shang@opi-corp.com
 *        Company:  Opi-Corp
 *
 * =====================================================================================
 */

#ifndef __FANSPAGE_RELOADER_ADAPTER_H__
#define __FANSPAGE_RELOADER_ADAPTER_H__

#include <IceUtil/RWRecMutex.h>
#include <FansPage.h>
#include <AdapterI.h>
#include <TaskManager.h>

namespace xce {
namespace buddy {
namespace adapter {

class FansPageReloaderAdapter : public MyUtil::ReplicatedClusterAdapterI<FansPageReloaderPrx>, public MyUtil::Singleton<FansPageReloaderAdapter> {
public:
  FansPageReloaderAdapter();
  void updateState(int pageId, int userId, short state);
  void insert(PageEntryPtr page);
  //插入一批PageEntry
  void insertBatch(PageEntryList pageList);
  void deleteByUidPid(int pageId, int userId);
  //删除用户userId的所有PageEntry
  void deleteByUid(int userId);
  //更新userId关注的所有PageEntry
  void loadFansPageByUid(int userId);
  //更新userId,pageId对应的PageEntry
  void loadFansPageByUidPid(int pageId, int userId);
  //更新所有的PageEntry
  void loadAllFansPage();
  //更新userId和state对应的所有PageEntry
  void loadFansPageByUidState(int userId, short state);
  bool isValid();
  void setValid(bool valid);
};

}
}
}
#endif
