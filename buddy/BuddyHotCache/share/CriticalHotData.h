#ifndef __CRITICAL_HOT_DATA__
#define __CRITICAL_HOT_DATA__

namespace xce{
  namespace buddy{

    class CriticalHotData : 
      virtual public Ice::Object , 
      virtual public IceUtil::Mutex{
        public:
          CriticalHotData(BuddyHotDataPtr data){
            _data = data->hotdatas;
          }
          CriticalHotData(HotDataSeq& data){
            _data = data;
          }
          virtual ~CriticalHotData(){
          }

          void add(HotDataSeq data){
            IceUtil::Mutex::Lock lock(*this);
            _data = data;
          }

          void add(HotDataPtr data){
            IceUtil::Mutex::Lock lock(*this);
            for(HotDataSeq::iterator it = _data.begin();it!=_data.end();++it){
              if(data->hotfriend == (*it)->hotfriend){
                //找到了，postion不变
                data->position = (*it)->position;
                (*it) = data; 
                return;
              }
            }
            //没找到，排到最后
            data->position = _data.back()->position;
            _data.push_back(data);
          }

          void remove(){
            IceUtil::Mutex::Lock lock(*this);
            _data.erase(_data.begin(),_data.end());
          }

          void remove(HotDataPtr data){
            IceUtil::Mutex::Lock lock(*this);
            for(HotDataSeq::iterator it = _data.begin();it!=_data.end();++it){
              if(data->hotfriend == (*it)->hotfriend){
                _data.erase(it);
              }
            }
          }
          HotDataSeq getAll(){
            IceUtil::Mutex::Lock lock(*this);
            return _data;
          }
          MyUtil::IntSeq getInt(){
            IceUtil::Mutex::Lock lock(*this);
            MyUtil::IntSeq result;
            for(HotDataSeq::const_iterator it=_data.begin(); it!=_data.end();++it){
              result.push_back((*it)->hotfriend);
            }
            return result;
          }
          int getSize(){
            IceUtil::Mutex::Lock lock(*this);
            return _data.size();
          } 
        private:
          HotDataSeq _data;
      };
    typedef IceUtil::Handle<CriticalHotData> CriticalHotDataPtr;

  }
}

#endif
