#include "Dict.h"
#include <fstream>
#include <map.h>

std::string Dict::GetIndex()  {
    return this->index;
}

void  Dict::GenerateSequencedDict()  {
    std::vector<const char *>key;
    std::vector <Darts::DoubleArray:: value_type> value;
    std::istream *is =  new std::ifstream(file.c_str());
    int n=0;
    if  (!*is)  {
       std::cerr << "Cannot Open: "<< file << std::endl;
    }
    std::string line;
    while (std::getline(*is, line))  {
       char *tmp = new char[line.size()+1];
       std::strcpy(tmp, line.c_str());
       key.push_back(tmp);
       n++;
     } 
     std::sort(key.begin(), key.end(),Cmp);
     delete is;
     if(m_da.build( key.size(),&key[0],0,0 )==0)  std::cerr<<"Construct success!"<<std::endl;
     if(m_da.save( index.c_str() ) == 0) std::cerr<<"DoubleArray.lib create success!"<<std::endl;
}
int  Dict::Construct()  {
      if(m_da.open(GetIndex().c_str()))  {
       std::cerr <<"Error: cannot open "<< GetIndex() << "!\n Please input the original dictionary!"<<std::endl;
       return 0;
      }
      int no=0;
      std::string words;
      if(stop_words_file.length()>0)  {
          std::ifstream input(stop_words_file.c_str());
          while(!input.eof())  {
             getline(input,words,'\n');
             StopWordsList.insert(std::pair<std::string,int>(words,no));
             no++;
          }
      }
      no=0;
      if(person_name_file.length()>0)  {
          std::ifstream p_input(person_name_file.c_str());
          while(!p_input.eof())  {
             getline(p_input,words,'\n');
             PersonNameList.insert(std::pair<std::string,int>(words,no));
             no++;
          }
      }    
      return 1;
}
