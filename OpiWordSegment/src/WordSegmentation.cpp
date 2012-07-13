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
    else if(*input=='_'|| *input=='-' || /*input==' ' || */
          *input=='.'||*input =='@')  return 3;
    else return 0;
}

bool WordSegmentation::RecogProperNoun(std::string Characters)  {
            return false;
}

std::string WordSegmentation::FilterChunk(Dict * dictionary, std::string sequence,std::string TypeOfSegment)  {   //分词处理主函数
  std::string result="";  
  int NumCharacters=sequence.length(),ProcessedIndex=0;
  std::vector<int> index;
  boost::shared_ptr<GenerateChunk> ChunkSeq(new GenerateChunk(dictionary));
  if(sequence.length()<=0) return "";
  std::string tmp_sequence=sequence;
  while(ProcessedIndex<NumCharacters)  {  
    index = ChunkSeq->CheckDict(tmp_sequence,dictionary,TypeOfSegment);
    if(TypeOfSegment=="max" || TypeOfSegment=="min")  { 
       if(dictionary->stop_words_file.length()>0)  { 
           std::map<std::string,int>::iterator iter=dictionary->StopWordsList.find(sequence.substr(ProcessedIndex,index.at(0)));
           if(iter==dictionary->StopWordsList.end())  {
              result+=sequence.substr(ProcessedIndex,index.at(0));
              SegmentedResult.push_back(sequence.substr(ProcessedIndex,index.at(0)));
              result+=" ";
           }
       }
       else  {
           result+=sequence.substr(ProcessedIndex,index.at(0));
           SegmentedResult.push_back(sequence.substr(ProcessedIndex,index.at(0)));
           result+=" ";
       }
    }
    else if(TypeOfSegment == "min_max")  {
       result+=FilterChunk(dictionary,sequence,"max");
       result+=" ";
       result+=FilterChunk(dictionary,sequence,"min");
       result+=" ";
       return result;
    }
    else if(TypeOfSegment=="mix")  {
       if(dictionary->stop_words_file.length()>0)  {
           if(index.size()==1)  {
              std::map<std::string,int>::iterator iter=dictionary->StopWordsList.find(sequence.substr(ProcessedIndex,index.at(0)));
              if(iter==dictionary->StopWordsList.end())  {
                 result+=sequence.substr(ProcessedIndex,index.at(0));
                 SegmentedResult.push_back(sequence.substr(ProcessedIndex,index.at(0)));
                 result+=" ";
              }
           }
           else if(index.size()==2)  {
              std::map<std::string,int>::iterator iter=dictionary->StopWordsList.find(sequence.substr(ProcessedIndex,index.at(1)));
              if(iter==dictionary->StopWordsList.end())  {
                 result+=sequence.substr(ProcessedIndex,index.at(1));
                 SegmentedResult.push_back(sequence.substr(ProcessedIndex,index.at(1)));
                 result+=" ";
              }
              std::string mix_substring = sequence.substr(ProcessedIndex,index.at(1));
              result+=FilterChunk(dictionary,mix_substring,"min");
              result+=" ";
           }
       }
       else  {
           if(index.size()==1)  {
              result+=sequence.substr(ProcessedIndex,index.at(0));
              SegmentedResult.push_back(sequence.substr(ProcessedIndex,index.at(0)));
              result+=" ";
           }
           else if(index.size()==2)  {
              result+=sequence.substr(ProcessedIndex,index.at(1));
              SegmentedResult.push_back(sequence.substr(ProcessedIndex,index.at(1)));
              result+=" ";
              std::string mix_substring = sequence.substr(ProcessedIndex,index.at(1));
              result+=FilterChunk(dictionary,mix_substring,"min");
              result+=" ";
           }
       }
    }
    if(index.size()==1)
    tmp_sequence=sequence.substr(ProcessedIndex+index.at(0),NumCharacters-ProcessedIndex-index.at(0));
    else if(index.size()==2)
       tmp_sequence=sequence.substr(ProcessedIndex+index.at(1),NumCharacters-ProcessedIndex-index.at(1));
    if(index.size()==1)
       ProcessedIndex += index.at(0);
    else if(index.size()==2)
       ProcessedIndex += index.at(1);
  }
  return result;
}
  
std::string WordSegmentation::SegmentEngine(std::string Input,std::string TypeOfSegment)  {  //暂不考虑专有名词词典
  std::string CharacterSequence="";
  std::string NotCharacterSequence="";
  std::string FinalSequence="";
  std::string PreChara="";
  std::string CurrentChara="";
  bool Symbol=true;  //默认为中文字符
  for(unsigned int i=0;i<Input.length();i++)  {  
    if(RecogSeqNotChara(Input.substr(i,1))) {
        if(i==0)  {
              //NotCharacterSequence+=Input.substr(i,1);
              if(RecogCharaType(Input.substr(i,1).c_str())!=0)  {
                    //SegmentedResult.push_back(NotCharacterSequence);
                    //NotCharacterSequence += " ";
                    NotCharacterSequence+=Input.substr(i,1);
              }
              Symbol=false;
         }
        else   {
             PreChara = Input.substr(i-1,1);
             CurrentChara = Input.substr(i,1);
             int type = RecogCharaType(CurrentChara.c_str());
             if( type ==0 )  {
                 //NotCharacterSequence+=" ";NotCharacterSequence+=CurrentChara;
                 //SegmentedResult.push_back(CurrentChara);
                 //NotCharacterSequence+=" ";
                 SegmentedResult.push_back(NotCharacterSequence);
                 NotCharacterSequence="";
                 Symbol=false;}
             else  {
                 if(type == 1 && RecogCharaType(PreChara.c_str()) ==2)  {
                   //NotCharacterSequence += " ";NotCharacterSequence +=CurrentChara;
                   SegmentedResult.push_back(NotCharacterSequence);
                   NotCharacterSequence=CurrentChara;
                   Symbol = false; 
                }
                else {
                     NotCharacterSequence += CurrentChara; 
                  Symbol = false;
                }
             }
        }
    }
    else  { CharacterSequence+=Input[i];Symbol=true;} 
    if( !Symbol && CharacterSequence.length()>0 )  { // 处理中文字符串
          FinalSequence+=FilterChunk(ProcessDict,CharacterSequence,TypeOfSegment);
          CharacterSequence="";
     } 
    if( Symbol && NotCharacterSequence.length()>0 )  {  //处理非中文字符串
         SegmentedResult.push_back(NotCharacterSequence);
         FinalSequence+=NotCharacterSequence; 
         FinalSequence+=" ";
         NotCharacterSequence="";
    } 
  }
  if(CharacterSequence.length()>0) {       //处理中文字符串
                      FinalSequence+=FilterChunk(ProcessDict,CharacterSequence,TypeOfSegment);}
  if(NotCharacterSequence.length()>0)  {    //处理非中文字符串
                      SegmentedResult.push_back(NotCharacterSequence);
                      FinalSequence+=NotCharacterSequence;
                      FinalSequence+=" ";} 
  return FinalSequence;
}
    
void WordSegmentation:: ProcessSegment(std::string Input,std::string TypeOfSegment)  {
  SegmentEngine(Input,TypeOfSegment);
}
