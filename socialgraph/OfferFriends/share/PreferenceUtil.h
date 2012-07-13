#ifndef __PREFERENCE_UTIL_H__
#define __PREFERENCE_UTIL_H__

#include <Ice/Ice.h>
#include <IceUtil/IceUtil.h>
#include <OfferFriends.h>
#include <Singleton.h>

namespace xce {
namespace offerfriends {

//**************************************************************************************************************

/*@brief 用户的Preference数据*/
class PreferenceData: virtual public Ice::Object, virtual public IceUtil::Mutex {
public:
	PreferenceData();

	/*@brief 初始化用户的preference数据,初始化成员变量_weights, _quotas
  * @param weight 不同类型对应的权重
  * @param quotas 不同类型对应的书展示数量
  */
	std::string set(const std::map<int, int>& weights, 
									const std::map<int, int>& quotas);

	void set(int type, int weight, int quota);
	std::map<int, int> getWeights();
	std::multimap<int, int> getQuotas(int total);
	std::map<int, int> getQuotasFrom(int total, const std::map<int, std::multimap<int, OfferDataN> >& typedWeightMap);
	std::string toString();
private:
	std::map<int, int> _weights;
	std::map<int, int> _quotas;
};
typedef IceUtil::Handle<PreferenceData> PreferenceDataPtr;

//**************************************************************************************************************

/*@brief 用户的preference数据生产者 */
class PreferenceProducer: public MyUtil::Singleton<PreferenceProducer>{
public:
  friend class MyUtil::Singleton<PreferenceProducer>;

  PreferenceDataPtr create(int, std::ostringstream&);
};

//**************************************************************************************************************

}
}

#endif
