#ifndef __PAGEFANSOBJ_H__
#define __PAGEFANSOBJ_H__

#include <PageCommon.h>
#include <Ice/Ice.h>

namespace xce{
namespace buddy{
using namespace MyUtil;
using namespace xce::buddy;
class PageFansObjI : virtual public PageFansObj, virtual public IceUtil::Mutex{
public:
	  PageFansObjI(){};
	  PageFansObjI(PageFansObjPtr ptr);
  	  PageEntryPtr getFan(int userId);
      int addFan(const PageEntryPtr& pe);
      int addFanNoLock(const PageEntryPtr& pe);
	  bool isFan(int userId);
	  int removeFan(int userId);
	  int removeFanNoLock(int userId);
	  int updateFan(const PageEntryPtr& pe);
	  int updateAllState(int oriState , int tarState);
	  int findBlock(int userId);
	  int removeBlock(int userId);
      int addBlock(const PageEntryPtr& pe);

	  PageEntryList getRealFans();
	  PageEntryList getOtherFans();
	  PageEntryList getBlockFans();

private:
};
typedef IceUtil::Handle<PageFansObjI> PageFansObjIPtr;


struct less_entry : public std::binary_function<PageEntryPtr, PageEntryPtr, bool> {
		bool operator()(const PageEntryPtr& x, const PageEntryPtr& y) const {
				return x->userId < y->userId;
		}   
};

struct equal_entry : public std::binary_function<PageEntryPtr, PageEntryPtr, bool> {
		bool operator()(const PageEntryPtr& x, const PageEntryPtr& y) const {
				return x->userId == y->userId;
		}   
};


}
}


#endif
