#ifndef LOGIC_MANAGER_H
#define LOGIC_MANAGER_H

class LogicManager{
public:
	void		put(const FeedItem& feed, const Int2IntMap& userWeight);
	FeedItemSeq	get(int userId, int begin, int count);
	void		remove(int userId, Ice::Long feedId);
	void		removeAll(int userId);
private:
	
};

#endif
