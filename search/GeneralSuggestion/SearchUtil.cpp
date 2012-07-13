#include "search/GeneralSuggestion/SearchUtil.h"
using namespace xce::search;
int main(){
	DocInfoListPtr list1 = new DocInfoList;
	list1->push(0,1);
	list1->push(2,3);
	list1->push(4,0);
	DocInfoListPtr list2 = new DocInfoList;
	list2->push(0,2);
	list2->push(1,0);
	list2->push(4,1);
	ScorerPtr scorer = new PrefixScorer;
	scorer->addScorer(new Scorer(list1));	
	scorer->addScorer(new Scorer(list2));	
	//scorer->init();
	
	DocInfoListPtr list3 = new DocInfoList;
	list3->push(0,3);
	list3->push(1,1);
	list3->push(2,4);
	ScorerPtr scorer2 = new PhraseScorer;
	scorer2->addScorer(scorer);
	scorer2->addScorer(new Scorer(list3));
	CollectorPtr collector = new Collector(100);
	scorer2->score(collector);
	vector<int> res = collector->topDocs();	
	cout<<"res size: "<<res.size()<<endl;
	for(vector<int>::iterator it=res.begin();it!=res.end();++it){
		cout<<*it<<endl;
	}
	
	//
}
