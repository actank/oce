#ifndef __TRIPOD_PRODUCERMANAGER_ICE__
#define __TRIPOD_PRODUCERMANAGER_ICE__

#include <TripodData.ice>

module com {
module renren {
module tripod {

interface ProducerManager {
        /*
         * 描述：产生key所对应的数据
         * 参数：keys 需要加载的key，businessId需要使用的factory标识符, useMaster标识是否使用主库加载
         * 返回值：所有加载成功的key，value，如果没有该key对应的数据则map中不含该key,如果需要从多个db加载只要有一个db失败就算该key加载失败
         * 异常类型：基本异常，暂不区分
         */
      com::renren::tripod::DataMap produce(com::renren::tripod::KeySeq keys, string businessId, bool useMaster) throws TripodProducerNotSupportedException;
      
        /*
         * 描述：产生所需要的key列表
         * 参数：beginId 开始的key，limit 产生的数量，businessId需要使用的factory标识符, hotKey是否产生的是hotkey
         * 返回值：产生的所有key
         * 异常类型：基本异常，暂不区分
         */
      com::renren::tripod::KeySeq produceKeys(string beginKey, int limit, string businessId, bool hotKey) throws TripodProducerNotSupportedException;

};

};
};
};
#endif
