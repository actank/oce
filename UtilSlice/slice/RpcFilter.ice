#ifndef RPCFILTER_ICE
#define RPCFILTER_ICE

module MyUtil {
  interface RpcFilter {
    void reload();
    bool isValid();
  };
};

#endif
