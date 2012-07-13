#ifndef _USERSCORE_ICE
#define _USERSCORE_ICE
module com{
	module xiaonei {
	   module wservice {
		module userscore {
			interface UserScoreLogic {
				//添加积分	
				void addScore(int userId, int type,int count);
				//强制设置用户的积分数
				void setScore(int userId, int score);
				//更新指定用户的缓存
				void updateCache(int userId);
				//更新服务器的一些配置
				void updateServerState(int item,int value);
				//获取积分
				int getUserScore(int userId);
		     };		 
		};
	  };
	};
};
#endif