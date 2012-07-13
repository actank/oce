#include "BinString.h"
#include <string.h>

using namespace xce::searchcache;

BinString::BinString() {
    m_pBuffer = new char[DEFAULT_SIZE];
    m_nBufferLen = 0;
    m_nBufferCapacity = DEFAULT_SIZE;
}

BinString::~BinString() {
    delete m_pBuffer;
    m_nBufferLen = 0;
    m_nBufferCapacity = 0;
}

BinString& BinString::add(const int i) {
    try {
		if (m_nBufferLen + sizeof(int) > m_nBufferCapacity)
			addBuffer();

        int *p = (int*)(m_pBuffer+m_nBufferLen);
        *p = i;
        m_nBufferLen += sizeof(int);
    } catch (std::bad_alloc) {
        //打印日志
    }
    return *this;
}

BinString& BinString::add(const string& str) {
    if (str.empty()) {
        add(0);
        return *this;
    }

    try {
        if (m_nBufferLen + str.length() > m_nBufferCapacity)
            addBuffer();

        add(str.length());

        int len = str.length();

        memcpy(m_pBuffer+m_nBufferLen, str.c_str(), len); 

        if (len > 0)
            m_nBufferLen += len;
        else
            throw("Error");
    } catch (std::bad_alloc) {
        //打印日志
    }
    return *this;
}

BinString& BinString::add(const char* p) {
    if (!p) {
        add(0);
        return *this;
    }

    try {
        if (m_nBufferLen + strlen(p) > m_nBufferCapacity)
            addBuffer();

        add(strlen(p));

        int len = strlen(p);

        memcpy(m_pBuffer+m_nBufferLen, p, len); 

        if (len > 0)
            m_nBufferLen += len;
        else
            throw("Error");
    } catch (std::bad_alloc) {
        //打印日志
    }
    return *this;
}

int BinString::getBuf(char **p, int *len) {
    *p = m_pBuffer;
    *len = m_nBufferLen;
	return *len;
}

void BinString::addBuffer() {
    int ncapacity = 2*m_nBufferCapacity;
    char* p = new char[ncapacity];
    memcpy(p, m_pBuffer, m_nBufferLen);
    delete m_pBuffer;
    m_pBuffer = p;
    m_nBufferCapacity = ncapacity;
}

bool BinString::empty() {
    return m_nBufferLen == 0;
}
