#include <unistd.h>
#include "OpiMMseg.h"


OpiMMseg::OpiMMseg(string configure_file)  {
     ifstream input(configure_file.c_str());
     string line;
     int No=0;
     while(!input.eof())  {
        getline(input,line,'\n');
        if(No==0)  unigram_dict = line;
        else if(No==1) stop_words_dict = line;
        else if(No==2) person_name_dict = line;
        else if(No==3) unigram_dict_index = line;
        else break;
        No++;
     }
     DictForSeg = new Dict(unigram_dict,stop_words_dict,person_name_dict,unigram_dict_index);
     if(access(unigram_dict_index.c_str(),F_OK)==-1)
       DictForSeg->GenerateSequencedDict();
     if(DictForSeg->Construct())  { 
         segment_engine = new WordSegmentation(DictForSeg); 
     }
}

OpiMMseg::~OpiMMseg()
{
   delete segment_engine;
   delete DictForSeg;
}

vector<string>&  OpiMMseg::Segment(const string& query, const string& input_segment_type)  {
   segment_type =input_segment_type;
   segment_engine->SegmentedResult.clear();
   segment_engine->ProcessSegment(query,segment_type);
   return segment_engine->SegmentedResult;
}

void  OpiMMseg::SegmentFile(const string for_segment_file_path, const string input_segment_type, const string written_file_path)  {
   string temp;
   segment_type = input_segment_type;
   if(for_segment_file_path.length()>0)  {
       if(written_file_path.length()<=0)  { cerr<<"Please input the file for writing!"<<endl; return;}
       ifstream input(for_segment_file_path.c_str());
       ofstream output(written_file_path.c_str());
       while(!input.eof())  {
          getline(input,temp,'\n');
          if(temp.length()>0)  {
              vector<string> SegmentedWords=Segment(temp,segment_type);
              if(SegmentedWords.size()>0)  {
                  for(unsigned int i=0;i<SegmentedWords.size();i++)
                      output<<SegmentedWords.at(i)<<" ";
                  output<<endl;
              }
          }
          else
            continue;
       }
   }
   else cerr<<"Please input the file for segmented!";
}


