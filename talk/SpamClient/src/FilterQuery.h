/*
 * FilterQuery.h
 *
 *  Created on: Jan 17, 2011
 *      Author: yantaoqiao
 */

#ifndef FILTERQUERY_H_
#define FILTERQUERY_H_

#include <vector>
#include <string>
#include <map>

using namespace std;

class FilterQuery {
private:
    /** 发送者ID */
	int _iFromId;
    /** 接收者列表 */
	vector<string> _toIdV;
    /** 业务类型 */
	int _iType;
	/** 内容列表 */
	vector<string> _contentV;
    /** 发送者ip */
	string strUserIp;
	string strClientIp;
public:
	FilterQuery();
	FilterQuery(int fromId, int type, string content);
	FilterQuery(int fromId, int type, vector<string> contentList);
	FilterQuery(map<string, vector<string> > queryMap);
	virtual ~FilterQuery();
	int getFromId();
	int getType();
	void addToId(int id);
	string getIp();
	string getClientIp();
	vector<string> getToIdList();
	vector<string> getContentList();
	map<string, vector<string> > getMap();
};

#endif /* FILTERQUERY_H_ */
