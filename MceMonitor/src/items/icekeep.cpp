
#include <Ice/Ice.h>
#include <boost/lexical_cast.hpp>
#include "item.h"
#include "icekeep.h"
#include <sys/types.h>
#include <csignal>
#include "../MceMonitorI.h"

using namespace std;
using namespace MceMonitor;
using namespace MceMonitor::Item;

IceKeep_Info::IceKeep_Info(const Ice::CommunicatorPtr& communicator, const string& machine_id, const Resource& item)
	:Item(machine_id, item)
{
	_ic = communicator;
	_out = Ice::createOutputStream(_ic);
}

void IceKeep_Info::run()
{
	while(true)
	{
		Log m;
		m.resourcegroup = _machine_id;
		m.resource = _item.name;
		m.type = _item.type;
		m.time = time(NULL);
		switch ( _item.parameters.size() )
		{
		case 4:
			m.value = get(_item.parameters[1], _item.parameters[2], _item.parameters[3]);
			break;
		case 5:
			{
			int param1 = boost::lexical_cast<int>(_item.parameters[4]);
			m.value = get(_item.parameters[1], _item.parameters[2], _item.parameters[3], param1);
			break;
			}
		case 6:
			{
			int param1 = boost::lexical_cast<int>(_item.parameters[4]);
			int param2 = boost::lexical_cast<int>(_item.parameters[5]);
			m.value = get(_item.parameters[1], _item.parameters[2], _item.parameters[3], param1, param2);
			break;
			}
		case 7:
			{
			int param1 = boost::lexical_cast<int>(_item.parameters[4]);
			int param2 = boost::lexical_cast<int>(_item.parameters[5]);
			int param3 = boost::lexical_cast<int>(_item.parameters[6]);
			m.value = get(_item.parameters[1], _item.parameters[2], _item.parameters[3], param1, param2, param3);
			break;
			}
		case 8:
			{
			int param1 = boost::lexical_cast<int>(_item.parameters[4]);
			int param2 = boost::lexical_cast<int>(_item.parameters[5]);
			int param3 = boost::lexical_cast<int>(_item.parameters[6]);
			int param4 = boost::lexical_cast<int>(_item.parameters[7]);
			m.value = get(_item.parameters[1], _item.parameters[2], _item.parameters[3], param1, param2, param3, param4);
			break;
			}
		default:
			break;
		}
		MonitorManagerI::instance().getMonitorManager()->addLog(m);
		//cout << m.value << endl;
		
		//do reboot
		if ( m.value != 100 ) {
			reboot_ice(_item.parameters[0]);
		}
		sleep(_item.interval);
	}
}

int IceKeep_Info::get(string strProxy, string invokeType, string strInvoke)
{
	return getStatus(strProxy, invokeType, strInvoke);
}


int IceKeep_Info::get(string strProxy, string invokeType, string strInvoke, int a)
{
	_out->writeInt(a);
	return getStatus(strProxy, invokeType, strInvoke);
}

int IceKeep_Info::get(string strProxy, string invokeType, string strInvoke, int a, int b)
{
	_out->writeInt(a);
	_out->writeInt(b);
	return getStatus(strProxy, invokeType, strInvoke);
}

int IceKeep_Info::get(string strProxy, string invokeType, string strInvoke, int a, int b, int c)
{
	_out->writeInt(a);
	_out->writeInt(b);
	_out->writeInt(c);
	return getStatus(strProxy, invokeType, strInvoke);
}

int IceKeep_Info::get(string strProxy, string invokeType, string strInvoke, int a, int b, int c, int d)
{
	_out->writeInt(a);
	_out->writeInt(b);
	_out->writeInt(c);
	_out->writeInt(d);
	return getStatus(strProxy, invokeType, strInvoke);
}

int IceKeep_Info::getStatus(string strProxy, string invokeType, string strInvoke)
{
	int result;
	Ice::ObjectPrx obj = _ic->stringToProxy(strProxy);
	bool isNonmutating = invokeType == "Normal" ? false : true ;
	try
	{
		std::vector< Ice::Byte > inParams, outParams;
		_out->finished(inParams);
		bool invokeResult;
		if ( isNonmutating ) 
		{
			invokeResult = obj->ice_invoke(strInvoke, Ice::Nonmutating, inParams, outParams);
		}
		else 
		{
			invokeResult = obj->ice_invoke(strInvoke, Ice::Normal, inParams, outParams);
		}
		
		if ( invokeResult )
		{
			// Handle success
			//cout << "OK" << endl;
			Ice::InputStreamPtr in = Ice::createInputStream(_ic, outParams);
			//int result = in->readInt();
			//cout << "RESULT: " << result << endl;
			result = 100;
			//assert(result == );
		} else {
			// Handle user exception
			//cout << "ERROR" << endl;
			result = 60;
		}
	} catch (const Ice::LocalException& ex) {
		// Handle exception
		//cout << "TERR" << endl;
		//cout << ex << endl;
		result = 0;
	} catch (...) {
		result = -1;
	}
	return result;
}

void IceKeep_Info::reboot_ice(string strBoxName)
{
	int pid = -1;
	FILE *fp;
	int status;
	char tmp[PATH_MAX];
	ostringstream cmd;
	cmd << "ps aux|grep icebox|grep " << strBoxName << "|grep -v grep|awk '{print $2}'";
	fp = popen( cmd.str().c_str(), "r" );
	if (fp == NULL)
	{
		cout << "open failed." << endl;
	}
	if ( fgets(tmp, PATH_MAX, fp) != NULL )
	{
		pid = boost::lexical_cast<int>(tmp);
	}
	status = pclose(fp);
	//if (status == -1) continue;else break;
	cout << "[DEBUG][ICEKEEP] " << strBoxName << " " << pid << " killed" << endl;
	if ( pid > 1 ) {
		kill((pid_t)pid,SIGKILL);
	}
	return;	
}
