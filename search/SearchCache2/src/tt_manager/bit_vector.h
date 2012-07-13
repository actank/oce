#ifndef __BITVECTOR_H__
#define __BITVECTOR_H__

#include <IceUtil/IceUtil.h>
#include <sys/types.h>
#include "UtilCxx/src/LogWrapper.h"

/********************************************************************
  功能描述: 位向量类
  用一个bit来表示一个元素的状态，只能表示两种状态： 0，1。 
  这样，一个char可以表示8个元素的状态。节省内存空间。
  主要对外接口: 
  Get()：获取第n个元素的状态
  Set()：设置第n个元素的状态为1
 *********************************************************************/    
#define USE_LOCK //使用互斥锁
class CBitVector
{
 private:
   /** 可装载的元素的最大数目 */
   int m_dwSize;

   /** 实际装载的元素的数目*/
   int m_dwCount;

   /** 装载位向量数据的字节数组 */
   char* m_pBits;
#ifdef USE_LOCK
   IceUtil::RWRecMutex bits_lock_;
#endif 

 public:
   /********************************************************************
     功能描述: 构造函数. 构造一个能够表示dwSize个元素的位向量
     输入参数: dwSize 位向量表示元素的数目                  
     返回 :   无 
    *********************************************************************/
   CBitVector(int dwSize)
   {
     m_dwCount = -1;
     m_dwSize = (dwSize <= 0) ? 0 : dwSize;

     //实际需要分配的字节数目是dwSize的1/8。因为一个字节可以表示8个状态。
     int dwLen = (m_dwSize >> 3) + 1;
     m_pBits = new char[dwLen];
     for (int i = 0;i < dwLen;i++)
     {
       m_pBits[i] = 0;
     }
#ifdef USE_LOCK
     MCE_DEBUG("[TEMPLE] Use mutex lock in bitmap");
     std::cout << "Use Mutex lock in bitmap" << std::endl;
#else
     MCE_DEBUG("[TEMPLE] DO Not Use mutex lock in bitmap");
     std::cout << "Use NO Mutex lock in bitmap" << std::endl;
#endif
   }

   /** 析构函数。释放占用的空间。 */
   ~CBitVector()
   {
     if (m_pBits)
     {
       delete m_pBits;
       m_pBits = NULL;
     }
   }

   /********************************************************************
     功能描述: 设置第n个元素的状态为1
     输入参数: n  第n个元素                  
     返回 :   无 
    *********************************************************************/
   inline void Set(int n)
   {
     if (n >= m_dwSize || n < 0) {
       std::cout << "Fatal! n is " << n << ", m_dwSize is " << m_dwSize << std::endl;
       return;
     }
     //MCE_DEBUG("[TEMPLE] Step in BitMap::Set, uid is " << (int)n);
#ifdef USE_LOCK
     IceUtil::RWRecMutex::WLock lock(bits_lock_);
#endif
     m_pBits[n >> 3] |= 1 << (n & 7);
     m_dwCount = -1;
   }

   /********************************************************************
     功能描述: 获取第n个元素的状态
     输入参数: n  第n个元素                  
     返回 :   若状态为1，则返回TRUE;若状态为0，则返回FALSE 
    *********************************************************************/
   inline bool Get(int n)
   {
     if (n >= m_dwSize || n < 0) {
       return 0;
     }
     //MCE_DEBUG("[TEMPLE] Step in BitMap::Get, uid is " << (int)n );
#ifdef USE_LOCK
     IceUtil::RWRecMutex::RLock lock(bits_lock_);
#endif
     return (m_pBits[n >> 3] & (1 << (n & 7))) != 0;
   }

   /********************************************************************
     功能描述: 获取从start_pos开始的64个元素的状态
     输入参数: start_pos  起始元素                  
     输出参数: 
     返回 :  遍历的位数，通常情况下为64， 当起始位置为最后63个时，另算
    *********************************************************************/
   inline int GetsValidInNext64Bit(int start_pos, std::vector<int>& valid_ids)
   {
     int return_value = 0;
     if (start_pos < 0 || start_pos > m_dwSize) {
       return return_value;
     }
#ifdef USE_LOCK
     IceUtil::RWRecMutex::RLock lock(bits_lock_);
#endif
     __int64_t temp_int;
     //注意"=="优先级比"^"高!!!
     //最后64位(8个字节), 防止越界
     if (0 == ((start_pos >> 6) ^ (m_dwSize >> 6))) {
       temp_int = (*(__int64_t*)&m_pBits[start_pos >> 6]); 
     } else {
       temp_int = (*(__int64_t*)(&m_pBits[start_pos >> 3])); 
     }

     __int64_t offset = (__int64_t)start_pos & 0x3F;

     for(__int64_t i=offset; i<64; ++i) {
       __int64_t mask = (__int64_t)1 << i;
       if(temp_int & mask) {
         valid_ids.push_back(start_pos + i);
       }
     }

     return (64-offset);
   }


   /********************************************************************
     功能描述: 清除第n个元素的状态为0
     输入参数: n  第n个元素                  
     返回 :   无 
    *********************************************************************/
   inline void Clear(int n)
   {
     if (n >= m_dwSize || n < 0) {
       return ;
     }
#ifdef USE_LOCK
     IceUtil::RWRecMutex::WLock lock(bits_lock_);
#endif
     m_pBits[n >> 3] &= ~(1 << (n & 7));
     m_dwCount = -1;
   }

   inline void Clear()
   {
#ifdef USE_LOCK
     IceUtil::RWRecMutex::WLock lock(bits_lock_);
#endif
     int dwLen = (m_dwSize >> 3) + 1;            
     for (int i = 0;i < dwLen;i++)
     {
       m_pBits[i] = 0;
     }
   }

   /********************************************************************
     功能描述: 获取可装载的元素的最大数目
     输入参数: 无                  
     返回 :   可装载的元素的最大数目
    *********************************************************************/
   inline int Size()
   {
     return m_dwSize;
   }

   /********************************************************************
     功能描述: 获取实际装载的元素的数目
     输入参数: 无                  
     返回 :   实际装载的元素的数目
    *********************************************************************/
   inline int Count()
   {
     MCE_DEBUG("[TEMPLE] Step in BitMap::Count");
#ifdef USE_LOCK
     IceUtil::RWRecMutex::WLock lock(bits_lock_);
#endif
     if (-1 == m_dwCount) 
     {
       char BYTE_COUNTS[] = {	0, 1, 1, 2, 1, 2, 2, 3, 1, 2, 2, 3, 2, 3, 3, 4,
         1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
         1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
         2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
         1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
         2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
         2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
         3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
         1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
         2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
         2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
         3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
         2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
         3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
         3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
         4, 5, 5, 6, 5, 6, 6, 7, 5, 6, 6, 7, 6, 7, 7, 8	};

       int c = 0;
       int dwEnd = (m_dwSize >> 3) + 1;
       for (int i = 0; i < dwEnd; i++)
       {
         c += BYTE_COUNTS[m_pBits[i] & 0xFF];	 
       }
       m_dwCount = c;
     }

     return m_dwCount;
   }

   //保存到文件
   int Save(const char* szpFileName)
   {
#ifdef USE_LOCK
     IceUtil::RWRecMutex::RLock lock(bits_lock_);
#endif
     FILE* fp= fopen(szpFileName,"wb");
     if(!fp)
     {				
       return -1;
     }

     //获取bitmap长度			
     int dwLen = (m_dwSize >> 3)+1 ;
     if (fwrite(m_pBits,sizeof(char),dwLen,fp) != (size_t)dwLen)
     {
       fclose(fp);
       return -2;
     }
     fclose(fp);
     return 0;		
   }

   //加载文件到内存
   int Load(const char* szpFileName)
   {
#ifdef USE_LOCK
     IceUtil::RWRecMutex::WLock lock(bits_lock_);
#endif
     FILE* fp= fopen(szpFileName,"rb");
     if(!fp)
     {				
       return -1;
     }

     //把文件的位置指针移到文件尾
     fseek(fp,0,SEEK_END);
     //获取文件长度
     int dwSize=ftell(fp);
     //读文件存入数组m_pBits
     fseek(fp,0,SEEK_SET);
     if (fread(m_pBits,sizeof(char),dwSize,fp) != (size_t)dwSize)
     {
       fclose(fp);
       return -2;
     }

     fclose(fp);
     return 0;
   }
};
#endif
