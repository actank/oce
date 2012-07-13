#ifndef _SOCIALGRAPH_RELATION_SERVICE_H_
#define _SOCIALGRAPH_RELATION_SERVICE_H_

#include <Ice/Ice.h>
#include <IceUtil/IceUtil.h>
#include "ServiceI.h"
#include "QueryRunner.h"
#include "SocialGraphRelation.h"
#include "emotion_state.h"

namespace xce {
namespace socialgraph {

const static int kEmotionStateCacheData = 1;

struct EmotionStateCacheData : public Ice::Object {
  IceUtil::Mutex mutex_;
  EmotionState emotion_state_;
  virtual ~EmotionStateCacheData() {}
};

typedef IceUtil::Handle<EmotionStateCacheData> EmotionStateCacheDataPtr;

class EmotionStateCacheDataFactory : virtual public MyUtil::ServantFactory {
 public:
  Ice::ObjectPtr create(Ice::Int user_id) {
    EmotionStateCacheDataPtr ptr = new EmotionStateCacheData();
    return ptr;
  }
};

class SgRelationServiceImpl : virtual public SgRelationService, virtual public MyUtil::Singleton<SgRelationServiceImpl> {
 public:
  virtual void reload(Ice::Int, const Ice::Current& = Ice::Current());
  virtual void deleteEmotionStateCache(Ice::Int, const Ice::Current& = Ice::Current());
  virtual EmotionState getEmotionState(Ice::Int, const Ice::Current& = Ice::Current());
  virtual void setEmotionState(Ice::Int, const EmotionState&, const Ice::Current& = Ice::Current());
  ~SgRelationServiceImpl() { }
 private:
  EmotionState Load(int user_id);
  void Delete(int user_id);
  void InitSgRelation(const SgRelationTuple&, EmotionState&);
  AbsEmotionState* EmotionStateStrategy(const EmotionState&);
 private:
  SgRelationServiceImpl();
  friend class MyUtil::Singleton<SgRelationServiceImpl>;
};

}}
#endif

