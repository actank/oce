#ifndef __XFEED_DB_ICE__
#define __XFEED_DB_ICE__
module xfeed
{
module db
{

struct DataNode{
	int nodeId; //node id from 0 to ...
	long space; // ×Ö½Ú
	int rowSize; //×Ö½Ú
	int maxFileSize;
	int curFileId; // node id from 0 to ...
	int curFileIndex;
	int maxFileIndex;
	bool enableWrite;
	bool enableRead;
}; 

class DbManager{
	bool addDataNode(DataNode node);
	bool removeDataNode(int nodeId);
	bool keepAlive(int nodeId);
	
};




};
};

#endif
