#include "HotShareBuffer.h"
#include "Config.h"
#include "LogWrapper.h"

using namespace MyUtil;
using namespace std;
using namespace xce::hotshare;

HotShareBuffer::HotShareBuffer()
{
  baseValue = 0;
  maxDBID = -1;
  bottomBufferPos = 0;
  topBufferPos = -1;
  bottomDBIDPos = 0;
  topDBIDPos = -1;

  hotShareDataBuffer = new string[BUFFER_SIZE];
  hotShareIDBuffer = new Ice::Long[BUFFER_SIZE]; 
}

HotShareBuffer::~HotShareBuffer()
{
  delete[] hotShareDataBuffer;
  delete[] hotShareIDBuffer;
}

Ice::Long HotShareBuffer::GetMaxDBID()
{
  return maxDBID;
}

void HotShareBuffer::SetMaxDBID(Ice::Long DBID)
{
  maxDBID = maxDBID<DBID? DBID: maxDBID;
}

Ice::Long HotShareBuffer::GetMaxLocalID()
{
  return topDBIDPos==-1 ? -1: topDBIDPos+baseValue;
}

Ice::Long HotShareBuffer::GetMaxDataBufferID()
{
  return topBufferPos==-1? -1 : topBufferPos+baseValue;  
}

bool HotShareBuffer::IsSameSize()
{
  return topDBIDPos == topBufferPos;
}

bool HotShareBuffer::IsInsertID(Ice::Long dbID)
{
  MCE_WARN("HotShareBuffer::IsInsertID  Message---->   topDBIDPos:"<<topDBIDPos<<"     hotShareIDBuffer[topDBIDPos]:"<<hotShareIDBuffer[topDBIDPos]<<"     dbID:"<<dbID);
  return topDBIDPos==-1 || hotShareIDBuffer[topDBIDPos]<dbID;
}

int HotShareBuffer::AddID(Ice::Long localID, Ice::Long dbID)
{        
  int ret = 0;            
  int tempTopDBIDPos = (topDBIDPos+1)%BUFFER_SIZE;    
                  
  if (tempTopDBIDPos == localID%BUFFER_SIZE)                        
  {                                
    if (tempTopDBIDPos == bottomDBIDPos && topDBIDPos>=0)                                          
      bottomDBIDPos =  (bottomDBIDPos +1)%BUFFER_SIZE;
                                                                              
    hotShareIDBuffer[tempTopDBIDPos] = dbID;                                            
    topDBIDPos = tempTopDBIDPos;

    if (topDBIDPos==0 && bottomDBIDPos==1 )                                                            
      baseValue += BUFFER_SIZE;                                                                                                                      
    ret = tempTopDBIDPos;                                                              
  }                                              
  return ret;                        
}

int HotShareBuffer::AddID(Ice::Long dbID)    
{   
  int tempTopDBIDPos = (topDBIDPos+1)%BUFFER_SIZE;    
  if (tempTopDBIDPos == bottomDBIDPos && topDBIDPos>=0)           
    bottomDBIDPos =  (bottomDBIDPos +1)%BUFFER_SIZE;        
        
  hotShareIDBuffer[tempTopDBIDPos] = dbID;            
  topDBIDPos = tempTopDBIDPos;
                                
  if (topDBIDPos==0 && bottomDBIDPos==1 )                        
    baseValue += BUFFER_SIZE;
                                                                                
  return tempTopDBIDPos;                      
}

  
bool HotShareBuffer::DeleteID(Ice::Long localID)    
{          
  bool ret = false;     
  const int tempTopDBIDPos =  bottomDBIDPos>topDBIDPos?topDBIDPos+BUFFER_SIZE:topDBIDPos;                     
  if (localID<=tempTopDBIDPos+baseValue && localID>=bottomDBIDPos+baseValue)                        
  {                                
    hotShareIDBuffer[(int)(localID%BUFFER_SIZE)] = 0;                                      
    ret = true;                                          
  }                      
  return ret;                        
}


  
bool HotShareBuffer::DoAddXML(string element , int pos)    
{                 
  if (pos <0 || pos>=BUFFER_SIZE)                  
    return false;            
              
  int tempPos = pos<bottomBufferPos? pos+BUFFER_SIZE : pos;                  
  int tempTopPos = (topBufferPos<bottomBufferPos&&topBufferPos>-1)? topBufferPos+BUFFER_SIZE+1 : topBufferPos+1;                                                

  MCE_INFO("HotShareBuffer::DoAddXML   Message----->   pos:"<<pos<<"   tempPos:"<<tempPos<<"   tempTopPos:"<<tempTopPos);
  if (tempPos < tempTopPos)                                
  {
    MCE_INFO("HotShareBuffer::DoAddXML  Message ----->    error_pos:"<<pos<<"  topBufferPos:"<<topBufferPos<<"   bottomBufferPos:"<<bottomBufferPos);          
    return false;                                                  
  }
                                                                                         
  while (tempTopPos <= tempPos)                                    
  {                                               
    if (tempTopPos < tempPos)                                                      
      hotShareDataBuffer[tempTopPos%BUFFER_SIZE] = "";                                                  
    else                                                            
      hotShareDataBuffer[tempTopPos%BUFFER_SIZE] = element;
                                                                                                                              
    if (bottomBufferPos==tempTopPos && topBufferPos>=0)                                                                  
      bottomBufferPos = (bottomBufferPos +1)%BUFFER_SIZE;

    topBufferPos = tempTopPos % BUFFER_SIZE;                                                                                                                                     ++tempTopPos;                                                                              
  }                                     

  MCE_INFO("HotShareBuffer::DoAddXML   Message ----->   add_pos:"<<pos<<"   baseValue:"<<baseValue);  
                                  
  return true;                                                                                  
}


  
bool HotShareBuffer::AddXML(string element ,Ice::Long dbID)    
{   
    
  MCE_INFO("HotShareBuffer::AddXML    Message ----->      dbID:"<<dbID<<"      topDBIDPos:"<<topDBIDPos<<"    hotShareIDBuffer[topDBIDPos]"<<hotShareIDBuffer[topDBIDPos] );
  bool ret =false;  
  if (topDBIDPos==-1 || dbID>hotShareIDBuffer[topDBIDPos])
  {
    ret = DoAddXML(element, (int)(topDBIDPos+1));
    if (ret)
      AddID(dbID);
  }
  else
  {
    int pos = GetDBIDPos(dbID);                                  
    if (pos == -2)                                            
    {                                                    
      MCE_WARN("HotShareBuffer::AddXML  Message ----->     cannot find dbId:"<<dbID); 
      return ret;                                                                  
    }                                        
    ret = DoAddXML(element,pos%BUFFER_SIZE);                                                                                          
  }
  
  return ret;
}

/*
bool HotShareBuffer::AddXML(string element ,Ice::Long dbID)    
{          
  bool ret = false;              
  if (!IsSameSize())                    
  {                            
    int pos = GetDBIDPos(dbID);                                  
    if (pos == -2)                                            
    {                                                    
      MCE_WARN("HotShareBuffer::AddXML  cannot find dbId:"<<dbID); 
      return ret;                                                                  
    }                                        
    ret = DoAddXML(element,pos);                                                                                          
  }                  
  else                            
  {                                
    ret = DoAddXML(element,(int)(topDBIDPos+1));
                                      
    if (ret)                                                
      AddID(dbID);                                                                                      
  }                                            
  return ret;                                                  
}
*/

bool HotShareBuffer::GetHotShare(vector<string> &hsVec, Ice::Long lowID, Ice::Long  highID, size_t size, Ice::Long &retLowID, Ice::Long &retHighID)    
{                   
  Ice::Long maxLocalID = topBufferPos+baseValue;                  
  Ice::Long minLocalID = 0;
                      
  if (bottomBufferPos == 0)                              
    minLocalID = bottomBufferPos+baseValue;                          
  else
    minLocalID = bottomBufferPos+baseValue-BUFFER_SIZE;
                                  
                              
  if (highID<lowID || highID>maxLocalID || lowID<minLocalID )
  {
    MCE_INFO("HotShareBuffer::GetHotShare  Message---->    lowID:"<<lowID<<"    highID:"<<highID<<"   maxLocalID:"<<maxLocalID<<"    minLocalID:"<<minLocalID);
    retLowID = retHighID = 0;
    return false;
  }

  Ice::Long tempID = highID;
  size_t tempSize = 0;
  int dataPos = 0;  
                                            
  retHighID = highID;

  MCE_INFO("HotShareBuffer::GetHotShare  Message---->    lowID:"<<lowID<<"    tempID:"<<tempID<<"   minLocalID:"<<minLocalID<<"    tempSIze:"<<tempSize<<"    size:"<<size);
  while (lowID<=tempID && minLocalID<=tempID && tempSize<size)
  {                                                                
    dataPos = (int)(tempID-baseValue);                                                                  
    if (dataPos < 0)                                      
      dataPos += BUFFER_SIZE;

    hsVec.push_back(hotShareDataBuffer[dataPos]);                                                                                
    --tempID;
    ++tempSize;                                                                                              
  }

  retLowID = tempID + 1;

  return true;     
}

bool HotShareBuffer::GetHotShare(vector<string> &hsVec, Ice::Long localID, size_t size, Ice::Long &retLowID, Ice::Long &retHighID)    
{
  Ice::Long  maxLocalID = topBufferPos+baseValue;  
  Ice::Long  minLocalID = 0;

  if (bottomBufferPos == 0)
    minLocalID = bottomBufferPos+baseValue;
  else
    minLocalID = bottomBufferPos+baseValue-BUFFER_SIZE;
                              
  Ice::Long tempID = maxLocalID;

  size_t tempSize = 0;
                                      
  if (localID>=maxLocalID || localID<minLocalID)
  {
    retLowID = retHighID = 0;
    return false;
  }
  int dataPos = 0;
  retHighID = tempID;

  while (localID<tempID && tempSize<size)
  {                                                                
    dataPos = (int)(tempID-baseValue);
   
    if (dataPos < 0)   
      dataPos += BUFFER_SIZE;

    hsVec.push_back(hotShareDataBuffer[dataPos]);
                                                                                  
    --tempID;
    ++tempSize;
                                                                                              
  }

  retLowID = tempID + 1;

  return true;                                                                   
}
  
int  HotShareBuffer::GetDBIDPos(Ice::Long DBID)    
{                   
  const int tempTopDBIDPos =  bottomDBIDPos>topDBIDPos?topDBIDPos+BUFFER_SIZE:topDBIDPos;   

  int temp_mid  = 0;                  
  int temp_head = bottomDBIDPos;                     
  int temp_rear = tempTopDBIDPos;
                          
  while (temp_head <= temp_rear)                               
  {                                        
    temp_mid = (temp_head+temp_rear)/2;                                              
    if (hotShareIDBuffer[temp_mid%BUFFER_SIZE] > DBID)   
      temp_rear = temp_mid - 1;
    else if  (hotShareIDBuffer[temp_mid%BUFFER_SIZE] < DBID)
      temp_head = temp_mid +1;
    else
      break; 
  }
                            
//  temp_mid = temp_mid%BUFFER_SIZE;
  if (temp_rear < temp_head )
    temp_mid = -2;
                                   
  return temp_mid;
                                       
}

int HotShareBuffer::GetLocalID(Ice::Long DBID, bool &insertID)
{
  MCE_INFO("HotShareBuffer::GetLocalID   Message----->begin DBID:"<<DBID);
  int ret = -1;
  
  if (topDBIDPos==-1 || hotShareIDBuffer[topDBIDPos]<DBID)
  {
    ret = topDBIDPos + baseValue + 1;
    insertID = true;
  }
  else  if (hotShareIDBuffer[topDBIDPos] >= DBID)
  {
    int pos = GetDBIDPos(DBID);
    if (pos != -2)
      ret = pos + baseValue;
    insertID = false;
  }

  MCE_INFO("HotShareBuffer::GetLocalID   Message ---->end  DBID:"<<DBID<<"    insertID:"<<insertID<<"   ret:"<<ret);

  return ret;
}

int HotShareBuffer::GetLocalID(Ice::Long DBID)
{
  MCE_INFO("HotShareBuffer::GetLocalID   DBID:"<<DBID);
  int ret = -1;
  if ( IsInsertID( DBID ) )
  {
    int pos = GetDBIDPos(DBID);
    if (pos != -2)
      ret = pos + baseValue;
  }
  else
    ret = topDBIDPos + baseValue + 1;
  
  MCE_INFO("HotShareBuffer::GetLocalID   ret:"<<ret);
  return ret;
}
