#ifndef	__EDM_SENDER_MANAGER__
#define	__EDM_SENDER_MANAGER__
//#include <Util.ice>

module edm{
  interface EdmSenderManager{
    int GetEdmId(int u);
    bool Reload();
        };

};

#endif
