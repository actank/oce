/*
 * XiaoNei search interface definition.
 * Written by bochun.bai@dudu-inc.com 
 * History: 
 *   2007-11-06 13:56 Create
 *
 */

#include <SearchModel.ice>
#include <ShareSearch.ice>

module com
{
  module xiaonei
  {
    module sharesearch
    {
      module server
      {

        // 保持Index运行的容器
        interface ShareIndexContainer
        {
          void load(search2::model::StrSeq directories);
          string getAddress();
          bool isOK();
          search2::model::StrSeq getDirectory();
          bool update(xce::sharesearch::ShareIndexInfoSeq shareInfos);
          void setCurrentState(int state);
          int getCurrentState();
          string getIsUsingDir();
        };

        sequence<ShareIndexContainer*> ShareIndexContainerList;
        dictionary<string,ShareIndexContainerList> Group;

        // 分派Index的关键服务 
        interface ShareIndexRegistry
        {
          Group getSearcherGroup(search2::model::GroupType type);
          void blockGroup(string groupname);
          void unblockGroup(string groupname);
        };

        // 供客户端调用的最外层对象
        interface ShareIndexManager 
        {
          search2::model::IndexShareResult searchShareIndex(search2::model::IndexCondition query, int begin, int limit);
        };
      };
    };
  };
};


