#include "Dict.h"

std::string Dict::GetIndex()  {
    return this->index;
}

void  Dict::GenerateSequencedDict()  {
    std::vector<UnigramRecord>m_items;
    std::vector <Darts::DoubleArray:: key_type *> key;
    std::vector <Darts::DoubleArray:: value_type> value;
    std::istream *is =  new std::ifstream(file.c_str());
    int n=0;
    if  (!*is)  {
       std::cerr << "Cannot Open: "<< file << std::endl;
    }
    std::string line;
    while (std::getline(*is, line))  {
       if(n%2)  {
           n++;
            continue;
       } 
       size_t spos = line.find('\t',0);
       size_t epos = line.size();
       std::string kCnt = line.substr(spos+1,epos-spos-1);
       UnigramRecord ur;
       ur.key = line.substr(0,spos);
       ur.count = atoi(kCnt.c_str());
       m_items.push_back(ur);
       n++;
     } 
     std::sort(m_items.begin(), m_items.end(),Cmp);
     UnigramRecord * rec = NULL;
     for(int i=0;i<m_items.size();i++)  {
          if(i>=0 && i<m_items.size())
              rec = &m_items[i];
          else rec = NULL;
          if(rec)  {
             char * ptr = &rec->key[0];
             key.push_back(ptr);
             value.push_back(rec->count);
          }
      }
      m_da.clear();
      delete is;
      if(m_da.build( key.size(),&key[0],0,&value[0] )==0)  std::cerr<<"Construct success!"<<std::endl;
      if(m_da.save( index.c_str() ) == 0) std::cerr<<"DoubleArray.lib create success!"<<std::endl;
}
int  Dict::Construct()  {
      if(m_da.open(GetIndex().c_str()))  {
       std::cerr <<"Error: cannot open "<< GetIndex() << "!\n Please input the original dictionary!"<<std::endl;
       return 0;
      }
      return 1;
}
