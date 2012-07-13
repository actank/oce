/*
 * NewsUtil.h
 *
 *  Created on: Dec 2, 2010
 *      Author: yejingwei
 */

#ifndef NEWSUTIL_H_
#define NEWSUTIL_H_

namespace xce {
namespace feed {

class MergeConfig: public MyUtil::Singleton<MergeConfig> {
public:
  /*
   * sort   主类型 评论类型
   *   0    601  2901    日志
   *   0    701  2902    评论单张照片
   *   1    709  2906    评论多张照片(相册)
   *   1    712  2905    评论终端页中的单张照片
   */
  MergeConfig() {
    main2comment_[601] = 2901;
    main2comment_[701] = 2902;
    main2comment_[709] = 2906;
    main2comment_[712] = 2905;
    for (map<Ice::Int, Ice::Int>::iterator mit = main2comment_.begin(); mit
        != main2comment_.end(); ++mit) {
      mains_.insert(mit->first);
      comments_.insert(mit->second);
      comment2main_[mit->second] = mit->first;
    }
    MCE_INFO("MergeConfig::Init done --> main2comment_ size = " << main2comment_.size());
  }
  bool IsMain(Ice::Int stype) {
    return mains_.count(stype);
  }
  bool IsComment(Ice::Int stype) {
    return comments_.count(stype);
  }

  Ice::Int Main2Comment(Ice::Int stype) {
    map<Ice::Int, Ice::Int>::const_iterator mit = main2comment_.find(stype);
    if (mit != main2comment_.end()) {
      return mit->second;
    } else {
      return stype;
    }
  }
  Ice::Int Comment2Main(Ice::Int stype) {
    map<Ice::Int, Ice::Int>::const_iterator mit = comment2main_.find(stype);
    if (mit != comment2main_.end()) {
      return mit->second;
    } else {
      return stype;
    }
  }
  //TODO 将本体放到队列最后
  bool Sort(vector<FeedPtr> & fv, bool & main_with_comment) {
    main_with_comment = false;
    if(fv.size() == 1){
      return false;
    }

    size_t pos = -1, main_cnt = 0,comment_cnt = 0;

    for (size_t i = 0; i != fv.size(); ++i) {
      int stype = fv.at(i)->SmallType();
//      long fid = fv.at(i)->feed_id();
      if (IsComment(stype)) { //comment
        main_with_comment = true;
        ++comment_cnt;
        break;
      } else if (IsMain(stype)) { //本体
        pos = i;
        ++main_cnt;
        continue;
      } else { //普通类型
        return false;
      }
    }
    if (comment_cnt == 0 ||(main_cnt == 1 && pos == fv.size() - 1  ) ) {
      return true;
    }
    vector<FeedPtr> mains;
    for (vector<FeedPtr>::iterator i = fv.begin(); i != fv.end();) {
      int stype = (*i)->SmallType();
      if (IsMain(stype)) {
        mains.push_back(*i);
        i = fv.erase(i);
      } else {
        ++i;
      }
    }

    if (mains.empty()) {
      MCE_WARN("Sorter::sort, mains is empty!!!");
      return false;
    }
    fv.insert(fv.end(), mains.begin(), mains.end());
    return true;
  }

private:
  set<Ice::Int> mains_;//main stype
  set<Ice::Int> comments_;//comment stype
  map<Ice::Int, Ice::Int> main2comment_;
  map<Ice::Int, Ice::Int> comment2main_;
};

class PrintHelper {
public:
  template<typename T>
  static string Print(const T & container) { //将容器内容打印成字符串
    ostringstream ios;
    ios << "(";
    bool first_item = true;
    int i = 0;
    for (typename T::const_iterator itr = container.begin(); itr
        != container.end(); ++itr, ++i) {
      if (first_item == true) {
        first_item = false;
      } else {
        ios << ",";
      }
      ios << *itr;
    }
    ios << ")";
    return ios.str();
  }
};

/*class Sorter{
 public:
 static bool Sort(vector<FeedPtr> & fv,bool & main_with_comment){
 for(size_t i = 0; i != fv.size(); ++i){
 MCE_DEBUG("fid:" << fv.at(i)->feed_id());
 }
 main_with_comment = false;
 size_t pos = 1,main_cnt = 0;

 for(size_t i = 0; i != fv.size(); ++i){
 int stype = fv.at(i)->SmallType();
 long fid =  fv.at(i)->feed_id();
 if( MergeConfig::instance().IsComment(stype)){ //comment
 MCE_DEBUG("comment:" << fid << " stype:" << stype);
 main_with_comment = true;
 break;
 }else if(MergeConfig::instance().IsMain(stype)){ //本体
 MCE_DEBUG("main:" << fid<< " stype:" << stype);
 pos = i;
 ++main_cnt;
 continue;
 }else{ //普通类型
 MCE_DEBUG("normal:" << fid<< " stype:" << stype);
 return false;
 }
 }

 if(pos == fv.size() - 1 && main_cnt == 1){
 MCE_DEBUG("Sorter::Sort, one main,and is the last one");
 return true;
 }
 vector<FeedPtr> mains;
 for(vector<FeedPtr>::iterator i = fv.begin();i != fv.end(); ){
 int stype = (*i)->SmallType();
 if(MergeConfig::instance().IsMain(stype)){
 mains.push_back(*i);
 i = fv.erase(i);
 }else{
 ++i;
 }
 }
 if(mains.empty()){
 MCE_WARN("Sorter::sort, mains is empty!!!");
 return false;
 }
 MCE_DEBUG("Sorter::sort,mains size:" << mains.size());
 fv.insert(fv.end(),mains.begin(),mains.end());
 return true;
 }
 };*/
}
}
#endif /* NEWSUTIL_H_ */
