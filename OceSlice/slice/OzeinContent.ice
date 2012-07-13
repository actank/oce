#ifndef _OZEIN_FILTER_ICE
#define _OZEIN_FILTER_ICE

module com{
        module xiaonei{
                module ozein{
		    		module content{
                        module generation{
								["java:type:java.util.ArrayList"]
								sequence<string>seqStr;
								dictionary<string, seqStr> ContentMap;
								dictionary<string, string> InfoMap;
                                module model{
                                   		struct Result{
                                                int flag;
                                                string content;
                                                string highLight;
                                        };
                                        
                                        struct ResultMore{
                                        		int flag;
                                        		seqStr contentList;
                                        		seqStr hightLightList;
                                        };
                                        
                                        struct Audit{
                                                int type;
                                        		int level;
                                        };
                                         struct Word{
                                                int begin;
                                                int length;
                                                int type;
                                                int level;
                                        };                                       

                                };		
                                ["java:type:java.util.ArrayList"]
                                sequence<model::Word> seqWord;
                                interface Filter{
                                		InfoMap antispamFilter(InfoMap query);
                                		int checkAndFilter(int fromid,int toId,int type,string content);
                                        model::Result checkAndFilterContent(int id, int type, string content);
										model::ResultMore checkAndFilterContentForMore(int id,int type,seqStr content);
                                        ContentMap ozeinContent(ContentMap map);
                                        model::Audit getDiaryAduitType(int id, string content);
                                       	model::Audit getAduitType(int id, int checkType, string content);
                                        seqWord getWordList(int filterType,string content);
                                        string getServerStatus(string key);
                                        string lable(int filterType,string content);
                                        string lableByName(string filterName,string content);
                                        void reloadAllKeyword();
                                        void reloadKeyword(int type);
                                        void reloadKeywordByName(string filterType);
                                        void setSwitch(string key,bool value);
                                        void reload(string key);
                            };
                        };
                };
        };
    };
};
#endif
