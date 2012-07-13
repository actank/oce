
#include "KeepUseI.h"
using namespace xce::keepuse;

void MyUtil::initialize()
{
	MCE_INFO("[KeepUse] starting");

    // TaskManager::instance().config(TASK_LEVEL_FILL, MyUtil::ThreadPoolConfig(1, 100));

	ServiceI& service = ServiceI::instance();
	int max_days = service.getCommunicator()->getProperties()->getPropertyAsIntWithDefault("KeepUse.MaxDays", 7);

	KeepUseManagerI* manager = new KeepUseManagerI;
	manager->initialize(max_days);

	service.getAdapter()->add(manager, service.createIdentity("M", ""));
	MCE_INFO("[KeepUse] inited.");
}

void xce::keepuse::KeepUseManagerI::initialize(int size){	
	MCE_INFO("[KeepUse] manager initializing.");
	datas_.resize(size);
	for(int pos = 0; pos < size; ++pos){
		datas_[pos] = new UserBits;
	}
	current_pos_ = 0;
	pos_max_ = size;
	MCE_INFO("[KeepUse] manager initialized.");
};

xce::keepuse::KeepUseManagerI::~KeepUseManagerI(){
	for(int pos = 0; pos < pos_max_; ++pos){
		delete datas_[pos];
	}
	datas_.clear();
}

void
xce::keepuse::KeepUseManagerI::todayuse(::Ice::Int id,
                                        const Ice::Current& current)
{
	IceUtil::RWRecMutex::RLock lock(mutex_);
	if(id<0) return;
	datas_[current_pos_]->set(id);
}

void
xce::keepuse::KeepUseManagerI::use(::Ice::Int days, const ::xce::keepuse::Int2BoolMap& used,
                                               const Ice::Current&)
{
	IceUtil::RWRecMutex::RLock lock(mutex_);
	int pos = (pos_max_ - days + current_pos_) % pos_max_;
	for(Int2BoolMap::const_iterator it = used.begin(); it != used.end(); ++it){
		datas_[pos]->set(it->first, it->second);
	}
}

::xce::keepuse::Int2BoolMap
xce::keepuse::KeepUseManagerI::isused(::Ice::Int days, ::Ice::Int id,
                                      const Ice::Current& current)
{
	if(id<0 || days > pos_max_) return Int2BoolMap();
    ::xce::keepuse::Int2BoolMap result;
	IceUtil::RWRecMutex::RLock lock(mutex_);
	for(int pos = 0;pos < pos_max_; ++pos){
		int date = (pos + pos_max_ + pos_max_ - days - current_pos_) % pos_max_;
		result[pos] = datas_[date]->test(id);
	}
	return result;
}

bool
xce::keepuse::KeepUseManagerI::newday(const ::std::string& date,
                                      const Ice::Current& current)
{
	IceUtil::RWRecMutex::WLock lock(mutex_);
	if(current_day_==date){
		return false;
	}
	current_pos_++;
	current_pos_ = current_pos_==pos_max_ ? 0 : current_pos_;
	datas_[current_pos_]->reset();
	if(++switch_count_ >= pos_max_) can_unset_ = true;
    return true;
}

bool
xce::keepuse::KeepUseManagerI::canUnset(const Ice::Current&){
	IceUtil::RWRecMutex::RLock lock(mutex_);
	return can_unset_;
}

void
xce::keepuse::KeepUseManagerI::setCanUnset(bool flag, const Ice::Current&){
	IceUtil::RWRecMutex::WLock lock(mutex_);
	can_unset_ = flag;
}


void
SetStateTask::run(){
}

void
UnsetStateTask::run(){
}

