/*
 * FilterAnswer.cpp
 *
 *  Created on: Jan 17, 2011
 *      Author: yantaoqiao
 */

#include "FilterAnswer.h"
#include <cstdlib>
using namespace std;
FilterAnswer::FilterAnswer() {
	flag_i = 0;
}

FilterAnswer::~FilterAnswer() {
	// TODO Auto-generated destructor stub
}

FilterAnswer::FilterAnswer(map<string, vector<string> > answerMap) {
	map<string, vector<string> >::iterator it = answerMap.begin();
	it = answerMap.find("flag");
	if(it != answerMap.end()) {
		this->flag_i = atoi((it->second.at(0).c_str()));
	} else {
		this->flag_i = 0;
	}

	it = answerMap.find("content");
	if(it != answerMap.end()) {
		this->contentV = it->second;
	}

	it = answerMap.find("highLight");
	if(it != answerMap.end()) {
		this->highLightV = it->second;
	}

	it = answerMap.find("auditFirst");
	if(it != answerMap.end()) {
		this->auditFirst_b = atoi(it->second.at(0).c_str());
	} else {
		this->auditFirst_b = false;
	}

	it = answerMap.find("notNotify");
	if(it != answerMap.end()) {
		this->notNotify_b = atoi(it->second.at(0).c_str());
	} else {
		this->notNotify_b = false;
	}
}


CheckResult FilterAnswer::toCheckResult(string content) {
	CheckResult result;
	return result;
}

CheckResultMore FilterAnswer::toCheckResultMore(vector<string> preContentList) {
	CheckResultMore result(this->flag_i, preContentList, contentV, highLightV);
	if(timeout_b) {
		result.setTimeout(timeout_b);
	}
	return result;
}

