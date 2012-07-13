#include <stdlib.h>
#include <string>

#include <boost/thread.hpp>

#include <IceUtil/IceUtil.h>
#include <XceCacheAdapter.h>

using namespace xce;

const int cnt = 80000000;

void get_proc() {
  XceCacheAdapter &adapter = XceCacheAdapter::instance();
  size_t fl = 0;
  size_t tu = 0, emp = 0, hl=0;
  size_t r = 0;
  for (;r< cnt; ++r) {
    if (r % 100000 == 0)
      std::cout << r << " empty value:" << emp << " good value:" <<  tu << " half value:" << hl << std::endl;
    std::string value;
    bool b = adapter.Get(r, &value);
    if (b) {
      if (value.size() == 400)
        tu++;
      else if (value.empty()) {
        emp++;
        std::cout << "empty:"<< r << std::endl;
      } else hl++;
    } else
      fl++;
    //usleep(1000000);
  }
 
  std::cout << r << " empty value:" << emp << " good value:" <<  tu << " half value:" << hl << std::endl;
}

void check_proc(int index) {
  XceCacheAdapter &adapter = XceCacheAdapter::instance();

  int error_items = 0;
  std::string value;

  for (int i=index; i<cnt; i+=8) {
    if (i % 100000 == 99999)
      std::cout << "error count:" << error_items << "\n";
      
    bool b = adapter.Get(i, &value);
    if (!b || *((int*)value.data()) == i)
      ++ error_items;
  }
}

void gets_proc() {
  XceCacheAdapter &adapter = XceCacheAdapter::instance();

  IntSequence uids(100, 0);

  while (true) {
    for(int j = 0; j < 100; ++j)
      uids[j] = rand() % cnt;

    adapter.Gets(uids);
    usleep(10);
  }
}

void remove_proc() {
  XceCacheAdapter &adapter = XceCacheAdapter::instance();

  while (true) {
    adapter.Remove(rand() % cnt);
    usleep(50);
  }
}

void removes_proc() {
  XceCacheAdapter &adapter = XceCacheAdapter::instance();

  IntSequence uids(100, 0);

  while (true) {
    for(int j = 0; j < 100; ++j)
      uids[j] = rand() % cnt;

    adapter.Removes(uids);
    usleep(80);
  }
}

void put_proc(int i) {
  XceCacheAdapter &adapter = XceCacheAdapter::instance();

  char a[400];
  std::string value(400, 'a');
  for (size_t r = i;r< cnt; r+=10) {
    for (int j=0; j<sizeof(a)/sizeof(int); ++j)
      ((int*)a)[j] = r;
    adapter.Put(r, value);
    // usleep(10);
  }
}

void puts_proc(int i) {
  XceCacheAdapter &adapter = XceCacheAdapter::instance();

  StringList values(100, std::string(400, '0'));

  IntSequence uids(100, 0);

  for (size_t r = i;r< cnt; r+=1000) {
    for(int j = 0; j < 100; ++j)
      uids[j] = j*10+r;

    adapter.Puts(uids, values);
    // usleep(10);
  }
}



int main(int argc, char *argv[]) {
  XceCacheAdapter &adapter = XceCacheAdapter::instance();
  if (argc !=2 ) {
    std::cout << "Usage: " << argv[0] << " fill\n"
      << argv[0] << " check\n";
    return 0;
  }

  boost::thread_group g;
  if (0 == strcmp(argv[1], "put")) {
    if (adapter.SetValid(0, true) == false) {
      std::cout << "SetValid is error" << std::endl;
      return -1;
    }
    if (adapter.IsValid(0) == false) {
      std::cout << "isValid1 is error" <<std::endl;
      return -1;
    }
    std::cout << "isValid1 is success" <<std::endl;

    adapter.SetValid(0, false);
    if (adapter.IsValid(0) == true) {
      std::cout << "isValid2 is error" <<std::endl;
      return -1;
    }
    std::cout << "isValid2 is success" <<std::endl;

    //void Puts(const IntSequence & uids, const StringList & values);
    IntSequence k;
    StringList l;
    Int2StrMap ll,  m, n;
    for (int i = 0; i< 100; i++) {
      k.push_back(i);
      l.push_back("aaa");
      ll.insert(std::make_pair(i, "aaa"));
    }
    adapter.Puts(k, l);
    m = adapter.Gets(k);
    if (ll != m) {
      std::cout << "Puts is error" <<std::endl;
      return -1;
    }
    std::cout << "Puts is success "<<std::endl;

    adapter.Removes(k);
    n  = adapter.Gets(k);
    if (!n[0].empty()) {
      std::cout << "removes is error sz =" <<  n[0] <<std::endl;
      return -1;
    }
    std::cout << "removes is success" <<std::endl;

    //g.create_thread(&remove_proc);
    //g.create_thread(&removes_proc);

    g.create_thread(boost::bind(put_proc, 0));
    g.create_thread(boost::bind(put_proc, 1));
    g.create_thread(boost::bind(put_proc, 2));
    g.create_thread(boost::bind(put_proc, 3));
    g.create_thread(boost::bind(put_proc, 4));
    g.create_thread(boost::bind(put_proc, 5));
    g.create_thread(boost::bind(put_proc, 6));
    g.create_thread(boost::bind(put_proc, 7));
    g.create_thread(boost::bind(put_proc, 8));
    g.create_thread(boost::bind(put_proc, 9));
    //   g.create_thread(&puts_proc);

    //    g.create_thread(&get_proc);
    //  g.create_thread(&gets_proc);

  }

  if (0 == strcmp(argv[1], "check")) {
    g.create_thread(boost::bind(check_proc, 1));
    g.create_thread(boost::bind(check_proc, 2));
    g.create_thread(boost::bind(check_proc, 3));
    g.create_thread(boost::bind(check_proc, 4));
    g.create_thread(boost::bind(check_proc, 5));
    g.create_thread(boost::bind(check_proc, 6));
    g.create_thread(boost::bind(check_proc, 7));
    g.create_thread(boost::bind(check_proc, 0));
  } else if (0 == strcmp(argv[1], "puts")) {
    g.create_thread(boost::bind(puts_proc, 1));
    g.create_thread(boost::bind(puts_proc, 2));
    g.create_thread(boost::bind(puts_proc, 3));
    g.create_thread(boost::bind(puts_proc, 4));
    g.create_thread(boost::bind(puts_proc, 5));
    g.create_thread(boost::bind(puts_proc, 6));
    g.create_thread(boost::bind(puts_proc, 7));
    g.create_thread(boost::bind(puts_proc, 8));
    g.create_thread(boost::bind(puts_proc, 9));
    g.create_thread(boost::bind(puts_proc, 0));
  }

  g.join_all();
  return 0;
}
