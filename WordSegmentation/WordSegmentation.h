#include <string>
#include "Dict.h"

class WordSegmentation  {
  public:
    Dict * ProcessDict;
    std::string SegmentedResult;
    WordSegmentation(Dict * DictStr) { ProcessDict=DictStr; }; 
    ~WordSegmentation() {};
    std::string SegmentEngine(std::string Input);
    std::string FilterChunk(Dict * dictionary,std::string sequence);
    void ProcessSegment(std::string Input);
    bool RecogSeqNotChara(std::string SingleCharacter);
     int RecogCharaType(const char * input);
    bool RecogProperNoun(std::string Characters);
};
