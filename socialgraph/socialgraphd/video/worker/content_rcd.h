#ifndef _XCE_SOCIALGRAPH_VIDEO_RCD_H_
#define _XCE_SOCIALGRAPH_VIDEO_RCD_H_

#include <Ice/Ice.h>
#include <IceUtil/IceUtil.h>
#include "ServiceI.h"
#include "ContentRcd.h"

#include "base/threadpool.h"
#include "tree.h"

#include "util/cpp/TimeCost.h"

namespace xce {
namespace socialgraph {

class InitVideoTreeMessage : public xce::Message {
 public:
  InitVideoTreeMessage(Tree* tree) {
    tree_ptr_ = tree;
  }

  void Run() {
    MyUtil::TimeCost tc = MyUtil::TimeCost::create("InitVideoTreeMessage::Run()", MyUtil::TimeCost::LOG_LEVEL_INFO);   
    if (tree_ptr_) {
      tree_ptr_->Init();
    } 
  }
 private:
  Tree* tree_ptr_;
};

class ContentRcdWorkerI : virtual public ContentRcdWorker, virtual public MyUtil::Singleton<ContentRcdWorkerI> {
 public:
  virtual ContentRcdSeq GetVideoRcd(int host, const ContentRcd& rcd, const Ice::Current& current = Ice::Current());
  virtual ~ContentRcdWorkerI() {
    if (tree_) {
      delete tree_;
      tree_ = 0;
    }
  }
 private:
  ContentRcdWorkerI();
  friend class MyUtil::Singleton<ContentRcdWorkerI>;
 private:
  Tree* tree_;
  xce::ThreadPool thread_poll_;
};

}}

#endif

