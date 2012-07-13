#include "base/logging.h"
#include "base/once.h"
#include "base/threadpool.h"

#include "boost/thread/mutex.hpp"
#include "boost/thread/condition.hpp"
#include "boost/thread/thread.hpp"
#include "boost/thread/xtime.hpp"
#include "boost/lexical_cast.hpp"

#include "json/json.h"

static const std::string DjParamID = "ID";
static const std::string DjParamR = "R";
static const std::string DjParamX = "X";
static const std::string DjParamY = "Y";
static const std::string DjParamT = "T";
static const std::string DjParamU = "U";
static const std::string DjParamO = "O";

static const size_t MAX_DATA_SIZE = 5000;
static const int MAX_WAIT_TIME = 1;

struct NotifyData {
	NotifyData( int type, int time, const std::string& param ):
		type_(type),time_(time),param_(param)
	{
	}
        int type_;
        int time_;
        std::string param_;
};

class WorkMessage : virtual public xce::Message {
public:
        WorkMessage( int type, const std::string& param ):
                        type_(type),time_(time(NULL)),param_(param)
	{
        }
        virtual void Run();
private:
        int type_;
        int time_;
        std::string param_;
};

class NotifyMessage : virtual public xce::Message {
public:
        NotifyMessage( const std::vector<boost::shared_ptr<NotifyData> >& datas):
			datas_(datas)
	{
        }
        virtual void Run();
private:
        std::vector<boost::shared_ptr<NotifyData> > datas_;
};

/*****************************************************************************/

class SetDataHelper {
public:
	SetDataHelper() :
		setter_(this) {
			boost::thread setter_thrd( boost::bind(&SetDataHelper::SetDataMethod::Run,&setter_) );
	}
	static SetDataHelper& GetInstance();
public:
        void InvokeDelay( int, int, const std::string& );
        void InvokeNow( int, int, const std::string& );
	void Swap( std::vector<boost::shared_ptr<NotifyData> >& );
private:
	boost::mutex mutex_;
	boost::condition datas_full_;

        std::vector<boost::shared_ptr<NotifyData> > datas_;

        class SetDataMethod {
        public:
                SetDataMethod(SetDataHelper* helper) :
                        helper_(helper), notify_thrdp_(1,1) {
                        }
		void Run();
        private:
                SetDataHelper* helper_;
		xce::ThreadPool notify_thrdp_;
        };

private:
        SetDataMethod setter_;
};

