#ifndef __SHARE_ICE__
#define __SHARE_ICE__

module com{
module xiaonei{
module wService {
module slice{
	
	exception UGCShareException{
		string msg;
	};

	
	interface UGCShareLogic{
		void createShareToShare(long id,int ownerid,int hostid, string ip,string comment) throws UGCShareException;
		void createInternalShare(long id, int ownerid, int type, int hostid, string ip, string comment) throws UGCShareException;
		void createExternalShare(int hostid,string link,string ip,string comment) throws UGCShareException;
	};
};
};
};
};

#endif
