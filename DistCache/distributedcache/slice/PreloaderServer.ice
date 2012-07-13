#ifndef PRELOADER_ICE
#define PRELOADER_ICE

module xce {
module distcache {
module server {

  interface PreloaderServer {
        void preloaderByName(string name);
        void preloaderAll();
  };

};
};
};

#endif
