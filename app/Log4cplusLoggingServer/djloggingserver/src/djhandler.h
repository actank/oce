#include "loggingserver.h"
#include "locallogger.h"

#include "base/url.h"
#include "boost/lexical_cast.hpp"
#include "json/json.h"

using namespace std;
using namespace log4cplus::spi;

class DjHandler : virtual public logging::SocketLoggingHandler {
public:
	DjHandler( const string& logging_path, long max_file_size, int max_backup_index, const string& logging_level ) {  
		LOCAL_INIT( logging_path, max_file_size, max_backup_index, logging_level );
	}
protected:
        virtual bool handle(InternalLoggingEvent& event) const;
};

bool DjHandler::handle(InternalLoggingEvent& event) const {

	std::string paramJ = event.getMessage();

	Json::Value value;
	Json::Reader reader;
	try{
		if(reader.parse(paramJ,value)) {
			int x = boost::lexical_cast<int>( value.get("X", 0).asInt() );
			int y = boost::lexical_cast<int>( value.get("Y", 0).asInt() );
			std::string r = value.get("R", "").asString();
			std::string u = value.get("U", "").asString();
			std::string t = value.get("T", "html").asString();
			std::string id = value.get("ID", "0").asString();
			std::string time = value.get("time", "null").asString();

			Url r_url( r.c_str() );
			ostringstream o;
			o << "NULL\t"
				<< x << "\t"
				<< y << "\t"
				<< r_url.host() << "\t"
				<< r_url.path() << "\t"
				<< r_url.parameter() << "\t"
				<< u << "\t"
				<< t << "\t"
				<< id << "\t"
				<< time << "\t"
				;

			LOCAL_INFO( o.str() );
			return true;
		} else{
			return false;
		}
	} catch(...){
		return false;
	}

}

