
#include <Ice/Ice.h>
#include <boost/lexical_cast.hpp>
#include "item.h"
#include "ice.h"
#include "../MceMonitorI.h"

using namespace std;
using namespace MceMonitor;
using namespace MceMonitor::Item;

Ice_Info::Ice_Info(
		const Ice::CommunicatorPtr& communicator,
		const string& machine_id, 
		const Resource& item)
: Item(machine_id, item)
{
	_ic = communicator;
	_out = Ice::createOutputStream(_ic);
	switch ( item.parameters.size() )
	{
		case 3:
			break;
		case 4:
			{
				int param1 = boost::lexical_cast<int>(item.parameters[3]);
				_out->writeInt(param1);
				break;
			}
		case 5:
			{
				int param1 = boost::lexical_cast<int>(item.parameters[3]);
				int param2 = boost::lexical_cast<int>(item.parameters[4]);
				_out->writeInt(param1);
				_out->writeInt(param2);
				break;
			}
		case 6:
			{
				int param1 = boost::lexical_cast<int>(item.parameters[3]);
				int param2 = boost::lexical_cast<int>(item.parameters[4]);
				int param3 = boost::lexical_cast<int>(item.parameters[5]);
				_out->writeInt(param1);
				_out->writeInt(param2);
				_out->writeInt(param3);
				break;
			}
		case 7:
			{
				int param1 = boost::lexical_cast<int>(item.parameters[3]);
				int param2 = boost::lexical_cast<int>(item.parameters[4]);
				int param3 = boost::lexical_cast<int>(item.parameters[5]);
				int param4 = boost::lexical_cast<int>(item.parameters[6]);
				_out->writeInt(param1);
				_out->writeInt(param2);
				_out->writeInt(param3);
				_out->writeInt(param4);
				break;
			}
		default:
			break;
	}
	_strProxy = item.parameters.at(0);
	_invokeType = item.parameters.at(1);
	_strInvoke = item.parameters.at(2);
}

void Ice_Info::run()
{
	Log m;
	m.resourcegroup = _machine_id;
	m.resource = _item.name;
	m.type = _item.type;
	while(true)
	{
		m.value = getStatus();
		m.time = time(NULL);

		MonitorManagerI::instance().getMonitorManager()->addLog(m);
		//cout << m.value << endl;
		sleep(_item.interval);
	}
}

int Ice_Info::getStatus()
{
	try
	{
		Ice::ObjectPrx obj = _ic->stringToProxy(_strProxy);
		std::vector< Ice::Byte > inParams, outParams;
		_out->finished(inParams);
		bool invokeResult;
		if ( _invokeType == "Normal" ) 
		{
			invokeResult = obj->ice_invoke(_strInvoke, Ice::Normal, inParams, outParams);
		}
		else 
		{
			invokeResult = obj->ice_invoke(_strInvoke, Ice::Nonmutating, inParams, outParams);
		}
		
		if (invokeResult)
		{
			// Handle success
			//cout << "OK" << endl;
			Ice::InputStreamPtr in = Ice::createInputStream(_ic, outParams);
			//int result = in->readInt();
			//cout << "RESULT: " << result << endl;
			return 100;
			//assert(result == );
		} else {
			// Handle user exception
			//cout << "ERROR" << endl;
			return 60;
		}
	} catch (const Ice::LocalException& ex) {
		// Handle exception
		//cout << "TERR" << endl;
		std::cout << "LocalException:" << ex << std::endl;
		return 0;
	} catch ( const Ice::Exception& ex ) {
		std::cout << "Exception:" << ex << std::endl;
		return 0;
	} catch (...) {
		return -1;
	}
}
