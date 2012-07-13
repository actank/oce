#ifndef __BINSTRING_H__
#define __BINSTRING_H__

#include <string>

using namespace std;

namespace xce {
namespace searchcache {
    class BinString {
        enum { DEFAULT_SIZE = 256 * 1024 };        
        public:
            BinString();
            ~BinString();
            BinString& add(const int i);
            BinString& add(const string &str);
            BinString& add(const char *p);
            int getBuf(char **p, int *len);
            bool empty();
            int getBufferLen() {return m_nBufferLen;}
            int getBufferCapacity() {return m_nBufferCapacity;}
        private:
            //不允许copy和复制
            BinString(const BinString&);
            BinString& operator=(const BinString&);
            void addBuffer();
            char* m_pBuffer;
            int m_nBufferCapacity;
            int m_nBufferLen;
    };
};
};
#endif
