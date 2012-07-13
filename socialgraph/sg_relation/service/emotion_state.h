#ifndef _SG_RELATION_EMOTION_STATE_H_
#define _SG_RELATION_EMOTION_STATE_H_
#include "SocialGraphRelation.h"

namespace xce {
namespace socialgraph {

struct AbsEmotionState {
  virtual void SetOn() = 0;
  virtual void SetOff() = 0;
  virtual ~AbsEmotionState() {}
};

class EmptyState : public AbsEmotionState {
 public:
  EmptyState(const EmotionState& es) : stat_(es) {}
  void SetOn();
  void SetOff();
 private:
  EmotionState stat_;
};

class SingleState : public AbsEmotionState {
 public:
  SingleState(const EmotionState& es) : stat_(es) { }
  void SetOn();
  void SetOff();
 private:
  EmotionState stat_;
};

class DivorcedState : public AbsEmotionState {
 public:
  DivorcedState(const EmotionState& es) : stat_(es) { }
  void SetOn();
  void SetOff();
 private:
  EmotionState stat_;
};
class CourtState : public AbsEmotionState {
 public:
  CourtState(const EmotionState& es) : stat_(es) { }
  void SetOn();
  void SetOff();
 private:
  EmotionState stat_;
};
class LikeState : public AbsEmotionState {
 public:
  LikeState(const EmotionState& es) : stat_(es) { }
  void SetOn();
  void SetOff();
 private:
  EmotionState stat_;
};
class InloveState : public AbsEmotionState {
 public:
  InloveState(const EmotionState& es) : stat_(es) { }
  void SetOn();
  void SetOff();
 private:
  EmotionState stat_;
};
class InloveColdwarState : public AbsEmotionState {
 public:
  InloveColdwarState(const EmotionState& es) : stat_(es) { }
  void SetOn();
  void SetOff();
 private:
  EmotionState stat_;
};
class EngagedState : public AbsEmotionState {
 public:
  EngagedState(const EmotionState& es) : stat_(es) { }
  void SetOn();
  void SetOff();
 private:
  EmotionState stat_;
};
class MarriedState : public AbsEmotionState {
 public:
  MarriedState(const EmotionState& es) : stat_(es) { }
  void SetOn();
  void SetOff();
 private:
  EmotionState stat_;
};
class MarriedHoneymoonState : public AbsEmotionState {
 public:
  MarriedHoneymoonState(const EmotionState& es) : stat_(es) { }
  void SetOn();
  void SetOff();
 private:
  EmotionState stat_;
};
class MarriedColdwarState : public AbsEmotionState {
 public:
  MarriedColdwarState(const EmotionState& es) : stat_(es) { }
  void SetOn();
  void SetOff();
 private:
  EmotionState stat_;
};

}}
#endif


