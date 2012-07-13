#ifndef __AI_GUIDE_CACHE_I_H__
#define __AI_GUIDE_CACHE_I_H__

#include "AIGuideCache.h"
#include "ServiceI.h"
#include <bitset>

namespace xce
{

namespace aiguidecache
{
	
/* @brief 32位组合的一个int
 */
class Bits32 : virtual public Ice::Object
//	virtual public IceUtil::Mutex {
{
public:
	Bits32() {
	};

	/* @breif	设置其中某一位
	 * @param	pos	该位在int中的位置(二进制)
	 * @param	value	该位要设置的值(1,0)
	 */
	void set(int pos, bool value);

	/* @brief 	取其中的某一位
	 * @param 	pos 	该位在int中的位置(二进制)
	 * @return	该位的结果
	 */
	int get(int pos);	

private:
	bitset<32> _data;
};

typedef IceUtil::Handle<Bits32> Bits32Ptr;

class BitsArray {
public:
	/* @brief	构造函数
	 * @param	length		bit数组长度
	 */
	BitsArray(int length=2147483647):
			// _data(length/32+1, Bits32()),
			_data(length/32+1),
			_length(length)		
	{
	}
	
	/* @brief	设置bit数组某一个成员
	 * @param	pos	在bit数组中的位置
	 * @param	value	设置值(0, 1)
	 */
	void set(int pos, bool value);

	/* @brief	取得bit数组的某一个成员
	 * @param	pos	在bit数组中的位置
	 * @return	该成员(0,1)
	 */
	int get(int pos);
private:
	std::vector<Bits32> _data;
	int _length;
		
};
	
class AIGuideCacheManagerI : virtual public AIGuideCacheManager,
			     virtual public MyUtil::Singleton<AIGuideCacheManagerI>

{
public:

    virtual void set(::Ice::Int userId, ::Ice::Int appId, bool status, const Ice::Current& crt=Ice::Current());

    virtual bool get(::Ice::Int userId, ::Ice::Int appId, const Ice::Current& crt=Ice::Current());
    
private:

    friend class MyUtil::Singleton<AIGuideCacheManagerI>;

    AIGuideCacheManagerI():
	    _arrayFarm(),
	    _arrayDog(),
	    _arrayCarpark(),
	    _arrayGod(),
	    _arrayTycoon()     {
    }

    AIGuideCacheManagerI(const AIGuideCacheManagerI& ) {
    }
   
    BitsArray _arrayFarm;  
    BitsArray _arrayDog;
    BitsArray _arrayCarpark;
    BitsArray _arrayGod;
    BitsArray _arrayTycoon;
    
};

}

}

#endif
