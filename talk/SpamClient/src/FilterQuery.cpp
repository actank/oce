/*
 * FilterQuery.cpp
 *
 *  Created on: Jan 17, 2011
 *      Author: yantaoqiao
 */

#include <cstdlib>
#include "FilterQuery.h"

FilterQuery::FilterQuery() {
	// TODO Auto-generated constructor stub

}

FilterQuery::~FilterQuery() {
	// TODO Auto-generated destructor stub
}

FilterQuery::FilterQuery(int fromId, int type, string content) {
	_iFromId = fromId;
	_iType = type;
	_contentV.push_back(content);
	strUserIp = "NOIP";
	strClientIp = "";
}

FilterQuery::FilterQuery(int fromId, int type, vector<string> contentList) {
	_iFromId = fromId;
	_iType = type;
	_contentV = contentList;
	strUserIp = "NOIP";
	strClientIp = "";
}

FilterQuery::FilterQuery(map<string, vector<string> > queryMap) {
	map<string, vector<string> >::iterator it;
	it = queryMap.find("fromId");
	if (it != queryMap.end()) {
		_iFromId = atoi(it->second[0].c_str());
	} else {
		_iFromId = 0;
	}

	it = queryMap.find("toId");
	if (it != queryMap.end()) {
		_toIdV = it->second;
	}

	it = queryMap.find("type");
	if (it != queryMap.end()) {
		_iType = atoi(it->second[0].c_str());
	} else {
		_iType = -1;
	}

	it = queryMap.find("content");
	if (it != queryMap.end()) {
		_contentV = it->second;
	}

	it = queryMap.find("ip");
	if (it != queryMap.end()) {
		strUserIp = it->second[0];
	} else {
		strUserIp = "NOIP";
	}

}

map<string, vector<string> > FilterQuery::getMap() {
	map<string, vector<string> > queryMap;
	vector<string> v_id;
	char fromid_str[64];
	sprintf(fromid_str, "%d", _iFromId);
	v_id.push_back(string(fromid_str));
	queryMap["fromId"] = v_id;

	queryMap["toId"] = _toIdV;

	vector<string> v_type;
	char type_str[64];
	sprintf(type_str, "%d", _iType);
	v_type.push_back(string(type_str));
	queryMap["type"] = v_type;

	queryMap["content"] = _contentV;

	if (strUserIp.compare("NOIP") != 0) {
		vector<string> v_ip;
		v_ip.push_back(strUserIp);
		queryMap["ip"] = v_ip;
	}

	return queryMap;
}

int FilterQuery::getFromId() {
	return _iFromId;
}

int FilterQuery::getType() {
	return _iType;
}

void FilterQuery::addToId(int id) {
	char fromid_str[64];
	sprintf(fromid_str, "%d", _iFromId);
	_toIdV.push_back(string(fromid_str));
}

string FilterQuery::getIp() {
	return strUserIp;
}

vector<string> FilterQuery::getToIdList() {
	return _toIdV;
}

vector<string> FilterQuery::getContentList() {
	return _contentV;
}
