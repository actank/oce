#include "AdminConsoleI.h"
#include <vector>
#include <string>
#include <list>
#include <map>
#include <set>
#include <deque>
#include <stack>
#include <bitset>
#include <algorithm>
#include <functional>
#include <numeric>
#include <utility>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <cstdio>
#include <cmath>
#include <cstdlib>
#include <ctime>

using namespace std;

/*
 * Supported operators:
 * 1) help:
 * 2) componentType:
 * 3) componentType help:
 * 4) componentType list:
 * 5) componentType componentName arg0 arg1 arg2 ...
 * */

string xce::adminconsole::AdminConsoleManagerI::parse(const string& order,
		const Ice::Current& current) {
	istringstream stream(order);
	vector<string> input;
	string str;
	while (stream >> str) {
		input.push_back(str);
	}
	if (input.size() == 0) {
		return "";
	} else if (input.size() == 1) {
		if (input[0] == "help") {
			return helpDescription;
		} else {
			map<string, map<string, xce::adminconsole::AdminConsoleComponentPtr> >::iterator iter = componentLists.find(input[0]); 
			if (iter != componentLists.end()) {
				return iter->second.begin()->second->help();
			} else {
				return commandNotFound;
			}
		}
	} else if (input.size() == 2) {
		map<string, map<string, xce::adminconsole::AdminConsoleComponentPtr> >::iterator iter = componentLists.find(input[0]);
		if (iter != componentLists.end()) {
			if (input[1] == "help") {
				return iter->second.begin()->second->help();
			} else if (input[1] == "list") {
				return list(iter->second);
			} else {
				return commandNotFound;
			}
		} else {
			return commandNotFound;
		}
	} else {
		map<string, map<string, xce::adminconsole::AdminConsoleComponentPtr> >::iterator iter = componentLists.find(input[0]);
		if (iter != componentLists.end()) {
			map<string, xce::adminconsole::AdminConsoleComponentPtr>::iterator iter1 = iter->second.find(input[1]);
			if (iter1 != iter->second.end()) {
				input.erase(input.begin());
				input.erase(input.begin());
				return iter1->second->execute(input);
			} else {
				return commandNotFound;
			}
		} else {
			return commandNotFound;
		}
	}
	return string();
}

string xce::adminconsole::AdminConsoleManagerI::list(map<string, xce::adminconsole::AdminConsoleComponentPtr>& resultMap) {
	string result = "";
	for (map<string, xce::adminconsole::AdminConsoleComponentPtr>::iterator iter = resultMap.begin(); iter
			!= resultMap.end(); ++iter) {
		result = result + (iter->first) + "\t";
	}
	return result + "\n";
}

string xce::adminconsole::AdminConsoleManagerI::registerComponent(
		const string& componentType, const string& componentName,
		const xce::adminconsole::AdminConsoleComponentPtr componentInstance) {
	map<string, map<string, xce::adminconsole::AdminConsoleComponentPtr> >::iterator iter = componentLists.find(componentType);
	if (iter == componentLists.end()) {
		helpDescription = helpDescription + componentType + "\t" +componentInstance->description();
		map<string, xce::adminconsole::AdminConsoleComponentPtr> temp;
		temp.insert(make_pair(componentName, componentInstance));
		componentLists.insert(make_pair(componentType, temp));
		return "register instance successful. yeap, you are the first to use this component.\n";
	} else {
		if (iter->second.find(componentName) != iter->second.end()) {
			return componentName
					+ " already exists, please use another name.\n";
		} else {
			iter->second.insert(make_pair(componentName, componentInstance));
			return "register instance successful.\n";
		}
	}
}
