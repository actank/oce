#ifndef __FEED_ICE__
#define __FEED_ICE__

#include <Util.ice>
#include <Ice/BuiltinSequences.ice>

module mop
{
	module hi
	{
		module svc
		{
			module feed
			{
				const string CTypeAll = "";

				class FeedPublish
				{
					int feedId;
					int fromId;
					string fromName;
					int ownerId;
					string ownerName;
					int resource;
					string type;
					string title;
					Ice::ByteSeq content;
					int timestamp;
				};
				sequence<FeedPublish> FeedPublishSeq;

				class FromBody
				{
					int id;
					string name;
				};
				sequence<FromBody> FromBodySeq;

				class FeedQuery
				{
					int feedId;
					FromBodySeq fromBodies;
					int ownerId;
					string ownerName;
					int resource;
					string type;
					string title;
					Ice::ByteSeq content;
					int timestamp;
					bool unknown;
				};
				sequence<FeedQuery> FeedQuerySeq;
				
				class FeedQueryResultSet
				{
					int total;
					int unknowns;
					FeedQuerySeq data;
				};

				class FeedBody
				{
					int fromId;
					string fromName;
					int ownerId;
					string ownerName;
					string title;
					Ice::ByteSeq content;
				};
				
				class FeedDispatcher
				{
					void dispatch(FeedPublish data);
					void dispatchAgain(int fromId, int toId);
					
					void deliver(FeedPublish data, MyUtil::IntSeq acceptors);
					void deliverSupply(int supplier, int acceptor);					
				};
				
				class FeedMiniManager
				{
					int getFeedCount(int userId, string type, bool unknown);
					FeedQueryResultSet getFeedSeq(int userId, string type, bool unknown, int begin, int limit);
					void setFeedKnown(int userId, int feedId);
					void setAllFeedKnown(int userId);

					void publishFeed(FeedPublish feed);
				};

				class FeedNewsManager
				{
					int getFeedCount(int userId, string type, bool unknown);
					FeedQueryResultSet getFeedSeq(int userId, string type, bool unknown, int begin, int limit);
					void setFeedKnown(int userId, int noteId);
					void setAllFeedKnown(int userId);

					void deliverFeed(FeedPublish feed, MyUtil::Int2IntMap targets, int dedup);
				};
				
			};
		};
	};
};
#endif
