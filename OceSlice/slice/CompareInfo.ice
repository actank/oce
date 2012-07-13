#ifndef _COMPARE_INFO
#define _COMPARE_INFO

#include <ObjectCache.ice>

module xce
{
module compareinfo
{


    const byte NETWORK = 1;
    const byte HIGHTSCHOOL =2;
    const byte UNIVERSITY =3;
    const byte COLLEGE =4;
    const byte WORKPLACE =5;
    const byte ELEMENTARYSCHOOL=6;
    const byte JUNIORHIGHTSCHOOL=7;

    class CompareInfoData
    {
      int userId;
      MyUtil::LongSeq networkName;
      MyUtil::LongSeq highschoolName;
      MyUtil::LongSeq universityName;
      MyUtil::LongSeq workplaceName;
      MyUtil::LongSeq collegeName;
      long elementarySchoolName;
      long juniorHighShoolName;
    };

   sequence<byte> byteSeq;
 
   class CompareInfoUpdateOldData
   {
     int userId;
     byteSeq table;
   };

   class tableAndvalue
   {
     byte table;
     MyUtil::LongSeq value;
   } ; 
   
   sequence<tableAndvalue> tableAndvalueSeq;

   class CompareInfoUpdateNewData
   {
     int userId;
     tableAndvalueSeq tablevalue;
     
   } ;

   interface CompareInfoReloader
   {
      void reload(MyUtil::IntSeq ids);
      void update(CompareInfoUpdateOldData data);
      void setValid(bool valid);
      bool isValid();
   };



   interface CompareInfoManager
   {
      bool CompareInfo(int userId1,int userId2);
      bool CompareInfoByTable(int userId1,int userId2,byteSeq table);
      void setValid(bool valid);
      void setData(MyUtil::ObjectResult data);
      void update(CompareInfoUpdateNewData userData);
      bool isValid();
      // bool Compare(int userId1,vector<int> userID); 
   };
};
};


#endif
