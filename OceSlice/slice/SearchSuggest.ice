#ifndef  SEARCH_SUGGEST_ICE
#define  SEARCH_SUGGEST_ICE

module com
{
  module xiaonei
  {
     module search
     {
       module suggest
       {
         dictionary<string, string> Str2StrMap;  
         //构造suggest请求的condition,包括userid, 待suggest的query, suggest的type（包括学校，地名，music，好友等信息的suggest,用map形式灵活约定）
         struct SuggestCondition  {
            int userid;
            Str2StrMap queryCondition;
          };

         //为suggest索引增加的词条，拼音及排序的权重
          struct  RecordItem  {
            string record;
            string pinyin;
            double weight;
          };
          sequence<RecordItem> RecordItemSeq;         
          sequence<string> StrSeq;

          interface SuggestManager
          {
            StrSeq getSuggest(SuggestCondition condition, int limit);
            
            void reload(string suggestType);
            
            bool addRecord(string suggestType, RecordItemSeq records);
          };
       };
     };
  };
};
#endif
