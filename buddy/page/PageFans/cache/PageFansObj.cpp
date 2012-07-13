#include "PageFansObj.h"
#include <ServiceI.h>
#include "../data/PageFansCommon.h"

using namespace xce::buddy;
using namespace MyUtil;

PageFansObjI::PageFansObjI(PageFansObjPtr ptr)
{
   this->realfans = ptr->realfans;
   this->otherfans = ptr->otherfans;
   this->blockfans = ptr->blockfans;

}
//select * from page_fans where page_id={pageId} 
PageEntryPtr PageFansObjI::getFan(int userId)
{
		MCE_DEBUG("PageFansObjI::getFan: userId" << userId );
		IceUtil::Mutex::Lock lock(*this);
		PageEntryList::iterator it;

		PageEntryPtr ptr = new PageEntry();
		ptr->userId=userId;

		it = lower_bound(realfans.begin(),realfans.end(), ptr , less_entry() );
		if (it!=realfans.end() &&   (*it)->userId == ptr->userId) {
				return *it;
		}   

		it = lower_bound(otherfans.begin(),otherfans.end(), ptr , less_entry() );
		if (it!=otherfans.end() &&  (*it)->userId == ptr->userId ) {
				return *it;
		}
		return NULL;
}

int PageFansObjI::addFan(const PageEntryPtr& pe)
{
	  MCE_INFO("PageFansObjI::addFan PageId:userId" << pe->pageId << ":" << pe->userId);
	  IceUtil::Mutex::Lock lock(*this);
	  PageEntryList::iterator it;
	  if(pe->state == NORMAL)
	  {
			  it = lower_bound(realfans.begin(),realfans.end() ,pe  , less_entry() );
			  if (it!=realfans.end() && (*it)->userId == pe->userId) {
					  MCE_INFO("PageFansObjI::addFan pageId:userId " <<  pe->pageId << ":" << pe->userId << "is exist" );
					  (*it) = pe; 
			  }else{
					realfans.insert(it,pe);			  
			  }
		
	  }
	  else{
			  it = lower_bound(otherfans.begin(),otherfans.end() , pe ,less_entry() );
			  if (it!=otherfans.end() && (*it)->userId == pe->userId) {
					  MCE_INFO("PageFansObjI::addFan pageId:userId " <<  pe->pageId << ":" << pe->userId << "is exist" );
					  (*it) = pe; 
			  }else{
					  otherfans.insert(it,pe);
			  }

	  }
	  return 1;
}

int PageFansObjI::addFanNoLock(const PageEntryPtr& pe)
{
	  MCE_INFO("PageFansObjI::addFanNoLock PageId:userId" << pe->pageId << ":" << pe->userId);
  
	  PageEntryList::iterator it;

	  if(pe->state == NORMAL)
	  {
			  it = lower_bound(realfans.begin(),realfans.end() ,pe  , less_entry() );
			  if (it!=realfans.end() && (*it)->userId == pe->userId) {
					  MCE_INFO("PageFansObjI::addFan pageId:userId " <<  pe->pageId << ":" << pe->userId << "is exist" );
					  (*it) = pe; 
			  }else{
					realfans.insert(it,pe);			  
			  }
		
	  }
	  
	  else{
			  it = lower_bound(otherfans.begin(),otherfans.end() , pe ,less_entry() );
			  if (it!=otherfans.end() && (*it)->userId == pe->userId) {
					  MCE_INFO("PageFansObjI::addFan pageId:userId " <<  pe->pageId << ":" << pe->userId << "is exist" );
					  (*it) = pe; 
			  }else{
					  otherfans.insert(it,pe);
			  }

	  }
	  return 1;
}




int PageFansObjI::removeFan(int userId)
{
	  MCE_INFO("PageFansObjI::removeFan PageId:userId" << userId);
	  IceUtil::Mutex::Lock lock(*this);
  	  PageEntryPtr ptr = new PageEntry();
	  ptr->userId=userId;

	  PageEntryList::iterator it;
	  it = lower_bound(realfans.begin(),realfans.end() , ptr , less_entry() );
	  if (it!=realfans.end() && (*it)->userId == userId) {
			  realfans.erase(it);			 
			 return 1; 
	  }

	  it = lower_bound(otherfans.begin(),otherfans.end(), ptr , less_entry() );
	  if (it!=otherfans.end() && (*it)->userId == userId) {
			  otherfans.erase(it);
			  return 1;
	  }
	    
	  return 0;
}
int PageFansObjI::removeFanNoLock(int userId)
{
	  MCE_INFO("PageFansObjI::removeFanNoLock PageId:userId" << userId);
  	  PageEntryPtr ptr = new PageEntry();
	  ptr->userId=userId;

	  PageEntryList::iterator it;
	  it = lower_bound(realfans.begin(),realfans.end() , ptr , less_entry() );
	  if (it!=realfans.end() && (*it)->userId == userId) {
			  realfans.erase(it);			 
			 return 1; 
	  }

	  it = lower_bound(otherfans.begin(),otherfans.end(), ptr , less_entry() );
	  if (it!=otherfans.end() && (*it)->userId == userId) {
			  otherfans.erase(it);
			  return 1;
	  }
	    
	  return 0;
}


int PageFansObjI::updateFan(const PageEntryPtr& pe)
{
	  MCE_INFO("PageFansObjI::updateFan PageId:userId" << pe->pageId << ":" << pe->userId);
      if(1==removeFan(pe->userId))
		  addFan(pe); 
	  return 1;
}

int PageFansObjI::updateAllState(int oriState , int tarState)
{
	  MCE_INFO("PageFansObjI::updateAllState oriState" << oriState << " :tarState  " << tarState);
	  IceUtil::Mutex::Lock lock(*this);
      PageEntryList list;
	  if(oriState == NORMAL){
	  	 list = realfans;
	  }
	  else{
	    list = otherfans;
	  }
	  
      //there user loop to change state for every data
	  for(PageEntryList::iterator it = list.begin();it!=list.end();it++)
	  {
	  MCE_INFO("PageFans cao ni m le ge bi " << (*it)->state );
			 if( (*it)->state == oriState )
			 {
					 (*it)->state = tarState ;	
					 removeFanNoLock((*it)->userId);
	  MCE_INFO("PageFansObjI::updateAllS cao ni m le ge bi " << (*it)->state );
					 addFanNoLock(*it) ;	
			 }
	  }		 

	  return 1;
}

bool PageFansObjI::isFan(int userId)
{
		MCE_DEBUG("PageFansObjI::isFan: userId" << userId );
		IceUtil::Mutex::Lock lock(*this);
		PageEntryList::iterator it;

		PageEntryPtr ptr = new PageEntry();
		ptr->userId=userId;

		it = lower_bound(realfans.begin(),realfans.end() , ptr , less_entry() );
		if (it!=realfans.end() && (*it)->userId == userId ) {
				return true;
		}   

		return false;
}

PageEntryList PageFansObjI::getRealFans()
{
		MCE_DEBUG("PageFansObjI::getRealFans "  );
		IceUtil::Mutex::Lock lock(*this);
		return realfans;
}

PageEntryList PageFansObjI::getOtherFans()
{
		MCE_DEBUG("PageFansObjI::getOtherFans "  );
		IceUtil::Mutex::Lock lock(*this);
		return otherfans;
}

PageEntryList PageFansObjI::getBlockFans()
{
		MCE_DEBUG("PageFansObjI::getBlockFans "  );
		IceUtil::Mutex::Lock lock(*this);
		return blockfans;
}


int PageFansObjI::addBlock(const PageEntryPtr& pe)
{
	  MCE_INFO("PageFansObjI::addBlock PageId:userId" << pe->pageId << ":" << pe->userId);
	  IceUtil::Mutex::Lock lock(*this);
  
	  PageEntryList::iterator it;

	  it = lower_bound(blockfans.begin(),blockfans.end() , pe , less_entry() );
	  if (it!=blockfans.end() && (*it)->userId == pe->userId) {
			  MCE_INFO("PageFansObjI::addBlock pageId:userId " <<  pe->pageId << ":" << pe->userId << "is exist" );
			  (*it) = pe; 
	  }else{
			  blockfans.insert(it,pe);			  
	  }
	  return 1;
}
int PageFansObjI::findBlock(int userId)
{
	  MCE_INFO("PageFansObjI::findBlock userId" << userId);
	  IceUtil::Mutex::Lock lock(*this);
	  PageEntryPtr ptr = new PageEntry();
	  ptr->userId=userId;

	  PageEntryList::iterator it;
	  it = lower_bound(blockfans.begin(),blockfans.end(), ptr , less_entry() );
	  if (it!=blockfans.end() && (*it)->userId == userId) {
			  return 1;
	  }
    
	  return 0;
}
int PageFansObjI::removeBlock(int userId)
{
	  MCE_INFO("PageFansObjI::remove userId" << userId);
	  IceUtil::Mutex::Lock lock(*this);
  
	  PageEntryList::iterator it;
  	  PageEntryPtr ptr = new PageEntry();
	  ptr->userId=userId;


	  it = lower_bound(blockfans.begin(),blockfans.end(), ptr , less_entry() );
	  if (it!=blockfans.end() && (*it)->userId == userId) {
			  blockfans.erase(it);			  
			  return 1;
	  }
    
	  return 0;
}
