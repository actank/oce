/*
 * CheckResult.h
 *
 *  Created on: Jan 19, 2011
 *      Author: yantaoqiao
 */

#ifndef CHECKRESULT_H_
#define CHECKRESULT_H_

#include <string>
#include <vector>
#include <map>

using namespace std;

class CheckResult {

private:
	int _flagI;
	string _preContentStr;
	string _safeContentStr;
	string _highLightContentStr;
	bool _auditFirstB;
	bool _notNotifyB;
	bool _advertisementB;
	bool _timeB;
	int _infoTypeI;

public:
	CheckResult();
	CheckResult(int flag, string preContent);
	CheckResult(int flag, string content, int infoType);
	virtual ~CheckResult();

	static const int SAFE = 0;
};

class CheckResultMore {
private:
	int _flagI;
	int _infoTypeI;
	vector<string> _preContentV;
	vector<string> _safeContentV;
	vector<string> _highLightContentV;
	bool _auditFirstB;
	bool _notNotifyB;
	bool _advertisementB;
	bool _timeB;

public:
	CheckResultMore(int flag, vector<string> precotnentV) :
		_flagI(flag) {
		this->_preContentV = precotnentV;
	}
	CheckResultMore(int flag, int infoType, vector<string> precotnentV) :
		_flagI(flag), _infoTypeI(infoType) {
		this->_preContentV = precotnentV;
	}

	CheckResultMore(int flag, vector<string> precotnentV,
			vector<string> contentV, vector<string> highLightV) :
		_flagI(flag) {
		this->_preContentV = precotnentV;
		this->_safeContentV = contentV;
		this->_highLightContentV = highLightV;
	}

	bool isTimeout() {
		return _timeB;
	}
	bool isAdvertisement() {
		return _advertisementB;
	}

	void setTimeout(bool timeout) {
		this->_timeB = timeout;
	}

	int getFlag() {
		return this->_flagI;
	}

	int getInfoType() {
		return this->_infoTypeI;
	}

	void setReason(int infoType) {
		this->_infoTypeI = infoType;
	}

	vector<string> getHighLightV() {
		return _highLightContentV;
	}

	void setNotNotify(bool noNotify) {
		this->_notNotifyB = noNotify;
	}

	bool isNotNotify() {
		return this->_notNotifyB;
	}

	void setAuditFirst(bool auditFirst) {
		this->_auditFirstB = auditFirst;
	}

	bool isAuditFirst() {
		return this->_auditFirstB;
	}
};

class CheckResultMap {
private:
	int _flagI;
	int _infoTypeI;
	map<string, string> _preContentMap;
	map<string, string> _safeContentMap;
	map<string, string> _highLightContentMap;
	bool _timeB;
public:
	CheckResultMap(int flag, map<string, string> preContentMap):_flagI(flag) {
		this->_preContentMap = preContentMap;
		this->_highLightContentMap = preContentMap;
		this->_safeContentMap = preContentMap;
	}

	CheckResultMap(int flag, int reason, map<string, string> preContentMap):_flagI(flag) {
		this->_preContentMap = preContentMap;
		this->_highLightContentMap = preContentMap;
		this->_safeContentMap = preContentMap;
	}

	CheckResultMap(int flag, map<string, string> preContentMap, map<string, string> contentMap, map<string, string> highLightMap):_flagI(flag) {
		this->_preContentMap = preContentMap;
		this->_highLightContentMap = contentMap;
		this->_safeContentMap = highLightMap;
	}

	bool isTimeout() {
		return _timeB;
	}

	void setTimeout(bool timeout) {
		this->_timeB = timeout;
	}

	int getInfoType() {
		return _infoTypeI;
	}
	void setInforType(int infoType) {
		this->_infoTypeI = infoType;
	}

	int getFlag() {return _flagI;}


};
#endif /* CHECKRESULT_H_ */
