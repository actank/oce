#ifndef USERAPICACHEDATAI_H_
#define USERAPICACHEDATAI_H_

#include <UserApiCache.h>
#include <QueryRunner.h>

namespace xce {
namespace userapicache {

const int TABLE_NUM = 8;

class UserApiCacheDataI: virtual public UserApiCacheData1 {
public:
	UserApiCacheDataI() : tablenum_(0) {}
	void setUserBornProperties(::mysqlpp::Row& , const char* ,const char* , const char* , const char* , const char* , const char* ); 
	void setUserStageProperties(::mysqlpp::Row& , const char* , const char* ); 
	void setUserConfigProperties(::mysqlpp::Row&, const char* , const char*, const char*);
	void setUserUrlProperties(::mysqlpp::Row&, const char*, const char*, const char*);
	void setUserPassportProperties(::mysqlpp::Row& row, const char* , const char*);
	void setUserNameProperties(::mysqlpp::Row& row, const char* );
	void setUserRightProperties(::mysqlpp::Row& row,  const char* );
	void setUserStateProperties(::mysqlpp::Row& row,  const char*, const char*);

	int getTableNum() {return tablenum_;}

private:
	int tablenum_;

};

typedef ::IceUtil::Handle<UserApiCacheDataI> UserApiCacheDataIPtr;
typedef ::std::vector<UserApiCacheDataIPtr> UserApiCacheDataISeq;

}
}
#endif
