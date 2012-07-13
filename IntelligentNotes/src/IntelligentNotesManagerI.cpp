#include "IntelligentNotesManagerI.h"
#include <stdlib.h>
#include <execinfo.h>
#include <QueryRunner.h>
#include <SearchCacheAdapter.h>
#include "XceCache.h"
//#include <glog/logging.h>
#include <fstream>

using namespace com::xiaonei::xce;
using namespace com::xiaonei::services::IntelligentNotes;

namespace MyUtil{
void initialize() {
    ServiceI& service = ServiceI::instance();
    string pinyin_dict_path = service.getCommunicator()->getProperties()->getProperty("IntelligentNotes.PinYinDict");
    string characters_pinyin_dict_path = service.getCommunicator()->getProperties()->getProperty("IntelligentNotes.CharactersPinYinDict");
    string character_dict_path = service.getCommunicator()->getProperties()->getProperty("IntelligentNotes.CharacterDict");
    string compact_syb_file_path = service.getCommunicator()->getProperties()->getProperty("IntelligentNotes.CompactSybFile");	             
    service.getAdapter()->add(&IntelligentNotesManagerI::instance(),service.createIdentity("N", ""));
    IntelligentNotesManagerI::instance().bootstrap(pinyin_dict_path,characters_pinyin_dict_path,character_dict_path,compact_syb_file_path);
    MCE_INFO("Succeed!");
}
}

/*
SearchPersistentCacheManagerI::SearchPersistentCacheManagerI() : _valid(false) {
}*/

void IntelligentNotesManagerI::bootstrap(std::string pinyin_dict_path, std::string characters_pinyin_dict_path,
                                         std::string character_dict_path, std::string compact_syb_file_path) {
  pinyin_dict = pinyin_dict_path;
  characters_pinyin_dict = characters_pinyin_dict_path;
  character_dict = character_dict_path;
  compact_syb_file = compact_syb_file_path;
  std::ifstream PinYinDict(pinyin_dict.c_str());
  std::ifstream CharactersPinYinDict(characters_pinyin_dict.c_str());
  std::ifstream CharacterDict(character_dict.c_str());
  int No=0;
  std::string line;
  while(!PinYinDict.eof())  {  //读入拼音字典
      getline(PinYinDict,line,'\n');
      check_syb.insert(std::pair<std::string, int>(line,No));
      No++;
  }
  pinyin_characters = new vector<int> [No];
  No=0;
  while(!CharacterDict.eof())  {  //读入单个汉字字典
      getline(CharacterDict,line,'\n');
      check_character.insert(std::pair<std::string, int>(line,No));
      No++;
  }
  character_word = new vector<int> [No];
  std::map<std::string, int>::iterator ptr;
  std::string words,pinyin,query="",character;
  std::string each_word_pinyin;
  int spos,epos,final,begin;
  bool symbol;
  std::vector<std::string> same_character;
  std::vector<std::string> same_pinyin;
  No = 0;
  std::string number;
  int cur_No,pre_No,normal_No;
  while(!CharactersPinYinDict.eof())  {  //读入中文词典(包含拼音)
      getline(CharactersPinYinDict,line,'\n');
      begin = line.find("\t");
      number = line.substr(0, begin);
      cur_No = atoi(number.c_str());
      if(cur_No==0)  pre_No = cur_No;
      spos = line.find("\t",begin+1);
      words = line.substr(begin+1, spos-begin-1);
      if(pre_No!=cur_No)  normal_No = No;
      if(cur_No!=0&&pre_No==cur_No )
         normalize.insert(pair<int,int> (No,normal_No));
      word_in_dict.push_back(words);
      same_character.clear();
      symbol = true;
      for(unsigned int i=0;i<words.length()/3;i++)  {
         character = words.substr(i*3,3);
         if(i==0) same_character.push_back(character);  //去除每个中文词条中可能重复的汉字
         else  {
            for(unsigned int j=0;j<same_character.size();j++)  {
               if(character==same_character.at(j))  {
                  symbol =false;
                  break;
               }
            }
         }
         if(symbol)  same_character.push_back(character);
         else  {
            symbol=true;
            continue;
         }
         ptr = check_character.find(character);
         if(ptr != check_character.end() )  {
             character_word[ptr->second].push_back(No);
         }
      }
      spos += 1;
      final = line.rfind(" ");
      each_word_pinyin = "";
      same_pinyin.clear();
      while(spos<final+1)  {
         symbol =true;
         epos = line.find(" ",spos);
         pinyin = line.substr(spos, epos-spos);
         each_word_pinyin += pinyin;
         each_word_pinyin += " ";
         if(same_pinyin.empty())  same_pinyin.push_back(pinyin);  //去除每个中文词条中可能重复的拼音
         else  {
            for(unsigned int j=0;j<same_pinyin.size(); j++)  {
                if(pinyin==same_pinyin.at(j))  {
                  symbol =false;
                  break;
               }
            }
         }
         if(symbol)  same_pinyin.push_back(pinyin);
         else  {
            spos = epos + 1;
            continue;
         }
         ptr = check_syb.find(pinyin);
         pinyin_characters[ptr->second].push_back(No);
         spos =epos + 1;
      }
      sequence_pinyin.push_back(each_word_pinyin);//读入每个中文词条对应的拼音串首字母组成的字符串
      No++;
      pre_No = cur_No;
  }
  std::ifstream compact_file;
  compact_file.open(compact_syb_file.c_str());
  if(!compact_file)  {
      if(Build()==0)  cout<<"Create double-array success!"<<endl;
      else  {  cout<<"create double-array error!"<<endl;  }
  }
  Read();
}

IntelligentNotesManagerI::IntelligentNotesManagerI()
{
}

IntelligentNotesManagerI::~IntelligentNotesManagerI()
{
}

int IntelligentNotesManagerI::Build()
{
   std::vector<std::string> wordList;
   std::ifstream ifs(pinyin_dict.c_str());
   if (!ifs){
       //fprintf(stderr,"cannot open %s\n",pinyin_dict.c_str());
       return -1;
   }
   std::string word;
   while (getline(ifs, word)){
    // handle CR-LF
     if (word.size() > 0 && word[word.size()-1] == '\r'){
       word = word.substr(0, word.size()-1);
     }
     if (word == "") continue;
     wordList.push_back(word);
   }
   if (syb.build(wordList,compact_syb_file.c_str()) == -1){
      //fprintf(stderr, "%s", syb.getErrorLog().c_str());
      //fprintf(stderr, "cannot build index for %s\n",pinyin_dict.c_str());
     return -1;
   }
   //fprintf(stderr, "%s", syb.getResultLog().c_str());
   return 0;
}

void IntelligentNotesManagerI::Read()
{
   if (syb.read(compact_syb_file.c_str()) == -1){//fprintf need to modify
      fprintf(stderr, "%s", syb.getErrorLog().c_str());
      fprintf(stderr,"cannot read index %s\n",compact_syb_file.c_str());
   }
   //fprintf(stderr,"%s", syb.getResultLog().c_str());
}

std::vector<std::string> * IntelligentNotesManagerI::ExpandSearch(std::string query)
{
    v_expand_search.clear();
    std::vector<std::string> *ret= &v_expand_search;
    const tx_tool::uint retNum = syb.expandSearch(query.c_str(),query.size(), *ret ,100);
    bool is_pinyin=false;
    for (std::vector<std::string>::iterator it = ret->begin(); it != ret->end();){
        if((*it) == query)  is_pinyin=true;
        if(it->length()<query.length()) it = ret->erase(it);
        else it++;
    }
    if(is_pinyin)  {
        ret->clear();
        ret->push_back(query);
    }
    //printf("expansionSearch %u\n", ret->size());
    return ret;
}

std::vector<std::string> * IntelligentNotesManagerI::CommonSearch(std::string query)
{
    v_common_search.clear();
    std::vector<std::string> *ret = &v_common_search;
    std::vector<tx_tool::uint> retID;
    const tx_tool::uint retNum = syb.commonPrefixSearch(query.c_str(),query.size(), *ret, retID, 10);
   // printf("commonPrefixSearch %u\n", retNum);
    return ret;
}

std::vector<int> * IntelligentNotesManagerI::Intersection(std::vector<int> * a, std::vector<int> * b)  {
    std::vector<int> *c = new std::vector<int>;
    sort(a->begin(), a->end(),compare);
    sort(b->begin(), b->end(),compare);
    set_intersection(a->begin(),a->end(),b->begin(),b->end(),back_inserter(*c));
    v_intersection.clear();
    for(unsigned int i=0;i<c->size();i++)  v_intersection.push_back(c->at(i));
    c->clear();
    delete c;
    return &v_intersection;
}

vector<int> *  IntelligentNotesManagerI::Union1(std::vector<int> * a, std::vector<int> * b)  {
   std::vector<int> *c= new std::vector<int>;
   sort(a->begin(), a->end(),compare);
   sort(b->begin(), b->end(),compare);
   set_union(a->begin(),a->end(),b->begin(),b->end(),back_inserter(*c));
   v_union1.clear();
   for(unsigned int i=0;i<c->size();i++)  v_union1.push_back(c->at(i));
   c->clear();
   delete c;
   return &v_union1;
}

std::vector<int> *  IntelligentNotesManagerI::Union2(std::vector<int> *a ,std::vector<int> *b)  {
   std::vector<int> *c = new std::vector<int>;
   sort(a->begin(), a->end(),compare);
   sort(b->begin(), b->end(),compare);
   set_union(a->begin(),a->end(),b->begin(),b->end(),back_inserter(*c));
   v_union2.clear();
   for(unsigned int i=0;i<c->size();i++)  v_union2.push_back(c->at(i));
   c->clear();
   delete c;
   return &v_union2;
}

std::string IntelligentNotesManagerI::TransLowCharacters(std::string query)  {
    if(query.length()>0)  {
        char temp = query[0];
        if( (temp>='a' && temp<='z') || (temp>='A' && temp<='Z') )  {
           if(query[0]>='A' && query[0]<='Z') query[0]+=32;
        }
        else query="!";
    }
    else query="";
    return query;
}

std::vector<int> * IntelligentNotesManagerI::SegmentPinYin(std::string query)  {
   int index = 0;
   int length = query.length();
   std::string tmp_query = query;
   std::vector<std::string> * common_ptr;
   v_segment_pinyin.clear();
   std::vector<int> * index_list = &v_segment_pinyin;
   index_list->push_back(index);
   while(index<length)  {
      common_ptr = CommonSearch(tmp_query);
      if(common_ptr->size()>0)  {
          if(common_ptr->at(common_ptr->size()-1) == "xian" && index ==0)  {
               index_list->push_back(2);
               index_list->push_back(4);
               index += 4;
          }
          else  {
               index += common_ptr->at(common_ptr->size()-1).length();
               index_list->push_back(index);
          }
          tmp_query = query.substr(index,length-index);
      }
      else  {
          if(tmp_query[0]=='z' || tmp_query[0]=='c' || tmp_query[0]=='s')  {
              if(tmp_query[1]=='h')  index += 2;
              else  index += 1;
          }
          else  {
              if(tmp_query[0]=='i' || tmp_query[0]=='u' || tmp_query[0]=='v')  {
                       index_list = NULL;
                       break;     
              } 
              else  index += 1;
          }
          index_list->push_back(index);
          tmp_query = query.substr(index,length-index);
      }
   }
   return index_list;
}

std::vector<int> * IntelligentNotesManagerI::ProcessCharacters(std::string input) {  //TODO
   std::map<std::string, int>::iterator ptr;
   if( (ptr = check_character.find(input))!=check_character.end() )  {
       single_character.clear();
       for(unsigned int i=0; i<character_word[ptr->second].size();i++)
           single_character.push_back(character_word[ptr->second].at(i));
       return &single_character;//&character_word[ptr->second];
   }
   return NULL;
}

std::vector<int> * IntelligentNotesManagerI::ProcessPinYin(std::string input)  {
  std::map<std::string, int>::iterator ptr;
  std::string process_pinyin;
  std::vector<int> * index_ptr;
  std::vector<std::string> * index_string;
  std::vector<int> * second_result_index;
  std::vector<int> * final_result_index = NULL;
  pinyin_in_query.clear();
  if( (ptr = check_syb.find(input))!=check_syb.end() && input != "xian")  {
     pinyin_in_query.push_back(input);
     single_syb.clear();
     for(unsigned int i=0; i<pinyin_characters[ptr->second].size();i++)
         single_syb.push_back(pinyin_characters[ptr->second].at(i));
     return &single_syb;//&pinyin_characters[ptr->second];
  }
  else  {
     index_ptr = SegmentPinYin(input);
     for(unsigned int i=0; i<index_ptr->size()-1; i++)  {
        process_pinyin = input.substr(index_ptr->at(i),index_ptr->at(i+1)-index_ptr->at(i));
        pinyin_in_query.push_back(process_pinyin);
        index_string = ExpandSearch(process_pinyin);
        if(index_string->size()<=0 ||index_string->size()>100000)
           return final_result_index;
        if(i==0)  {
           if(index_string->size()>0)  {
               for(unsigned int j=0; j<index_string->size(); j++)  {
                   if((ptr = check_syb.find(index_string->at(j)))!= check_syb.end())  {
                       if(j==0)   final_result_index = &pinyin_characters[ptr->second];
                       else  final_result_index=Union1(final_result_index,&pinyin_characters[ptr->second]);
                   }
               }
           }
        }
        else   {
          if(index_string->size()>0)  {
               for(unsigned int j=0; j<index_string->size(); j++)  {
                   if((ptr = check_syb.find(index_string->at(j)))!= check_syb.end())  {
                        if(j==0)  second_result_index = &pinyin_characters[ptr->second];
                        else  second_result_index=Union2(second_result_index,&pinyin_characters[ptr->second]);
                   }
               }
          }
          final_result_index = Intersection(final_result_index,second_result_index);
       }
    }
    index_ptr->clear();
    index_string->clear();
    return final_result_index;
   }
}

std::vector<int> * IntelligentNotesManagerI::FilterNotSeqPinyin(std::vector<int> * final)  {
    unsigned int count;
    unsigned int j,spos,epos,end;
    std::string pinyin_seq,single_pinyin;
    bool is_seq;
    if(final != NULL)  {
      for(std::vector<int>::iterator iter=final->begin();iter!=final->end();)  {
                count = 0;
                j = 0;
                pinyin_seq = sequence_pinyin.at(*iter);
                spos = 0;
                end = pinyin_seq.rfind(" ");
                is_seq = false;
                while(spos < end)  {
                   epos = pinyin_seq.find(" ",spos);
                   single_pinyin = pinyin_seq.substr(spos,epos-spos);
                   if(spos == 0)  {  //首字母对齐过滤
                      if(single_pinyin.find(pinyin_in_query.at(0))!=0)
                         break;
                   }
                   spos = epos + 1;
                   if(pinyin_in_query.size()>0)  {
                       if(j<pinyin_in_query.size())  {
                              if(single_pinyin.find(pinyin_in_query.at(j))==0)  {
                                    if(j==0 ||is_seq)
                                       count++;
                                    j++;
                                    is_seq = true;
                              }
                              else is_seq = false;
                       }
                   }
               }
               if(count != pinyin_in_query.size())
                   iter =final->erase(iter);
               else iter++;
     }
   }
   return final;
}

std::vector<int> * IntelligentNotesManagerI::SortFinalResult(std::vector<int> * final)  {
  /* struct result  {
       int id;
       std::string words_seq;
   };*/
   //result tmp_result;
   std::vector<result> string_result;
   if(final->size()>0)  {
   for(unsigned int i=0; i< final->size(); i++)  {
      result tmp_result;
      tmp_result.id = final->at(i);
      tmp_result.words_seq = word_in_dict.at(final->at(i));
      string_result.push_back(tmp_result);
   }
   sort(string_result.begin(),string_result.end(),StringCompare);
   /*cout<<"最后结果:"<<string_result.size()<<endl;
   for(unsigned int i=0; i< string_result.size(); i++)
      cout<<string_result.at(i)<<" ";
   cout<<endl;*/
   final->clear();
   for(unsigned int i=0; i<string_result.size(); i++)
      final->push_back(string_result.at(i).id);
   }
   return final;
}


std::vector<int> * IntelligentNotesManagerI::FilterNotSeqCharacter(std::vector<int> * final, std::string seq_hanzi)  {
    if(final!=NULL)  {
       for(std::vector<int>::iterator iter=final->begin();iter!=final->end();)  {
          if( int( (word_in_dict.at(*iter)).find(seq_hanzi) )==-1)
               iter = final->erase(iter);
          else  iter++;
       }
    }
    return final;
}

std::vector<string> IntelligentNotesManagerI::GetResults(const Ice::Current&)  {
    return word_in_dict;
}

void IntelligentNotesManagerI::Normalization(std::vector<int> * final)  {
     int count,index;
     std::multimap<int,int>::iterator it;
     bool is_included = false;
     final_sequence.clear();
     for(unsigned int i=0;i<final->size();i++)  {
         count = normalize.count(final->at(i));
         if( count ==0 )  final_sequence.push_back(final->at(i));
         else  {
           it = normalize.find(final->at(i));
           for(int j=1;j<=count;j++)  {
              index = it->second;
              for(unsigned int k=0;k<final_sequence.size();k++)  {
                 if(index == final_sequence.at(k))  {
                     is_included = true;
                     break;
                 }
              }
              if(!is_included)  final_sequence.push_back(index);
              is_included = false;
              it++;
           }
        }
    }
}

std::vector<int> IntelligentNotesManagerI::ProcessSequence(const std::string & query,const Ice::Current&)  {
    std::string hanzi,pinyin="";
    std::string seq_hanzi="",transed_query;
    bool is_continuous=true;
    //std::vector<std::string> Alarm;
    Alarm.clear();
    Alarm.push_back(-1);
    unsigned int i=0,character_count = 0;
    std::vector<int> * final=NULL, *final_tmp;
    while(i<query.length())  {
      if((query[i]&0x80)!=0x80)  {
         transed_query=TransLowCharacters(query.substr(i,1));
         if(transed_query=="!")  { /*cout<<"Can't find the result!"<<endl;*/ return Alarm; }
         if(is_continuous)
           pinyin+=transed_query;
         else  {
           pinyin=transed_query;
           is_continuous = true;
         }
         i++;
      }
     else  {
        if(pinyin.length()>0)  {
           final_tmp = ProcessPinYin(pinyin);
           if(final_tmp!=NULL)  {
              if(final == NULL)
                 final = final_tmp;
              else
                 final = Intersection(final,final_tmp);
              final = FilterNotSeqPinyin(final);
           }     
           pinyin="";
           seq_hanzi = "";
        }
        hanzi = query.substr(i,3);
        seq_hanzi += hanzi;
        is_continuous =false;
        final_tmp = ProcessCharacters(hanzi);
        if(final_tmp!=NULL)  {
           if(final == NULL)
                final = final_tmp;
           else
                final = Intersection(final,final_tmp);
           final = FilterNotSeqCharacter(final,seq_hanzi);
        }
        character_count++;  
        i+=3;
     }
    }
    if(pinyin.length()>0)  {
        if(character_count)  {     //改bug,后加
            string ini_pinyin,tmp_pinyin,standard_pinyin;
            unsigned int spos,epos,end,count_no;
            for(vector<int>::iterator iter=final->begin();iter!=final->end();)  {
                 standard_pinyin="";
                 spos = 0;
                 count_no = 0;
                 ini_pinyin=sequence_pinyin.at(*iter);
                 end = ini_pinyin.rfind(" ");
                 while(spos<end)  {
                      epos = ini_pinyin.find(" ",spos);
                      count_no++;
                      if(count_no>= character_count+1)  {
                            tmp_pinyin = ini_pinyin.substr(spos,epos-spos);
                            standard_pinyin += tmp_pinyin;
                      }
                      spos = epos+1;
                 }
                 if(count_no>= character_count+1)  {
                     tmp_pinyin = ini_pinyin.substr(spos, ini_pinyin.length()-spos);
                     standard_pinyin += tmp_pinyin;
                 }
                 if(standard_pinyin.find(pinyin)!=0)  iter = final ->erase(iter);
                 else iter++;
            }
        }
        else  {
          final_tmp = ProcessPinYin(pinyin);  //改bug,后加
          if(final_tmp)  {
             if(final == NULL)
                final = final_tmp;
             else
               final = Intersection(final,final_tmp);
             final = FilterNotSeqPinyin(final);
          }
          if(query == "xian")  {
              map<string, int>::iterator ptr;
              ptr = check_syb.find(query);
              for(unsigned int i=0; i<pinyin_characters[ptr->second].size(); i++)  {
                  if(sequence_pinyin.at(pinyin_characters[ptr->second].at(i)).find(query)==0)
                     final->push_back(pinyin_characters[ptr->second].at(i));
              }
          }
      }
    }
   if(final != NULL)  {
      if(final->size()>0 && final->size()<100000)  {
    /* cout<<"The final results:"<<final->size()<<endl;
     for(int i=0; i<final->size(); i++)
       cout<<word_in_dict.at(final->at(i))<<" ";
     cout<<endl;*/
         Normalization(final);
         SortFinalResult(&final_sequence);
        // return *final;
        return final_sequence;
      }
      else  {/*final=NULL;*//*cout<<"Can't find the result!"<<endl;*/ return Alarm;}
   }
   else return Alarm;
}
