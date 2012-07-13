#ifndef __GENERAL_SUGGESTION_INDEX_H__
#define __GENERAL_SUGGESTION_INDEX_H__


#include <ServiceI.h>
#include <queue>
static int NO_MORE_DOCS = -1;
namespace xce{
namespace search{
	class DocInfo : public IceUtil::Shared{
	public:
		DocInfo(int id,int postion){
			id_ = id;
			addPostion(postion);
		}
		void addPostion(int postion){
			postions_.push_back(postion);
		}
		int doc(){
			return id_;
		}
		int tf(){
			return postions_.size();
		}
		vector<int> postions(){
			return postions_;
		}
		int id_;
	    std::vector<int> postions_;
	};
	typedef IceUtil::Handle<DocInfo> DocInfoPtr;

	class DocInfoList : public IceUtil::Shared {
	public:
		DocInfoPtr get(int index){
			return list_[index];	
		}
		int size(){
			return list_.size();
		}
		DocInfoPtr back(){
			return list_.back();
		} 
		bool empty(){
			return list_.empty();
		}
		void push(int id, int postion){
			if(list_.empty() || list_.back()->doc() != id){
				DocInfoPtr p = new DocInfo(id,postion);
				list_.push_back(p);
			}else{
				list_.back()->addPostion(postion);
			}
		}
	private:
		std::vector<DocInfoPtr> list_;	
	};
	typedef IceUtil::Handle<DocInfoList> DocInfoListPtr;

	class ScoreDoc {
	public:
		ScoreDoc(int doc,float score):doc_(doc),score_(score){
		}
		int doc_;
		float score_;
	};	
	class Scorer;
	typedef IceUtil::Handle<Scorer> ScorerPtr;

	class Collector : public IceUtil::Shared{
	public:
		Collector(int n):limit_(n){
			collector.reserve(n);
		}
		void collect(int doc,float score){
			if(collector.size() < limit_){
				collector.push_back(doc);
			}
		}
		vector<int> topDocs(){
			return collector;
		}
	private:
		vector<int> collector;
		size_t limit_;
	};
	typedef IceUtil::Handle<Collector> CollectorPtr;

	class Scorer : public IceUtil::Shared{
	public:
		Scorer():doc_(-1),current_(-1){
		}
		Scorer(DocInfoListPtr docs):doc_(-1),docs_(docs), current_(-1){
		}
		virtual void init(){
		}
		virtual void addScorer(ScorerPtr sp){
		}
		virtual void score(CollectorPtr collector){
			while(next()){
				collector->collect(docID(),1.0f);
			}	
		}
		virtual int docID(){
			return doc_;
		}

		virtual bool next(){
			if(++current_ < docs_->size()){
				doc_ = docs_->get(current_)->doc();
				postions_ = docs_->get(current_)->postions();
				pos_ = -1;
				return true;
			}else{
				doc_ = NO_MORE_DOCS;
				return false;
			}
		}
		virtual int tf(){
			return postions_.size();
		}
		virtual int nextPostion(){
			return postions_.at(++pos_);
		}	
		virtual bool skipTo(int target){
			do{
				if(!next())
					return false;
			}while(target > doc_);
			return true;
		}
		int doc_;
		vector<int> postions_;
		int pos_;
	private:
		DocInfoListPtr docs_;
		int current_;

	};

	struct sp_comp{
		bool operator()(const ScorerPtr& lp, const ScorerPtr& rp) const
		{
			return lp->docID() > rp->docID();
		}
	};

	class PrefixScorer : public virtual Scorer {
	public:
		void addScorer(ScorerPtr sp){
			if(sp->next()){
				pq_.push(sp);
			}	
		}

		bool next(){
			ScorerPtr top = NULL;
			postions_.clear();
			if(!pq_.empty()){
				top = pq_.top();
				pq_.pop();
				doc_ = top->docID();
				postions_.insert(postions_.end(),top->postions_.begin(),top->postions_.end());
				if(top->next()){
					pq_.push(top);
				}
				// MCE_INFO("Prefix Top Is "<<doc_);
				while(!pq_.empty()){
					top = pq_.top();
					if(top->docID()==doc_){
						pq_.pop();
						postions_.insert(postions_.end(),top->postions_.begin(),top->postions_.end());
						if(top->next()){
							pq_.push(top);
						}
					}else{
						break;
					}
				}
			}else{	
				doc_ = NO_MORE_DOCS;
				return false;
			}
			pos_ = -1;
			sort(postions_.begin(),postions_.end());
			return true; 
		}
	
	private:
		std::priority_queue<ScorerPtr,std::vector<ScorerPtr>,sp_comp> pq_;
	};

	class PhrasePostions;
	typedef IceUtil::Handle<PhrasePostions> PhrasePostionsPtr;

	class PhrasePostions : public IceUtil::Shared{
	public:
		PhrasePostions(ScorerPtr sp,int offset):sp_(sp),offset_(offset){
		}
		bool next(){
			if(!sp_->next()){
				doc_ = NO_MORE_DOCS;
				return false;
			}else{
				doc_ = sp_->docID();
				postion_ = 0;
				return true;
			}
		}
		bool skipTo(int target){
			if(!sp_->skipTo(target)){
				doc_ = NO_MORE_DOCS;
				return false;
			}
			doc_ = sp_->docID();
			postion_ = 0;
			return true;
		}
		void firstPostion(){
			count_ = sp_->tf();
			nextPostion();
		}
		bool nextPostion(){
			if(count_-- > 0){
				postion_ = sp_->nextPostion() - offset_;
				return true;
			}else{
				return false;
			}	
		}
		ScorerPtr sp_;
		PhrasePostionsPtr next_;	
		int doc_;
		int postion_;
		int offset_;
		int count_;
	};
	struct phrase_comp {  
		bool operator()(const PhrasePostionsPtr& lp, const PhrasePostionsPtr& rp) const   
		{  
			if(lp->doc_ == rp->doc_)
				if(lp->postion_ == rp->postion_)
					return lp->offset_ > rp->offset_;
				else
					return lp->postion_ > rp->postion_;
			else
				return lp->doc_ > rp->doc_;
		}  
	};  
	class PhraseScorer : public virtual Scorer {
	public:
		PhraseScorer():postion_(-1),more_(true),firstTime(true){
		}
		void addScorer(ScorerPtr sp){
			PhrasePostionsPtr pp = new PhrasePostions(sp,++postion_);
			terms_.push_back(pp);
			if(last_){
				last_->next_ = pp;
			} else {
				first_ = pp;
			}
			last_ = pp;
		};
		void init(){
			first_->doc_ = -1;
			for(PhrasePostionsPtr pp = first_; more_ && pp ; pp = pp->next_){
				more_ = pp->next();
			}
			if(more_){
				sort();
			}
		}	
		void sort(){
			pq_ = priority_queue<PhrasePostionsPtr,std::vector<PhrasePostionsPtr>,phrase_comp>();
			for(PhrasePostionsPtr pp = first_; pp; pp = pp->next_){
				pq_.push(pp);
			}
			pqToList();	
		}
		void pqToList(){
			first_ = last_ = NULL;
			while(!pq_.empty()){
				PhrasePostionsPtr pp = pq_.top();
				pq_.pop();
				// cout<<"pop doc:"<<pp->doc_<<" pos:"<<pp->postion_<<" offset:"<<pp->offset_<<endl;
				if(last_){
					last_->next_ = pp;
				}else{
					first_ = pp;
				}
				last_ = pp;
				pp->next_ = NULL;
			}
		}
		void firstToLast(){
			last_->next_ = first_;
			last_ = first_;
			first_ = first_->next_;
			last_->next_ = NULL;
		}
		int docID(){
			return first_->doc_;
		};	
		virtual bool next(){
			if(firstTime){
				init();
				firstTime = false;
			}else if(more_){
				more_ = last_->next();
			}
			if(!doNext()){
				first_->doc_ = NO_MORE_DOCS;
			}
			return first_->doc_ != NO_MORE_DOCS;
		};
		bool skipTo(int target){
			for(PhrasePostionsPtr pp = first_; more_&&pp ; pp = pp->next_){
				more_ = pp->skipTo(target);
			}
			if(more_) {
				sort();
			}
			if(!doNext()){
				first_->doc_ = NO_MORE_DOCS;
			}
			return first_->doc_ != NO_MORE_DOCS;
		}
		bool doNext(){
			while(more_){
				while(more_ && first_->doc_ < last_->doc_){
					more_ = first_->skipTo(last_->doc_);
					firstToLast();
				}
				if(more_){
					int freq = phraseFreq();
					if(freq == 0)
						more_ = last_->next();
					else
						return true;
				}
			}
			return false;
		}
		int phraseFreq(){
			pq_ = priority_queue<PhrasePostionsPtr,std::vector<PhrasePostionsPtr>,phrase_comp>();
			for(PhrasePostionsPtr pp = first_; pp ; pp = pp->next_){
				pp->firstPostion();
				pq_.push(pp);
			}		
			pqToList();
			int freq = 0;
			do{
				while(first_->postion_ < last_->postion_){
					do{
						if(!first_->nextPostion()){
			//				cout<<"break phrase: "<<first_->doc_<<" freq: "<<freq<<endl;
			//				cout<<"first_pos:"<<first_->postion_<<" last_pos:"<<last_->postion_<<endl;
							return freq;
						}
					}while(first_->postion_ < last_->postion_);
					firstToLast();
				}
				freq++;
			}while(last_->nextPostion());
			//cout<<"phrase: "<<first_->doc_<<" freq: "<<freq<<endl;
			//cout<<"first_pos:"<<first_->postion_<<" last_pos:"<<last_->postion_<<endl;
			return freq;
		}
	private:
		int postion_;
		bool more_;
		bool firstTime;
		vector<PhrasePostionsPtr> terms_;
		PhrasePostionsPtr first_,last_;
		std::priority_queue<PhrasePostionsPtr,std::vector<PhrasePostionsPtr>,phrase_comp> pq_;
	};
	typedef IceUtil::Handle<PhraseScorer> PhraseScorerPtr; 	
};
};

#endif
