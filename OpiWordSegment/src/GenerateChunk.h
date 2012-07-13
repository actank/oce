#include "Dict.h"

typedef std::vector<int> words;

class GenerateChunk  {
  public:
   Dict * DictTemp;
   GenerateChunk(Dict *temp);
   ~GenerateChunk();
   std::vector<int> & CheckDict(std::string substrings, Dict * dictionary, std::string type);
  private:
   std::vector<int> SegmentIndexList;
};
