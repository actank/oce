/*
 * =====================================================================================
 *
 *       Filename:  ShareIndexLogicI.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2010年10月12日 17时56分07秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  lijie (), lijie@opi-corp.com
 *        Company:  Opi-Corp
 *
 * =====================================================================================
 */
#ifndef __SHAREINDEXLOGICI_H__
#define __SHAREINDEXLOGICI_H__

#include <boost/shared_ptr.hpp>
#include "ServiceI.h"
#include "IceUtil/IceUtil.h"
#include "QueryRunner.h"

const int THREAD_NUM = 1;
const int BATCH_URL_SIZE = 1000;

   


class LoadUrlThread : virtual public IceUtil::Thread  {
  public:
    LoadUrlThread(int db_no, std::ofstream & url_set);
    void run();
    int itoa(int i,char* string);
    
  private:
    IceUtil::RWRecMutex _url_set_lock;
    int _db_no;
    std::ofstream & _url_set;
};

typedef std::vector<LoadUrlThread *>  LoadUrlsVec;

class BatchUrlMd5ResultHandlerI: public com::xiaonei::xce::ResultHandler {
  public:
    BatchUrlMd5ResultHandlerI(long& maxUrl, std::ofstream & output);
    virtual bool handle(mysqlpp::Row& row) const;
  private:
    long& _maxUrl;
    //long & _maxUrl;
    std::ofstream & _output;
};



#endif
