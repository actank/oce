#include "MonitorAdapter.h"

using monitor::MonitorAlerterAdapter;
using monitor::MonitorDispatcherAdapter;
using monitor::MonitorAnalyzerAdapter;
using monitor::MonitorHeartBeaterAdapter;

void MonitorAlerterAdapter::alert(const string& subject, const string& msg, int type, int interval = 900) {
  try {
    getProxy(0)->alert(subject, msg, type, interval);
  } catch (const Ice::Exception& e) {
    MCE_WARN("MonitorAlerterAdapter::alert Ice::Exception: " << e);
  } catch (...) {
    MCE_WARN("MonitorAlerterAdapter::alert unknown exception");
  }
}

void MonitorAlerterAdapter::reload() {
  try {
    getProxy(0)->reload();
  } catch (const Ice::Exception& e) {
    MCE_WARN("MonitorAlerterAdapter::reload Ice::Exception: " << e);
  } catch (...) {
    MCE_WARN("MonitorAlerterAdapter::reload unknown exception");
  }
}

void MonitorDispatcherAdapter::report(const string& type, const string& message) {
  try {
    getProxy(0)->report(type, message);
  } catch (const Ice::Exception& e) {
    MCE_WARN("MonitorDispatcherAdapter::report Ice::Exception: " << e);
  } catch (...) {
    MCE_WARN("MonitorDispatcherAdapter::report unknown exception");
  }
}

void MonitorHeartBeaterAdapter::remove(const string& subject) {
  try {
    getProxy(0)->remove(subject);
  } catch (const Ice::Exception& e) {
    MCE_WARN("MonitorHeartBeaterAdapter::remove Ice::Excetpion: " << e);
  } catch (...) {
    MCE_WARN("MonitorHeartBeaterAdapter::remove unknown exception");
  }
}
void MonitorHeartBeaterAdapter::beat(const string& subject) {
  try {
    getProxy(0)->beat(subject);
  } catch (const Ice::Exception& e) {
    MCE_WARN("MonitorHeartBeaterAdapter::beat Ice::Excetpion: " << e);
  } catch (...) {
    MCE_WARN("MonitorHeartBeaterAdapter::beat unknown exception");
  }
}

void MonitorAnalyzerAdapter::analyse(const string& subject, const string& msg) {
  try {
    getProxy(0)->analyse(subject, msg);
  } catch (const Ice::Exception& e) {
    MCE_WARN("MonitorAnalyzerAdapter::analyse Ice::Excetpion: " << e);
  } catch (...) {
    MCE_WARN("MonitorAnalyzerAdapter::analyse unknown exception");
  }
}

void MonitorAnalyzerAdapter::reload() {
  try {
    getProxy(0)->reload();
  } catch (const Ice::Exception& e) {
    MCE_WARN("MonitorAnalyzerAdapter::reload Ice::Excetpion: " << e);
  } catch (...) {
    MCE_WARN("MonitorAnalyzerAdapter::reload unknown exception");
  }
}
