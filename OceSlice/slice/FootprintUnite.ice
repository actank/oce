#ifndef FOOTPRINT_UNITE
#define FOOTPRINT_UNITE

#include <Util.ice>
#include <ObjectCache.ice>

module xce {
module footprint {

  class UserVisitCount {
    int userId;
    int count;
  };

	class BaseStep {
		int visitor;
		int timestamp;
	};
	sequence<BaseStep> BaseStepSeq;

	class HomeStep extends BaseStep {
		long gid;
	};
	sequence<HomeStep> HomeStepSeq;

  class HomeVisitHistory {
    long owner;
    HomeStepSeq footprints;
  };

	class BaseInfo{
	};

	class UgcInfo extends BaseInfo {
		BaseStepSeq steps;
	}; 

	class CommonInfo extends BaseInfo {
		BaseStepSeq steps;
	}; 

	class HomeInfo extends BaseInfo {
		HomeStepSeq steps;
		int visitcount;
	};

	class FootprintInfo extends HomeStep {
		int owner;
	};
	sequence<FootprintInfo> FootprintInfoSeq;

	class IpFootprintInfo {
		long gid;
		int owner;
		string visitorIp;
		int timestamp;
	};
	sequence<IpFootprintInfo> IpFootprintInfoSeq;

	/*-----------------------------------
	 *
	 * 	服务接口
	 *
	 ------------------------------------*/
	class UgcFootprintCache {
		void visit(FootprintInfo fpt);
		void visitBatch(FootprintInfoSeq fpts);
		void remove(FootprintInfo fpt);
		int getSize(long id);
		UgcInfo getAll(long id,int begin,int nLimit,int tLimit);
	
		bool isValid();
		void setValid(bool newState);
		void contral(MyUtil::Str2StrMap cmd);

	};
 
        // ----- this CommonFootprintCache  interface is to replace the UgcFootprintCache
	class CommonFootprintCache {
		void visit(FootprintInfo fpt);
		void visitBatch(FootprintInfoSeq fpts);
		void remove(FootprintInfo fpt);
		int getSize(long id);
		CommonInfo getAll(long id,int begin,int nLimit,int tLimit);
	
		bool isValid();
		void setValid(bool newState);
		void contral(MyUtil::Str2StrMap cmd);

	};

  class HomeFootprintReader {
    void setData(MyUtil::ObjectResult homeFootprintResult);
    void setValid(bool valid);
    bool isValid();
  };

  class HomeFootprintBufReader {
    void setData(MyUtil::ObjectResult homeFootprintBufResult);
    void setValid(bool valid);
    bool isValid();
  };

  class HomeUserVisitCountReader {
    void setData(MyUtil::ObjectResult homeUserVisitCountResult);
    void setValid(bool valid);
    bool isValid();
  };

	class HomeFootprintCache {
    HomeVisitHistory getHomeFootprintObj(long id, int category);
    UserVisitCount getUserVisitCountObj(int id);    
		void visit(FootprintInfo fpt);
		void visitBatch(FootprintInfoSeq fpts);
		void remove(FootprintInfo fpt);
		int getSize(long id);
		HomeInfo getAll(long id,int begin,int nLimit,int tLimit);
		
		HomeInfo visitAndGet(FootprintInfo fpt,int begin,int nLimit,int tLimit);	
		int getUserVisitCount(long uid);
		void setUserVisitCount(long uid,int count);
		void incUserVisitCount(long uid);
		void incUserVisitCountBatch(MyUtil::LongSeq uids,int count);
		
		bool isValid();
		void setValid(bool newState);
		void contral(MyUtil::Str2StrMap cmd);
	};

	class HomeFootprintReplica {
    void setFootprint(MyUtil::ObjectResult pData, int category);
    void setUserVisitCount(MyUtil::ObjectResult pData);
		HomeInfo getAll(long id,int begin,int nLimit,int tLimit);
		int getUserVisitCount(long uid);
		int getSize(long id);

		bool isValid();
		void setValid(bool newState);
	};

	class FootprintFacadeManager    {
		HomeInfo visitAndGet(FootprintInfo fpt);
		void visit(FootprintInfo fpt);
		void visitBatch(FootprintInfoSeq fpts);
		void contral(MyUtil::Str2IntMap cmd);

		void interview(FootprintInfo fpt);
		void removeAllByVisitor(int visitor);

		//提供给小小站使用，其他使用为非法(guanghe.ge 2011-4-6)
		void visitByIp(IpFootprintInfo fpt);
    	};
};
};

#endif
