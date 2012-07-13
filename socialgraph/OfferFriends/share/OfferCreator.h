#ifndef __OFFERFRIENDS_CREATOR_H__
#define __OFFERFRIENDS_CREATOR_H__

#include <Ice/Ice.h>
#include <IceUtil/IceUtil.h>
#include "OfferCacheDataI.h"
#include "OfferCacheDataFactory.h"
#include "PreferenceUtil.h"
#include "BackendUtil.h"
#include "OfferFriendsUtil.h"

namespace xce {
namespace offerfriends {

//**************************************************************************************************************

struct less_offer: public std::less<OfferDataN> {
	bool operator()(const OfferDataN& x, const OfferDataN& y) const {
		return x.weight > y.weight;
	}
};

//**************************************************************************************************************

class OfferCreator: public MyUtil::Singleton<OfferCreator> {
	public:
		/*@brief 创建用户的OfferFriends推荐数据
  	* @param userId 用户Id
  	* @param resultSize 推荐数量
  	* @param createRes 输出log
  	*/
		OfferCacheDataNPtr create(int userId,
															int resultSize,
															ostringstream& createRes);

		/*@brief 创建用户的HighSchool推荐数据
  	* @param userId 用户Id
  	* @param resultSize 推荐数量
  	* @param createRes 输出log
  	*/
		OfferCacheDataNPtr createHighSchoolData(int userId,
															int resultSize,
															ostringstream& createRes);
    friend class MyUtil::Singleton<OfferCreator>;

  private:
		/*@brief 排序用户offerfriends推荐数据
  	* @param typeWeightMap 
  	* @param bak 用户backend数据
  	* @param createLog 简单输出log
  	* @param createDetail 具体输出log
  	*/
		MyUtil::IntSeq dedup(std::map<int, std::multimap<int, OfferDataN> >& typedWeightMap, 
												 const BackendDataPtr& bak, 
												 ostringstream& createLog, 
												 ostringstream& createDetail);

		/*@brief 过滤用户头像
  	* @param typeWeightMap 
  	* @param photo_map 用户是否有头像数据
  	* @param createLog 简单输出log
  	* @param createDetail 具体输出log
  	*/
		void photo_filter(map<int, multimap<int, OfferDataN> >& typedWeightMap, 
											const map<int, bool>& photo_map, 
											ostringstream& createLog, 
											ostringstream& createDetail);

		/*@brief 用户是否被封禁
  	* @param typeWeightMap 
  	* @param status_map 用户是否被封禁
  	* @param createLog 简单输出log
  	* @param createDetail 具体输出log
  	*/
		void status_filter(map<int, multimap<int, OfferDataN> >& typedWeightMap, 
											 const map<int, bool>& status_map, 
											 ostringstream& createLog, 
											 ostringstream& createDetail);

		/*@brief 用户人气排序
  	* @param typeWeightMap 
  	* @param view_count_map 用户人气数据
  	* @param createLog 简单输出log
  	* @param createDetail 具体输出log
  	*/
		void view_count_filter(map<int, multimap<int, OfferDataN> >& typedWeightMap, 
													 const map<int, int>& view_count_map, 
													 ostringstream& createLog, 
													 ostringstream& createDetail);

		/*@brief 用户星级排序
  	* @param typeWeightMap 
  	* @param star_map用户星级数据
  	* @param createLog 简单输出log
  	* @param createDetail 具体输出log
  	*/
		void star_filter(map<int, multimap<int, OfferDataN> >& typedWeightMap, 
										 const map<int,bool>& star_map, 
										 ostringstream& createLog, 
										 ostringstream& createDetail);

		OfferCacheDataNPtr pick_out(int, 
															  map<int, multimap<int, OfferDataN> >&, 
																PreferenceDataPtr&, 
																ostringstream&, 
																ostringstream&);
};

//**************************************************************************************************************

}
}

#endif
