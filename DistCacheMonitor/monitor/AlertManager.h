#ifndef __ALERT_MANAGER_H__ 
#define __ALERT_MANAGER_H__

#include <ServiceI.h>

#include <IceUtil/Mutex.h>

#include <XceAlertAdapter.h>

namespace xce
{
namespace distcache
{
namespace monitor
{
	class AlertManager : public MyUtil::Singleton<AlertManager>
	{
		public:

			AlertManager()
			{ 
				MyUtil::ServiceI& service = MyUtil::ServiceI::instance();

				Ice::PropertiesPtr props = service.getCommunicator()->getProperties();
				
				m_fileName = props->getPropertyWithDefault("Service.Config",
							"/data/dc/UserDCService/etc/templates/Alert.config");
			}
			
			void reloadConfig()
			{
				MCE_INFO("AlertManager::reloadConfig start");

				IceUtil::Mutex::Lock lock(m_mutex); 
			
				Ice::PropertiesPtr prop = Ice::createProperties();
				
				prop->load(m_fileName);

				stringstream mobilestream(prop->getProperty("Mobile")); 
				
				string mobile;
				
				while (mobilestream >> mobile) 
				{
					m_phoneNums.push_back(mobile);
					MCE_INFO("Add Mobile : "<<mobile);
				} 
				MCE_INFO("AlertManger::reloadConfig end");
			}

			void alert(const std::string& msg)
			{
				std::string machineName;
		
				machineName.assign(getenv("NICKNAME"));
		
				MyUtil::StrSeq typeSeq;
				
				typeSeq.push_back(machineName);

				try
				{
					com::xiaonei::xce::XceAlertAdapter::instance().notifyAlert(typeSeq, msg);
				}catch(Ice::Exception& e)
				{
					alertForAlertService( e.what() );
				}catch(...)
				{
					alertForAlertService( "UnKnow Exception" );
				}
			}

			void alertForMonitorService( const std::string& reasonStr )
			{
				std::string machineName;
		
				machineName.assign(getenv("NICKNAME"));
		
				MyUtil::StrSeq typeSeq;
				
				typeSeq.push_back(machineName);
				
				typeSeq.push_back("DC");
			
				std::string msg;

				msg = "DistCacheMonitor on " + machineName + " Exception : " + reasonStr ;

				try
				{
					com::xiaonei::xce::XceAlertAdapter::instance().notifyAlert(typeSeq, msg);
				}catch(Ice::Exception& e)
				{
					alertForAlertService( e.what() );
				}catch(...)
				{
					alertForAlertService( "UnKnow Exception" );
				}
			}
			
			void alertForAlertService( const std::string& reasonMsg )
			{
				MyUtil::StrSeq phoneNums;

				{
					IceUtil::Mutex::Lock lock(m_mutex); 

					phoneNums = m_phoneNums;
				}

				std::string msg;

				msg = "DistCacheMonitor Can not call XceAlert Exception: " + reasonMsg;

				for( MyUtil::StrSeq::const_iterator it = phoneNums.begin(); it != phoneNums.end(); ++it )
				{
					
					std::ostringstream cmd;

					cmd << "wget -q -O /dev/null \"http://10.22.198.81:2000/receiver?number="<<(*it)<<"&message="<<msg<<"\""; 
					
					system(cmd.str().c_str());

					MCE_WARN(cmd.str());

					MCE_WARN(*it);
				}
			}

		private: 

			IceUtil::Mutex m_mutex;
			
			std::string m_fileName;
			
			MyUtil::StrSeq m_phoneNums;
	};
}
}
}

#endif
