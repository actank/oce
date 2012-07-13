#ifndef _USERSCORELOGIN_COUNT_ICE
#define _USERSCORELOGIN_COUNT_ICE
module com{
	module xiaonei {
	   module wservice {
		module userscore {
		 module login {
			interface LoginRecord {
				//增加一次登录
				void incrLoginCount(int userId,byte type);
				//更新服务器的一些配置
				void updateServerState(int item,int value);
		     };
		  };		 
		};
	  };
	};
};
#endif