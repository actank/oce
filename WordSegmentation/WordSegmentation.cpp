#include "WordSegmentation.h"
#include "GenerateChunk.h"
#include <boost/shared_ptr.hpp>

bool WordSegmentation::RecogSeqNotChara(std::string SingleCharacter)  {   //识别输入的单个字符是否为英文或数字等非中文字符
   if(SingleCharacter.length()==1)  {
     if((SingleCharacter.c_str()[0] & 0x80)==0 )   return true;}
   return false;
}

int WordSegmentation::RecogCharaType(const char * input)  {
    if( (*input>='a'&& *input<='z') || (*input>='A' && *input<='Z') )
        return 1;
    else if(*input>='0'&& *input<='9')  return 2;
    else if(*input=='_'|| *input=='-' || *input==' ' || 
          *input=='.'||*input =='@')  return 3;
    else return 0;
}

bool WordSegmentation::RecogProperNoun(std::string Characters)  {

}

std::string WordSegmentation::FilterChunk(Dict * dictionary, std::string sequence)  {   //分词处理主函数
  std::string result="";  
  int NumCharacters=sequence.length(),ProcessedIndex=0,index;
  boost::shared_ptr<GenerateChunk> ChunkSeq(new GenerateChunk(dictionary));
  if(sequence.length()<=0) return "";
  std::string tmp_sequence=sequence;
  while(ProcessedIndex<NumCharacters)  {
    index = ChunkSeq->CheckDict(tmp_sequence,dictionary);
    result+=sequence.substr(ProcessedIndex,index);
    tmp_sequence=sequence.substr(ProcessedIndex+index,NumCharacters-ProcessedIndex-index);
    result+=" ";
    ProcessedIndex += index;
  }
  return result;
}
  
std::string WordSegmentation::SegmentEngine(std::string Input)  {  //暂不考虑专有名词词典
  std::string CharacterSequence="";
  std::string NotCharacterSequence="";
  std::string FinalSequence="";
  std::string PreChara="";
  std::string CurrentChara="";
  bool Symbol=true;  //默认为中文字符
  for(int i=0;i<Input.length();i++)  {  
    if(RecogSeqNotChara(Input.substr(i,1))) {
        if(i==0)  {
              NotCharacterSequence+=Input.substr(i,1);
              if(RecogCharaType(Input.substr(i,1).c_str())==0) NotCharacterSequence += " ";
              Symbol=false;
         }
        else   {
             PreChara = Input.substr(i-1,1);
             CurrentChara = Input.substr(i,1);
             int type = RecogCharaType(CurrentChara.c_str());
             if( type ==0 )  {
                 NotCharacterSequence+=" ";NotCharacterSequence+=CurrentChara;
                 NotCharacterSequence+=" ";Symbol=false;}
             else  {
                 if(type == 1 && RecogCharaType(PreChara.c_str()) ==2)  {
                   NotCharacterSequence += " ";NotCharacterSequence +=CurrentChara;
                   Symbol = false; 
                }
                else {
                  NotCharacterSequence += CurrentChara; Symbol = false;
                }
             }
        }
    }
    else  { CharacterSequence+=Input[i];Symbol=true;} 
    if( !Symbol && CharacterSequence.length()>0 )  { // std::cerr<<"first chunk!"<<std::endl;
          FinalSequence+=FilterChunk(ProcessDict,CharacterSequence);
          CharacterSequence="";
     } //处理中文字符串
    if( Symbol && NotCharacterSequence.length()>0 )  {
         FinalSequence+=NotCharacterSequence; FinalSequence+=" ";
         NotCharacterSequence="";
    } //处理非中文字符串
  }
  if(CharacterSequence.length()>0) { 
                      FinalSequence+=FilterChunk(ProcessDict,CharacterSequence);}  //处理中文字符串
  if(NotCharacterSequence.length()>0)  {FinalSequence+=NotCharacterSequence;FinalSequence+=" ";}  //处理非中文字符串
  return FinalSequence;
}
    
void WordSegmentation:: ProcessSegment(std::string Input)  {
  SegmentedResult=SegmentEngine(Input);
}
