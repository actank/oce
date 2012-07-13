#ifndef __BINSTRING_H__
#define __BINSTRING_H__

#include <string>
#include <SearchCache.h>
#include <IceUtil/IceUtil.h>
#include <ServiceI.h>

using namespace std;

namespace xce {
namespace searchcache {
    class BinString : virtual public Ice::Object {
        enum { DEFAULT_SIZE = 64 };        
        public:
            BinString();
            BinString(char*, size_t);
            ~BinString();
            template<typename T>
                BinString& add(const T value) {
                    try {
                        if (m_nBufferLen + (int)sizeof(T) > m_nBufferCapacity)
                            addBuffer((int)sizeof(T));

                        T *p = (T*)(m_pBuffer+m_nBufferLen);
                        *p = value;
                        m_nBufferLen += (int)sizeof(T);
                    } catch (std::bad_alloc) {
                        //打印日志
                        MCE_WARN("BBBB out of memory ");
                    }
                    return *this;
                }
            BinString& add(const string &str);
            BinString& add(const char *p);
            BinString& add(const BinString &bstr);
            int getBuf (char **p, int *len) const;
            bool empty();
            int getBufferLen() const {return m_nBufferLen;}
            int getBufferCapacity() const {return m_nBufferCapacity;}
        private:
            //不允许copy和复制
            BinString(const BinString&);
            BinString& operator=(const BinString&);
            void addBuffer(size_t len);
            char* m_pBuffer;
            int m_nBufferCapacity;
            int m_nBufferLen;
    };

    typedef IceUtil::Handle<BinString> BinStringPtr;
};
};
#endif
