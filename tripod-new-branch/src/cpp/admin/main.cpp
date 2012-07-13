#include <expat.h>
#include <cstdio>
#include <iostream>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <string>
#include <set>
#include <map>
#include <vector>
#include <zookeeper/zookeeper.h>

using namespace std;
using namespace boost;
using namespace boost::algorithm;

vector<string> xmlpath;
string xmlnsname;
string xmltypename;
zhandle_t *zk;

class Node {
public:
  Node(const string& host, int port) : host_(host), port_(port) {}
  string toString() {
      return host_ + ":" + lexical_cast<string>(port_);
  }
  bool operator<(const Node &m) const {
    if (host_ < m.host_) {
      return true;
    } else {
      return port_ < m.port_;
    }
  }
private:
  string host_;
  int port_;
};

class Config {
public:
  map<string, set<Node> > & caches() {return caches_;}
  set<Node> & managers() {return managers_;}
  map<string, set<Node> > & producers() {return producers_;}
private:
  map<string, set<Node> > caches_;
  set<Node> managers_;
  map<string, set<Node> > producers_;
};

Config zookeeperconfig;
Config xmlconfig;

static void start(void *data, const char *el, const char **attr) {
  xmlpath.push_back(string(el));

  map<string, string> attrmap;
  for (int i = 0; attr[i]; i += 2) {
    attrmap[string(attr[i])] = string(attr[i+1]);
  }

  if ("namespace" == string(el) && "tripod" == xmlpath.at(0)) {
    xmlnsname = attrmap["name"];
  }

  if ("type" == string(el) && "tripod" == xmlpath.at(0)) {
    xmltypename = attrmap["name"];
  }

  if ("node" == string(el) && "tripod" == xmlpath.at(0)) {
    string host = attrmap["host"];
    int port = lexical_cast<int>(attrmap["port"]);

    if (xmlpath.at(xmlpath.size() - 2) == "namespace") {
      xmlconfig.caches()[xmlnsname].insert(Node(host, port));
    } else if (xmlpath.at(xmlpath.size() - 2) == "manager") {
      xmlconfig.managers().insert(Node(host, port));
    } else if (xmlpath.at(xmlpath.size() - 3) == "producer") {
      xmlconfig.producers()[xmltypename].insert(Node(host, port));
    }
  }
}

static void end(void *data, const char *el) {
  xmlpath.pop_back();
}

void GetXmlConfig(const std::string& xmlPath) {
  const int BUFFSIZE = 8192;
  char Buff[BUFFSIZE];
  XML_Parser p = XML_ParserCreate(NULL);
  if (!p) {
    cerr << endl;
    std::cout<<"Can not get XML_Parser!"<<std::endl;
    exit(-1);
  }
  XML_SetElementHandler(p, start, end);

  FILE* pFile= fopen(xmlPath.c_str(),"rb");

  for (;;) {
    int done;
    int len;

    len = (int)fread(Buff, 1, BUFFSIZE, pFile);
    if (ferror(pFile)) {
      fprintf(stderr, "Read error\n");
      exit(-1);
    }
    done = feof(pFile);

    if (XML_Parse(p, Buff, len, done) == XML_STATUS_ERROR) {
      fprintf(stderr, "Parse error at line %" "l" "u:\n%s\n",
              XML_GetCurrentLineNumber(p),
              XML_ErrorString(XML_GetErrorCode(p)));
      exit(-1);
    }

    if (done)
      break;
  }
  XML_ParserFree(p);
}

Node GetZooKeeperNode(const string& node) {
  vector<string> parts;
  split(parts, node, is_any_of(":"));
  if (parts.size() == 2) {
    return Node(parts[0], lexical_cast<int>(parts[1]));
  }
}

void GetZooKeeperConfig() {
  if (zk == NULL) {
    std::cout<<"Can not get zk in GetZooKeeperConfig!"<<std::endl;
    exit(-1);
  }

  // Manager
  struct String_vector managernodes;
  int rc = zoo_get_children(zk, "/Tripod/Manager", 0, &managernodes);
  if (rc != ZOK) {
    std::cout<<"Can not get children for path Manager!"<<std::endl;
    exit(-1);
  }
  for (int i = 0; i < managernodes.count; ++i) {
    zookeeperconfig.managers().insert(GetZooKeeperNode(string(managernodes.data[i])));
  }

  // Producer
  struct String_vector producernodes;
  rc = zoo_get_children(zk, "/Tripod/Producer", 0, &producernodes);
  if (rc != ZOK) {
    std::cout<<"Can not get children for path Producer!"<<std::endl;
    exit(-1);
  }
  for (int i = 0; i < producernodes.count; ++i) {
    string node = string(producernodes.data[i]);
    ostringstream oss;
    oss << "/Tripod/Producer/" << node;
    struct String_vector typenodes;
    rc = zoo_get_children(zk, oss.str().c_str(), 0, &typenodes);
    if (rc != ZOK) {
      std::cout<<"Can not get children for path Producer "<< node<<std::endl;
      exit(-1);
    }
    for (int ii = 0; ii < typenodes.count; ++ii) {
      zookeeperconfig.producers()[node.c_str()].insert(GetZooKeeperNode(string(typenodes.data[ii])));
    }
  }

  // Cache
  struct String_vector namespaces;
  rc = zoo_get_children(zk, "/Tripod/Cache", 0, &namespaces);
  if (rc != ZOK) {
    std::cout<<"Can not get children for path Cache!"<<std::endl;
    exit(-1);
  }
  for (int i = 0; i < namespaces.count; ++i) {
    string ns = string(namespaces.data[i]);
    ostringstream oss;
    oss << "/Tripod/Cache/" << ns;
    struct String_vector nodes;
    rc = zoo_get_children(zk, oss.str().c_str(), 0, &nodes);
    if (rc != ZOK) {
      std::cout<<"Can not get children for path Cache "<<ns<<std::endl;
      exit(-1);
    }
    for (int ii = 0; ii < nodes.count; ++ii) {
      zookeeperconfig.caches()[ns].insert(GetZooKeeperNode(string(nodes.data[ii])));
    }
  }

}

set<Node> DifferenceSet(const set<Node>& a, const set<Node>& b) {
  set<Node> results;
  for (set<Node>::iterator i = a.begin(); i != a.end(); ++i) {
    if (b.count(*i) <= 0) {
      results.insert(*i);
    }
  }
  return results;
}

void Delete() {
  // Manager
  set<Node> managers = DifferenceSet(zookeeperconfig.managers(), xmlconfig.managers());

  // Producer
  map<string, set<Node> > producers;
  for (map<string, set<Node> >::iterator p = zookeeperconfig.producers().begin(); p != zookeeperconfig.producers().end(); ++p) {
    producers[p->first] = DifferenceSet(p->second, xmlconfig.producers()[p->first]);
  }

  // Cache
  map<string, set<Node> > caches;
  for (map<string, set<Node> >::iterator c = zookeeperconfig.caches().begin(); c != zookeeperconfig.caches().end(); ++c) {
    if(xmlconfig.caches().find(c->first) != xmlconfig.caches().end()) {
        caches[c->first] = DifferenceSet(c->second, xmlconfig.caches()[c->first]);
    }else {
        caches[c->first] = c->second;
    }
  }

  int rc = 0;
  if(!managers.empty()) {
      for(set<Node>::iterator it = managers.begin(); it != managers.end(); ++it) {
          rc = zoo_delete(zk, ("/Tripod/Manager/" + const_cast<Node&>(*it).toString()).c_str(), -1);
          if (rc != ZOK) { 
              std::cout<<"Can not delete path Manager "<<const_cast<Node&>(*it).toString()<<std::endl;
              exit(-1);
          }
      }
  }
  
  if(!producers.empty()) {
      for(map<string, set<Node> >::iterator it = producers.begin(); it != producers.end(); ++it) {
          if(!(it->second.empty())) {
              for(set<Node>::iterator iIt = it->second.begin(); iIt != it->second.end(); ++iIt) {
                  rc = zoo_delete(zk, ("/Tripod/Producer/" + it->first + "/" + const_cast<Node&>(*iIt).toString()).c_str(), -1);
                  if (rc != ZOK) { 
                      std::cout<<"Can not delete path Producer "<<it->first<<const_cast<Node&>(*iIt).toString()<<std::endl;
                      exit(-1);
                  }
              }
          }
      }
  }
  
  if(!caches.empty()) {
      for(map<string, set<Node> >::iterator it = caches.begin(); it != caches.end(); ++it) {
          if(!(it->second.empty())) {
              for(set<Node>::iterator iIt = it->second.begin(); iIt != it->second.end(); ++iIt) {
                  rc = zoo_delete(zk, ("/Tripod/Cache/" + it->first + "/" + const_cast<Node&>(*iIt).toString()).c_str(), -1);
                  if (rc != ZOK) { 
                      std::cout<<"Can not delete path cache "<<it->first<<const_cast<Node&>(*iIt).toString()<<std::endl;
                      exit(-1);
                  }
              }
          }
          struct String_vector nodes;
          rc = zoo_get_children(zk, ("/Tripod/Cache/" + it->first).c_str(), 0, &nodes);
          if (rc != ZOK) {
              std::cout<<"Can not get children path Cache "<<it->first<<std::endl;
              exit(-1);
          }
          if(nodes.count == 0) {
              rc = zoo_delete(zk, ("/Tripod/Cache/" + it->first).c_str(), -1);
              if (rc != ZOK) { 
                  std::cout<<"Can not delete path Cache "<<std::endl;
                  exit(-1);
              }
          }
      }
  }
}

void Insert() {
  // Manager
  set<Node> managers = DifferenceSet(xmlconfig.managers(), zookeeperconfig.managers());

  // Producer
  map<string, set<Node> > producers;
  for (map<string, set<Node> >::iterator p = xmlconfig.producers().begin(); p != xmlconfig.producers().end(); ++p) {
    if(zookeeperconfig.producers().find(p->first) != zookeeperconfig.producers().end()) {
        producers[p->first] = DifferenceSet(p->second, zookeeperconfig.producers()[p->first]);
    }else {
        producers[p->first] = p->second;
    }
  }

  // Cache
  map<string, set<Node> > caches;
  for (map<string, set<Node> >::iterator c = xmlconfig.caches().begin(); c != xmlconfig.caches().end(); ++c) {
    if(zookeeperconfig.caches().find(c->first) != zookeeperconfig.caches().end()) {
        caches[c->first] = DifferenceSet(c->second, zookeeperconfig.caches()[c->first]);
    }else{
        caches[c->first] = c->second;
    }
  }

  int rc = 0;
  if(!managers.empty()) {
      for(set<Node>::iterator it = managers.begin(); it != managers.end(); ++it) {
          rc = zoo_create(zk, ("/Tripod/Manager/" + const_cast<Node&>(*it).toString()).c_str(), "", 0, &ZOO_OPEN_ACL_UNSAFE, 0, 0, 0);
          if (rc != ZOK) { 
              std::cout<<"Can not create for path Manager "<<const_cast<Node&>(*it).toString()<<std::endl;
              exit(-1);
          }
      }
  }
  
  if(!producers.empty()) {
      for(map<string, set<Node> >::iterator it = producers.begin(); it != producers.end(); ++it) {
          if(!(it->second.empty())) {
              for(set<Node>::iterator iIt = it->second.begin(); iIt != it->second.end(); ++iIt) {
                  rc = zoo_create(zk, ("/Tripod/Producer/" + it->first + "/" + const_cast<Node&>(*iIt).toString()).c_str(), "", 0, &ZOO_OPEN_ACL_UNSAFE, 0, 0, 0);
                  if (rc != ZOK) { 
                      std::cout<<"Can not create for path Producer "<<it->first<<const_cast<Node&>(*iIt).toString()<<std::endl;
                      exit(-1);
                  }
              }
          }
      }
  }
  
  if(!caches.empty()) {
      for(map<string, set<Node> >::iterator it = caches.begin(); it != caches.end(); ++it) {
          rc = zoo_exists(zk, ("/Tripod/Cache/" + it->first).c_str(), 0, 0);
          if (rc != ZOK) { 
              rc = zoo_create(zk, ("/Tripod/Cache/" + it->first).c_str(), "0", 1, &ZOO_OPEN_ACL_UNSAFE, 0, 0, 0);
              if (rc != ZOK) { 
                  std::cout<<"Can not create for path Cache "<<it->first<<std::endl;
                  exit(-1);
              }
          }

          if(!(it->second.empty())) {
              for(set<Node>::iterator iIt = it->second.begin(); iIt != it->second.end(); ++iIt) {
                  rc = zoo_create(zk, ("/Tripod/Cache/" + it->first + "/" + const_cast<Node&>(*iIt).toString()).c_str(), "", 0, &ZOO_OPEN_ACL_UNSAFE, 0, 0, 0);
                  if (rc != ZOK) { 
                      std::cout<<"Can not create for path Cache "<<it->first<<const_cast<Node&>(*iIt).toString()<<std::endl;
                      exit(-1);
                  }
              }
          }
      }
  }
}

void NotifyNode(const string& path) {
  // Manager
  int version;
  char buffer[1048576];
  int buflen = 1048576;
  int rc = zoo_get(zk, path.c_str(), 0, buffer, & buflen, 0);
  if (rc != ZOK) {
    std::cout<<"Can not get for path "<<path<<std::endl;
    exit(-1);
  }
  version = lexical_cast<int>(string(buffer, buffer + buflen));
  rc = zoo_set(zk, path.c_str(), lexical_cast<string>(version + 1).c_str(), lexical_cast<string>(version + 1).length(), -1);
  if (rc != ZOK) {
    std::cout<<"Can not set for path "<<path<<std::endl;
    exit(-1);
  }
}

void NotifyChildren(const string& path) {
  struct String_vector nodes;
  int rc = zoo_get_children(zk, path.c_str(), 0, &nodes);
  if (rc != ZOK) {
    std::cout<<"Can not get children for path "<<path<<std::endl;
    exit(-1);
  }
  for (int ni = 0; ni < nodes.count; ++ni) {
    ostringstream oss;
    oss << path << "/" << string(nodes.data[ni]);
    NotifyNode(oss.str().c_str());
  }
}

int main(int argc, char *argv[]) {
  if(argc != 3) {
      std::cout<<"We need zkaddress and xml path! correct way to use this is : ./exec zkaddress xmlpath"<<std::endl;
      return 0;
  }
  std::string zkAddress(argv[1]);
  std::string xmlPath(argv[2]);

  zk = zookeeper_init(zkAddress.c_str(), 0, 10000, 0, 0, 0);
  int rc = zoo_create(zk, "/Tripod/mutex", NULL, 0, &ZOO_OPEN_ACL_UNSAFE, ZOO_EPHEMERAL, NULL, 0);
  if (rc != ZOK) {
    std::cout<<"Can not get lock!Someone else is using admin!"<<std::endl;
    exit(-1);
  }

  GetXmlConfig(xmlPath);
  GetZooKeeperConfig();

  // Manager
  Delete();
  Insert();
  NotifyNode("/Tripod/Manager");
  NotifyNode("/Tripod/Producer/cc");
  NotifyNode("/Tripod/Producer/java");
  NotifyChildren("/Tripod/Cache");

  zookeeper_close(zk);
  return 0;
}
