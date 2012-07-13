#include "base/threadpool.h"
#include "base/messagequeue.h"
#include "base/logging.h"

#include <boost/thread/tss.hpp>
#include <boost/bind.hpp>

#ifdef POSIX
#include "base/thread.h"
#endif

namespace xce {

class Worker : public MessageQueue {
public:
  Worker() : thread_(0) {}
  ~Worker() {
    // join?
    delete thread_;
  }

  bool Start(int stack_size) {
#ifdef POSIX
    thread_ = new thread(boost::bind(&Worker::Process, this), stack_size);
#else
    thread_ = new boost::thread(boost::bind(&Worker::Process, this));
#endif
    return thread_->joinable();
  }

  void Join() {
    return thread_->join();
  }

private:
#ifdef POSIX
  thread* thread_;
#else
  boost::thread* thread_;
#endif
};

#if 0
boost::thread_specific_ptr<MessageQueue> MQ_;

void WorkProcess() {
  MQ_.reset(new MessageQueue());
  MessageQueue* mq = MQ_.get();
  mq->Process();
}
#endif

ThreadPool::~ThreadPool() {
  for (std::list<Worker*>::iterator it=workers_.begin(),end=workers_.end();
    it!=end;
    ++it) {
    delete *it;
  }
}

bool ThreadPool::Post(Message* m) {
  MessageQueue* w = GetIdle();
  if (w) {
    w->Post(m);
    ++ message_count_;
  }
  return w != 0;
}

void ThreadPool::Stop() {
  LOG_F(INFO) << "";
  for(std::list<Worker*>::iterator it=workers_.begin(),end=workers_.end();
    it!=end;
    ++it)
  {
    (*it)->Close();
  }
}

void ThreadPool::Join() {
  LOG_F(INFO) << "";
  // TODO: 禁止 Create
  std::list<Worker*> threads;
  {
    boost::mutex::scoped_lock alock(mutex_);
    threads = workers_;
  }

  for(std::list<Worker*>::iterator it=threads.begin(),end=threads.end();
    it!=end;
    ++it)
  {
    (*it)->Join();
  }
}

void ThreadPool::set_size(int size) {
  // shrink or inflate

  int current = (int)workers_.size();
  if (current > size) {
    // TODO: shrink
  }
  else if (current < size) {
    for (int i=0; i<size - current; ++i) {
      Create();
    }
  }
}

Worker* ThreadPool::Create() {
  Worker* w = new Worker();
  if (!w)
    return 0;

  if (w->Start(stack_size_)) {
    boost::mutex::scoped_lock alock(mutex_);
    workers_.push_back(w);
    
    return w;
  }
  
  delete w;
  return 0;
}

MessageQueue* ThreadPool::GetIdle() {
  // 策略:
  // 1 找到第一个非 busy 的MessageQueue添加
  // 2 如果还可以创建足够的线程，创建，并赋予之
  // 3 找到一个 busy_ 稍小的直接 Post
  boost::mutex::scoped_lock alock(mutex_);

  size_t worker_count = workers_.size();

  size_t c = 0;
  // 1
  while (c++ < worker_count) {
    if (i_ == workers_.end())
      i_ = workers_.begin();

    Worker* mq = *i_++;
    if (0 == mq->busy()) {
      return mq;
    }
  }

  alock.unlock();

  // 2
  if (worker_count < max_size_) {
    Worker* w = Create();
    LOG(INFO) << "ThreadPool inflate to " << c;
    return w;
  }

  alock.lock();

  // 3 TODO:
  LOG(INFO) << "ThreadPool busy";
  i_ = workers_.begin();
  return *i_;
}

}
