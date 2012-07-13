#include "Dict.h"

typedef std::vector<int> words;

class GenerateChunk  {
  public:
   Dict * DictTemp;
   GenerateChunk(Dict *temp);
   ~GenerateChunk();
   int CheckDict(std::string substrings, Dict * dictionary);
};
