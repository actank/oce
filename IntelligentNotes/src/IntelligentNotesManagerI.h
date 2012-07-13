#ifndef __INTELLIGENTNOTESMANAGERI_H__
#define __INTELLIGENTNOTESMANAGERI_H__

#include <IntelligentNotes.h>
#include <algorithm>
#include "tx.hpp"
#include "IceExt/src/ServiceI.h"
#include "UtilCxx/src/TaskManager.h"
#include <IceUtil/IceUtil.h>

namespace com {
namespace xiaonei {
namespace services { 
namespace IntelligentNotes {
class IntelligentNotesManagerI: virtual public IntelligentNotesManager,
		public MyUtil::Singleton<IntelligentNotesManagerI> {
public:
        IntelligentNotesManagerI();
        ~IntelligentNotesManagerI();
        std::vector<int> ProcessSequence(const std::string& query, const Ice::Current&);
        std::vector<std::string> GetResults(const Ice::Current&);
        void bootstrap(std::string pinyin_dict_path, std::string characters_pinyin_dict_path,
                       std::string character_dict_path, std::string compact_syb_file_path);
private:
        struct result  {
            int id;
            std::string words_seq;
        };
	int Build();
        void Read();
        std::vector<std::string> * ExpandSearch(std::string query);
        std::vector<std::string> * CommonSearch(std::string query);
        std::vector<int> * Intersection(std::vector<int> *a, std::vector<int> *b);
        std::vector<int> * Union1(std::vector<int> *a, std::vector<int> *b);
        std::vector<int> * Union2(std::vector<int> *a, std::vector<int> *b);
        std::vector<int> * SegmentPinYin(std::string query);
        std::vector<int> * ProcessCharacters(std::string input);
        std::vector<int> * ProcessPinYin(std::string input);
        std::vector<int> * FilterNotSeqPinyin(std::vector<int> * final);
        std::vector<int> * FilterNotSeqCharacter(std::vector<int> *final, std::string seq_hanzi);
        void Normalization(std::vector<int> * final);        //最后结果归一化处理
        std::vector<int> * SortFinalResult(std::vector<int> * final);      //按字符串长短对最终结果排序
        std::string TransLowCharacters(std::string query);
        std::string pinyin_dict;
        std::string characters_pinyin_dict;
        std::string character_dict;
        std::string compact_syb_file;
        std::vector<int> single_syb;
        std::vector<int> single_character;
        std::map<std::string, int> check_syb;
        std::map<std::string, int> check_character;
        std::vector<std::string> word_in_dict;
        std::vector<std::string> sequence_pinyin;
        std::vector<std::string> pinyin_in_query;            //待检索query中对应的每个拼音
        std::vector<int> * pinyin_characters;//???
        std::vector<int> * character_word;//???
        tx_tool::tx syb;//include other header
        std::multimap <int,int> normalize;                   //词典归一化用
        std::vector<int> final_sequence;                     //最后检索的结果
        static bool compare(int a, int b)  {
              return a<b;
        }
        static bool StringCompare(const result& s1, const result& s2)  {
           return (s1.words_seq).length()<(s2.words_seq).length();
        }
        std::vector<std::string> v_expand_search;
        std::vector<std::string> v_common_search;
        std::vector<int> v_intersection;
        std::vector<int> v_union1;
        std::vector<int> v_union2;
        std::vector<int> v_segment_pinyin;
        std::vector<int> Alarm;
};
};
};
};
};
#endif
