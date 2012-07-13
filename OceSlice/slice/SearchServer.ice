/*
 * XiaoNei search interface definition.
 * Written by bochun.bai@dudu-inc.com 
 * History: 
 *   2007-11-06 13:56 Create
 *
 */

#include <SearchModel.ice>

module com
{
  module xiaonei
  {
    module search2
    {
      module server
      {

        // 保持Index运行的容器
        interface IndexContainer
        {
          void load(model::StrSeq directories);
          string getAddress();
          bool isOK();
          model::StrSeq getDirectory();
        };

        sequence<IndexContainer*> IndexContainerList;
        dictionary<string,IndexContainerList> Group;

        // 分派Index的关键服务 
        interface IndexRegistry
        {
          Group getSearcherGroup(model::GroupType type);
          void blockGroup(string groupname);
          void unblockGroup(string groupname);
        };

        // 供客户端调用的最外层对象
        interface IndexManager
        {
          model::IndexResult searchIndex(model::IndexCondition query, int begin, int limit);
          //caller表示这个调用是从那里来的，有OpenPlatform, SearchWeb, FriendsWeb, RenrenWap等来源,增加这个方法的目的是为了限制每个搜索源每天的搜索总次数和5秒内
          //的搜索次数
          model::IndexResult searchIndexWithRestriction(model::IndexCondition query, int begin, int limit, string caller);
        };
        interface IndexShareManager 
        {
          model::IndexShareResult searchShareIndex(model::IndexCondition query, int begin, int limit);
        };
      };
    };
  };
};


