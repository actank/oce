#ifndef _CARD_SEARCHER_ICE
#define _CARD_SEARCHER_ICE

#include <Util.ice>
#include <CardCache.ice>
module xce
{
  module cardsearch
  {
    class FriendCardList {
      xce::cardcache::PartInfoSeq acceptList;
      xce::cardcache::PartInfoSeq receiveList;
      xce::cardcache::PartInfoSeq addList;
      xce::cardcache::PartInfoSeq friendList;
    };
    interface CardSearcher
    {
      MyUtil::Int2IntMap search(int userId, string query, int limit);
      FriendCardList getFriendList(int userid);	
      MyUtil::IntList	getUnExchangeList(int userId);
      xce::cardcache::PartNewInfoSeq getFriendsList(int userId);
    };
    interface CardImportSearcher
    {
      xce::cardcache::ImportValueSeq searchImport(int userId, string query, int limit);
      xce::cardcache::ImportValueNewSeq searchImportNew(int userId, string query, int limit);
    };
  };
};

#endif
