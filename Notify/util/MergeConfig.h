/*
 * MergeConfig.h
 *
 *  Created on: Nov 25, 2010
 *      Author: yejingwei
 */

#ifndef _XCE_NOTIFY_MERGE_CONFIG_H_
#define _XCE_NOTIFY_MERGE_CONFIG_H_

#include "Singleton.h"
#include <set>
using namespace std;
namespace xce {
namespace notify {

using namespace MyUtil;
using namespace Ice;

class MergeConfig: public MyUtil::Singleton<MergeConfig> {
public:
  void Init() {
    //数组types[]中的是schemaid，实际只要保存业务类型即可，业务类型为
    //{14,15,16,17,18,19,20,21,95}
    Int types[] = {14,15,16,17,18,19,20,21,58,59,77,95,128,142,146,147,148,149,150,153};
    for(size_t i = 0 ;i != sizeof(types)/sizeof(types[0]);++i){
      tv_ref_.insert(make_pair(types[i],0));
      tv_ref_.insert(make_pair(types[i],1));
      t_ref_.insert(types[i]);
    }
    v_ref_.insert(0);
    v_ref_.insert(1);


    MCE_INFO("MergeConfig init done. tv_ref size:" << tv_ref_.size()
        << ",v_ref_ size:" << v_ref_.size() << ",t_rev_size:" << t_ref_.size());

    for (set<pair<Int, Int> >::iterator i = tv_ref_.begin(); i != tv_ref_.end(); ++i) {
      MCE_DEBUG("MergeConfig. type-view:" << i->first << "-" << i->second);
    }

  }
  bool MustMergeView(Int view) {
    return v_ref_.count(view);
  }
  bool MustMergeType(Int type) {
    return t_ref_.count(type);
  }
  bool MustMerge(Int type, Int view) {
    return tv_ref_.count(make_pair(type, view));
  }

private:
  set<pair<Int, Int> > tv_ref_;//需要merge的type和view
  set<Int> v_ref_;//需要merge的view
  set<Int> t_ref_;
};

}
}

#endif /* MERGECONFIG_H_ */
