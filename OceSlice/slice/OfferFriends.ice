#include <ObjectCache.ice>
#include <Ice/BuiltinSequences.ice>

module xce {
module offerfriends {

	struct OfferDataN{
		int userId;
		int weight;
		int sign;
	};
	sequence<OfferDataN> OfferDataNSeq;
	dictionary<int,OfferDataNSeq> OfferDataNMap;

	class OfferCacheDataN{
		OfferDataNSeq bestOffers;
		OfferDataNSeq offers;
	};

	struct GetInfo{
		int userId;
		bool hit;
		int limit;
		int bestRes;
		int bestCount;
		int onlineRes;
		int offerRes;
		int offerCount;
		long timeout;
	};

	struct RmInfo{
		int userId;
		bool hit;
		bool found;
		OfferDataN data;
		int bestCount;
		int offerCount;
		long timeout;
	};

	const int BaseTypeMix = 0;
	const int BaseTypeCommonFriend = 10;
	const int BaseTypeSameInfoWorkPlace = 21;
	const int BaseTypeSameInfoUniv = 22;
	const int BaseTypeSameInfoHighSchool = 23;
	const int BaseTypeSameInfoJuniorSchool = 24;
	const int BaseTypeSameInfoElementarySchool = 25;
	const int BaseTypeIPSame = 30;
	const int BaseTypeAreaEnrollSame = 40;

	const int BaseTraitCommonFriend = 0;
	const int BaseTraitSameInfoWorkPlace = 1;
	const int BaseTraitSameInfoUniv = 2;
	const int BaseTraitSameInfoHighSchool = 3;
	const int BaseTraitSameInfoJuniorSchool = 4;
	const int BaseTraitSameInfoElementarySchool = 5;
	const int BaseTraitIPSame = 6;
	const int BaseTraitAreaEnrollSame = 7;

	const int HIGHSCHOOL = 10;
	const int UNIV = 20;
	const int WORK = 30;
	const int OTHER = 90;

	interface OfferFriendsCacheNManager {

		OfferDataNSeq getFastOffer(int userId, int limit);
		OfferDataNSeq getBestOffer(int userId, int limit);
		OfferDataNSeq getRandomOffer(int userId, int limit);

		OfferDataNSeq getBestOfferByStage(int stage, int userId, int limit);

		RmInfo apply(int applicant, int acceptor);
		RmInfo accept(int acceptor, int applicant);
		RmInfo denyOffer(int hostId, int guestId);
		RmInfo ignoreOffer(int hostId, int guestId);

		void load(int userId,Object o);
		bool isValid();
		void setValid(bool valid);
		void setData(MyUtil::ObjectResult datas);
	};

	interface OfferFriendsLoaderN{

		void apply(int applicant, int acceptor);
		void accept(int acceptor, int applicant);
		void denyOffer(int hostId, int guestId);
		void ignoreOffers(int userId, MyUtil::IntSeq ids);

		void report(GetInfo info);
		void rebuild(int userId,bool now);
		void rebuildBatch(MyUtil::IntSeq userIds,bool now);

		string debug(int userId);

		void reload(int userId);
		void reloadBatch(MyUtil::IntSeq userIds);
	};

};
};
