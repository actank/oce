#ifndef _SEARCHCACHEDATASERIALIZE_H_
#define _SEARCHCACHEDATASERIALIZE_H_
namespace xce {
namespace searchcache {
class SearchCacheDataSerialize {
    public:
    template<typename T>
        static inline bool UnserializeFromBuffer(const char** p, int& total, T& value) {
            value = *(T*)(*p);
            *p+=sizeof(T);
            total -= sizeof(T);
            if (total < 0) { 
                MCE_WARN("UnserializeFromBuffer is Error! total<0"); 
                    return false;
            }
            return true;
        }

   static inline bool UnserializeFromBuffer(const char** p, int& total, string& value) {
        int strlen = *(int*)(*p);
        *p+=sizeof(int);
        total -= sizeof(int);
        if (total < 0 || strlen < 0) { 
            MCE_WARN("total=" << total << "strlen =" << strlen); 
            return false;
        }
        total -= strlen;
        if (total < 0) { 
            MCE_WARN("total is:"<<total<<"\t"<<"value is:"<<value);
            MCE_WARN("total < 0"); 
            return false;
        }
        value = string(*p, strlen);
        *p+=strlen;
        return true;
    }

    static inline void SerializeToBinString(BinStringPtr& binString, char type, 
            int content, int& totalField){
        binString->add(type); //标签
        binString->add((int)sizeof(content)); //长度
        binString->add(content); //内容
        totalField++;
    }

    static inline void SerializeToBinString(BinStringPtr& binString, char type, 
            const std::string& content, int& totalField){
        if (content.empty()) return;
        binString->add(type); //标签
        binString->add((int)(sizeof(int) + content.length())); //长度
        binString->add((int)content.length());
        binString->add(content);//内容
        totalField++;
    }

    static inline void SerializeToBinString(BinStringPtr& binString, char type, 
            int total, int vec_size, BinString& content, int& totalField) {
            binString->add(type);
            binString->add(total);
            binString->add(vec_size);
            binString->add(content);
            totalField++;
    }
};
}
}
#endif
