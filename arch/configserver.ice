#ifndef XCE_ARCH_REMOTE_CONFIG_ICE__
#define XCE_ARCH_REMOTE_CONFIG_ICE__

module xce {


interface ConfigListener {
  bool Update(string key, string val);
};

sequence<string> StringList;
dictionary<string, string> ConfigContent;

interface ConfigServer {
  // 注册时，key 支持以下形式
  // a   => a, a.*
  // *   => *
  // a.b => a.b.*
  void Subscribe(string key, ConfigListener* o);
  void Unsubscribe(string key, ConfigListener* o);

  // 完全匹配的查询
  ["cpp:const"] string Query(string key);

  // 支持 a.* a* a. a 查询方式
  ["cpp:const"] ConfigContent BatchQuery(string key);

  void Update(string key, string val);
  void Updates(StringList key, StringList val);
};

};
#endif // XCE_ARCH_REMOTE_CONFIG_ICE__
