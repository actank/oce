/*
 * FilterAnswer.h
 *
 *  Created on: Jan 17, 2011
 *      Author: yantaoqiao
 */

#ifndef FILTERANSWER_H_
#define FILTERANSWER_H_

#include <vector>
#include <string>
#include <map>
#include "CheckResult.h"

using namespace std;

class FilterAnswer {
private:
	int flag_i;
	vector<string> contentV;
	vector<string> highLightV;
	bool auditFirst_b;
	bool notNotify_b;
	int infoType_i;
	bool timeout_b;

public:
	FilterAnswer();
	virtual ~FilterAnswer();
	FilterAnswer(int flag);
	FilterAnswer(int flag, vector<string> contentList, vector<string> highLightList);
	FilterAnswer(map<string, vector<string> > answerMap);
	bool isTimeout() {return timeout_b;};
	void setTimeout(bool timeout) {this->timeout_b = timeout;};
	CheckResult toCheckResult(string content);
	CheckResultMore toCheckResultMore(vector<string> preContentList);
	CheckResultMap toCheckResultMap(map<string, string> preContentMap, vector<string> keyV);

	map<string, string> getMap();
	vector<string> getHighLightV() {return this->highLightV;}
	vector<string> getContentV() {return this->contentV;}
	int getFlag() { return this->flag_i;}
	void addHighLight(string highLight) {this->highLightV.push_back(highLight);}
	bool isAuditFirst() {return this->auditFirst_b;}
	bool isNotNotify() {return this->notNotify_b;}
};

#endif /* FILTERANSWER_H_ */
