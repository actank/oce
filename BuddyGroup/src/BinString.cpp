#include "BinString.h"
#include <string.h>
#include <util/cpp/String.h>

using namespace xce::binstring;

BinString::BinString(char* buf, size_t size) {
  if (buf != NULL && size>0) {
    m_pBuffer = new char[size];
    m_nBufferLen = size;
    m_nBufferCapacity = size;
    memcpy(m_pBuffer, buf, size);
  }
  else {
    m_pBuffer = new char[1];
    m_nBufferLen = 0;
    m_nBufferCapacity = 1;
  }
}

BinString::BinString() {
  m_pBuffer = new char[DEFAULT_SIZE];
  m_nBufferLen = 0;
  m_nBufferCapacity = DEFAULT_SIZE;
}

BinString::~BinString() {
  delete m_pBuffer;
  m_nBufferLen = 0;
  m_nBufferCapacity = 0;
  m_pBuffer = NULL;
}

BinString& BinString::add(const string& str) {
  try {
    add((int)str.length());

    if (str.empty()) 
    {
      return *this;
    }

    if (m_nBufferLen + (int)str.length() > m_nBufferCapacity)
      addBuffer(str.length());

    memcpy(m_pBuffer+m_nBufferLen, str.c_str(), str.length()); 

    m_nBufferLen +=  str.length();
  } catch (std::bad_alloc) {
    MCE_WARN("BBBB out of memory ");
    //打印日志
  }
  return *this;
}

BinString& BinString::add(const BinString& bstr) {
  if (bstr.getBufferLen() == 0) {
    return *this;
  }

  try {
    char*p;
    int len = 0;
    bstr.getBuf(&p, &len);

    if (m_nBufferLen + len > m_nBufferCapacity)
      addBuffer(len);

    memcpy(m_pBuffer+m_nBufferLen, p, len); 
    m_nBufferLen += len;
  } catch (std::bad_alloc) {
    //打印日志
    MCE_WARN("BBBB out of memory ");
  }
  return *this;
}

int BinString::getBuf(char **p, int *len) const {
  *p = m_pBuffer;
  *len = m_nBufferLen;
  return *len;
}

void BinString::compress()
{
  string sourceStr(m_pBuffer,m_nBufferLen);

  string desStr = ::MyUtil::StrUtil::gzip_compress(sourceStr);

  try {
    char* p = new char[desStr.length()];

    memcpy(p, desStr.c_str(), desStr.length());

    delete m_pBuffer;

    m_pBuffer = p;

    m_nBufferLen = desStr.length();

  } catch (std::bad_alloc) {
    MCE_WARN("BBBB out of memory ");
    //打印日志
  }

}

void BinString::unCompress(char** p, int* len)
{
  string sourceStr(m_pBuffer,m_nBufferLen);

  string desStr = ::MyUtil::StrUtil::gzip_uncompress(sourceStr);

  try {
    *p = new char[desStr.length()];

    memcpy(*p, desStr.c_str(), desStr.length());

    *len = desStr.length();

  } catch (std::bad_alloc) {
    MCE_WARN("BBBB out of memory ");
    //打印日志
  }
}

string BinString::unCompress()
{
  string sourceStr(m_pBuffer,m_nBufferLen);
  return ::MyUtil::StrUtil::gzip_uncompress(sourceStr);
}

void BinString::addBuffer(size_t len) {
  int ncapacity = 0;

  if ((int)len < DEFAULT_SIZE)
    ncapacity = m_nBufferLen + DEFAULT_SIZE;
  else
    ncapacity = m_nBufferLen + len;

  char* p = new char[ncapacity];
  memcpy(p, m_pBuffer, m_nBufferLen);
  delete m_pBuffer;
  m_pBuffer = p;
  m_nBufferCapacity = ncapacity;
}

bool BinString::empty() {
  return m_nBufferLen == 0;
}
