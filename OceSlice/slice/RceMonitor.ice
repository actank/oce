#ifndef RCEMONITOR_H
#define RCEMONITOR_H

module rce {
module monitor {
  const int SENDMSG = 1;
  const int SENDEMAIL = 2;
  const int SENDALL = 3;

  interface RceMonitorAlerter
  {   
    void reloadAlertMap();
    void notifyAlert(string type, string pattern, string msg, int danger);
  };  

  interface RceMonitorAnalyzer
  {
    void report(string bussiness, string info);
    void reloadConfig();
  };

  interface RceMonitorDispatcher
  {
    void report(string bussiness, string info);
    void reloadConfig();
  };


  dictionary<string, string> Instance;
  sequence<Instance> Instances;
  dictionary<string, Instances> Business;
  dictionary<string, Business> Businesses;
  dictionary<string, string> User;
  dictionary<string, User> Users;

  interface RceMonitorConfiger {
    Users getUsers();
    User getUser(string name);
    Businesses getBusinesses();
    Business getBusiness(string name);
    void setBusiness(string name, Business b);
    void setUser(string name, User u);
    void exportConfig(string file);
    void importConfig(string file);
    void update(string file,string bussiness);
  };

};
};

#endif
