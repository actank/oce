/*
 *  SearchLogicAdapter.h
 *  
 *   Created on: 2010-9-27
 *       Author: lijie
 **/

#ifndef SEARCHRELATIONLOGICADAPTER_H_
#define SEARCHRELATIONLOGICADAPTER_H_

#include "Singleton.h"
#include "AdapterI.h"

#include <SearchRelationLogic.h>

using namespace com::xiaonei::search5::searchrelation;

class SearchRelationLogicAdapter : public MyUtil::Singleton<SearchRelationLogicAdapter>, public MyUtil::ReplicatedClusterAdapterI<SearchRelationLogicPrx>
{

  public:
    SearchRelationLogicAdapter();

    virtual ~SearchRelationLogicAdapter();

    bool pushFriendShip(int fromUserId, int toUserId, Operation op);

};
#endif
