#ifndef __XceLoggerI_h__
#define __XceLoggerI_h__
#include <ServiceI.h>
#include <XceLogger.h>
#include <set>
#include <IceUtil/IceUtil.h>
#include <ctime>
#include <sys/stat.h>
#include <sys/types.h>

namespace xce
{

namespace log
{

class HourlyRollingFileAppender :
	virtual public IceUtil::Monitor<IceUtil::Mutex>,
	virtual public IceUtil::Thread {
private:
	std::string _base_fs_name;
	std::ofstream _fs;
	std::string _current_fs_name;
	time_t _next_rolling;
	std::vector<std::string> _log_cache;
	bool _inited;

	void check_rolling()
	{
		/* 1 取当前时间 */
		time_t now = time(NULL);
		//MCE_DEBUG("check_rolling " << now << " " << _next_rolling);
		/* 2 判断是否需要转换 */
		if (now < _next_rolling) {
			return;
		}
		/* 3 在initialize的时候，_fs是没有打开的 */
		MCE_INFO("Rolling file: " << _current_fs_name << " " << _next_rolling << " " << now << " (" << __FILE__ << ":" << __LINE__ << ")");
		if (_fs.is_open()) _fs.close();

		/* 4 准备新文件名 并打开 */
		struct tm timeinfo;
		localtime_r(&now, &timeinfo);
		std::ostringstream date;
		date<<_base_fs_name<<"." << setfill('0') << std::setw(4) << timeinfo.tm_year+1900 << "-" 
			<< std::setw(2) << timeinfo.tm_mon+1 <<"-"
			<< std::setw(2) << timeinfo.tm_mday << "-"
			<< std::setw(2) << timeinfo.tm_hour;
		_current_fs_name=date.str();
		MCE_INFO("Rolling file: " << _current_fs_name << " " << _next_rolling << " " << now << " (" << __FILE__ << ":" << __LINE__ << ")");
		_fs.open(_current_fs_name.c_str(), std::ios_base::app);

		/* 5 设置下个转换时间 */
		time_t next_t;
		time(&next_t);
		struct tm nextinfo;
		localtime_r(&next_t, &nextinfo);
		nextinfo.tm_sec = 0;
		nextinfo.tm_min = 60;
		_next_rolling = mktime(&nextinfo);
		if(-1 == _next_rolling) {
			MCE_WARN("mktime failed. ");
		};
		MCE_INFO("Rolling file: " << _current_fs_name << " " << _next_rolling << " " << now << " " << next_t << " " << " (" << __FILE__ << ":" << __LINE__ << ")");
	}	

	/* 实际写入 单线程调用此函数 */
	void write(std::vector<std::string>& logs) {
		MCE_INFO("HourlyRollingFileAppender::writing " << _current_fs_name << " " << logs.size() << " entries");
		for(std::vector<std::string>::const_iterator it= logs.begin();
				it != logs.end(); ++it) {
			_fs << *it << std::endl;
		}
	}
	virtual void run(){
		while(true){
			try{
				std::vector<std::string> log_to_write;
				{
					IceUtil::Monitor<IceUtil::Mutex>::Lock lock(*this);
					while (_log_cache.empty()) {
						timedWait(IceUtil::Time::seconds(10));
					}
					log_to_write.swap(_log_cache);
				}
				check_rolling();
				write(log_to_write);
			} catch (std::exception& e) {
				MCE_WARN("HourlyRollingFileAppender::run() " << _current_fs_name << " => std::exception " << e.what());
			} catch (...) {
				MCE_WARN("HourlyRollingFileAppender::run() " << _current_fs_name << " => unknown exception ");
			}
		}
		MCE_WARN("HourlyRollingFileAppender::run() " << _current_fs_name << " => unreachable code " << __FILE__ << ":" << __LINE__);
	}
public:
	HourlyRollingFileAppender(const std::string& file)
		: _base_fs_name(file), _next_rolling(0), _inited(false) {
	}

	void log(const std::string& log) {
		IceUtil::Monitor<IceUtil::Mutex>::Lock lock(*this);
		if (!_inited) {
			MCE_WARN("Please init HourlyRollingFileAppender correctly. " << _base_fs_name);
			return;
		}
		_log_cache.push_back(log);
		if (500 < _log_cache.size()) {
			notify();
		}
	}
	void initialize(){
		try{
			string::size_type slash_pos = 0;
			do {
				slash_pos = _base_fs_name.find_first_of("/", slash_pos + 1);
				MCE_INFO("mkdir " << _base_fs_name.substr(0, slash_pos) << " for " << _base_fs_name);
				mkdir(_base_fs_name.substr(0, slash_pos).c_str(), S_IRWXU | S_IRWXG
						| S_IROTH | S_IXOTH);
			} while (slash_pos < _base_fs_name.find_last_of("/"));
			check_rolling();
			this->start().detach();
			IceUtil::Monitor<IceUtil::Mutex>::Lock lock(*this);
			_inited = true;
			MCE_INFO(_base_fs_name << " inited successed.");
		}catch(Ice::Exception& e){
			MCE_WARN(_base_fs_name << " inited failed. Ice::Exception " << e);
		}catch(std::exception& e){
			MCE_WARN(_base_fs_name << " inited failed. std::exception " << e.what());
		}catch(...){
			MCE_WARN(_base_fs_name << " inited failed. Unknown exception ");
		}
	}
};

typedef IceUtil::Handle<HourlyRollingFileAppender> HourlyRollingFileAppenderPtr;

class LoggerI : virtual public Logger
{
public:

	LoggerI(const std::string& logroot);
    	virtual void append(const ::xce::log::LogEntries&,
                        const Ice::Current&);

private:
	HourlyRollingFileAppenderPtr locateAppender(const std::string& category);
	IceUtil::RWRecMutex mutex_;
	std::map<std::string, HourlyRollingFileAppenderPtr> inited_loggers_;
	std::string logroot_;
};

}

}

#endif

