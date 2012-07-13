#ifndef MONITOR_ICE
#define MONITOR_ICE

module monitor {

const int ALERTERSMS = 0;
const int ALERTEREMAIL = 1;
const int ALERTERALL = 2;

class MonitorAlerter {

  void alert(string subject, string message, int type, int interval);
  void alertmerely(int type, string phone, string email, string subject, string message);

  void reload();
};

class MonitorDispatcher {
  void report(string type, string message);
};

class MonitorAnalyzer {
  void analyse(string type, string message);
  void reload();
};

class MonitorHeartBeater {
  void beat(string name);
  void remove(string name);
};

};

#endif
