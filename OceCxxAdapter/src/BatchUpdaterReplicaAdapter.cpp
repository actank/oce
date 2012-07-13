#include "boost/lexical_cast.hpp"
#include <iostream>
#include "OceCxxAdapter/src/BatchUpdaterReplicaAdapter.h"

using namespace mop::hi::svc::adapter;
using namespace std;
using namespace xce::clusterstate;

//---------------------------------------------------------------------------

BatchUpdaterReplicaAdapter::BatchUpdaterReplicaAdapter() :
	_clientCS("ControllerBatchUpdater", 120, 300) {
	
	_clientCS.intialize();
	_cluster = _clientCS.getCluster();
	
}

void BatchUpdaterReplicaAdapter::inc(const MyUtil::IntSeq& keyvalue, ::Ice::Int count,
		const std::string& type) {
	try {
		_clientCS.getProxy(INDEX)->inc(keyvalue, count, type);
	} catch (Ice::Exception& e){
		MCE_INFO("[BatchUpdaterReplicaAdapter::inc] Ice::Exception : " << e.what());
	} catch (std::exception& e){
		MCE_INFO("[BatchUpdaterReplicaAdapter::inc] std::exception : " << e.what());
	} catch (...){
		MCE_WARN("[BatchUpdaterReplicaAdapter::inc] exception");
	}
}

void BatchUpdaterReplicaAdapter::inc(const MyUtil::LongSeq& keyvalue, ::Ice::Int count,
                        const std::string& type){
 	try {
                _clientCS.getProxy(INDEX)->incLong(keyvalue, count, type);
        } catch (Ice::Exception& e){
                MCE_INFO("[BatchUpdaterReplicaAdapter::inc] Ice::Exception : " << e.what());
        } catch (std::exception& e){
                MCE_INFO("[BatchUpdaterReplicaAdapter::inc] std::exception : " << e.what());
        } catch (...){
                MCE_WARN("[BatchUpdaterReplicaAdapter::inc] exception");
        }

}


void BatchUpdaterReplicaAdapter::dec(const MyUtil::IntSeq& keyvalue, ::Ice::Int count,
		const std::string& type) {
	try {
		_clientCS.getProxy(INDEX)->dec(keyvalue, count, type);
	} catch (Ice::Exception& e){
		MCE_INFO("[BatchUpdaterReplicaAdapter::dec] Ice::Exception : " << e.what());
	} catch (std::exception& e){
		MCE_INFO("[BatchUpdaterReplicaAdapter::dec] std::exception : " << e.what());
	} catch (...){
		MCE_WARN("[BatchUpdaterReplicaAdapter::dec] exception");
	}
}

void BatchUpdaterReplicaAdapter::dec(const MyUtil::LongSeq& keyvalue, ::Ice::Int count,
                         const std::string& type){
	try {
                 _clientCS.getProxy(INDEX)->decLong(keyvalue, count, type);
         } catch (Ice::Exception& e){
                 MCE_INFO("[BatchUpdaterReplicaAdapter::dec] Ice::Exception : " << e.what());
         } catch (std::exception& e){
                 MCE_INFO("[BatchUpdaterReplicaAdapter::dec] std::exception : " << e.what());
         } catch (...){
                 MCE_WARN("[BatchUpdaterReplicaAdapter::dec] exception");
         }
	
}

void BatchUpdaterReplicaAdapter::incUserViewCount(int id, int count) {
	try {	
		_clientCS.getProxy(INDEX)->incUserViewCount(id, count);
	} catch (Ice::Exception& e){
		MCE_INFO("[BatchUpdaterReplicaAdapter::incUserViewCount] Ice::Exception : " << e.what());
	} catch (std::exception& e){
		MCE_INFO("[BatchUpdaterReplicaAdapter::incUserViewCount] std::exception : " << e.what());
	} catch (...){
		MCE_WARN("[BatchUpdaterReplicaAdapter::incUserViewCount] exception");
	}
}

void BatchUpdaterReplicaAdapter::incUserViewCount(long id, int count){
	try {
		MyUtil::LongSeq keyvalue;
		keyvalue.push_back(id);
		std::string type("USERVIEWCOUNTER");
		_clientCS.getProxy(INDEX)->incLong(keyvalue, count, type);
         } catch (Ice::Exception& e){
                 MCE_INFO("[BatchUpdaterReplicaAdapter::incUserViewCount] Ice::Exception : " << e.what());
         } catch (std::exception& e){
                 MCE_INFO("[BatchUpdaterReplicaAdapter::incUserViewCount] std::exception : " << e.what());
         } catch (...){
                 MCE_WARN("[BatchUpdaterReplicaAdapter::incUserViewCount] exception");
         }

}

void BatchUpdaterReplicaAdapter::incLoginCount(int id, int count) {
	try {
		_clientCS.getProxy(INDEX)->incLoginCount(id, count);
	} catch (Ice::Exception& e){
		MCE_INFO("[BatchUpdaterReplicaAdapter::incLoginCount] Ice::Exception : " << e.what());
	} catch (std::exception& e){
		MCE_INFO("[BatchUpdaterReplicaAdapter::incLoginCount] std::exception : " << e.what());
	} catch (...){
		MCE_WARN("[BatchUpdaterReplicaAdapter::incLoginCount] exception");
	}
}

void BatchUpdaterReplicaAdapter::incLoginCount(long id, int count){
	 try {
                 MyUtil::LongSeq keyvalue;
                 keyvalue.push_back(id);
                 std::string type("LOGINCOUNTER");
                 _clientCS.getProxy(INDEX)->incLong(keyvalue, count, type);
          } catch (Ice::Exception& e){
                  MCE_INFO("[BatchUpdaterReplicaAdapter::incLoginCount] Ice::Exception : " << e.what());
          } catch (std::exception& e){
                  MCE_INFO("[BatchUpdaterReplicaAdapter::incLoginCount] std::exception : " << e.what());
          } catch (...){
                  MCE_WARN("[BatchUpdaterReplicaAdapter::incLoginCount] exception");
          }
}
void BatchUpdaterReplicaAdapter::setLastLoginTime(int id, string time) {
	try {
		_clientCS.getProxy(INDEX)->setLastLoginTime(id, time);
	} catch (Ice::Exception& e){
		MCE_INFO("[BatchUpdaterReplicaAdapter::setLastLoginTime] Ice::Exception : " << e.what());
	} catch (std::exception& e){
		MCE_INFO("[BatchUpdaterReplicaAdapter::setLastLoginTime] std::exception : " << e.what());
	} catch (...){
		MCE_WARN("[BatchUpdaterReplicaAdapter::setLastLoginTime] exception");
	}
}

void BatchUpdaterReplicaAdapter::setLastLoginTime(long id, string time) {
	 try {
                  MyUtil::LongSeq keyvalue;
                  keyvalue.push_back(id);
                  std::string type("LASTLOGIN");
                  _clientCS.getProxy(INDEX)->setLong(keyvalue, time, type);
           } catch (Ice::Exception& e){
                   MCE_INFO("[BatchUpdaterReplicaAdapter::setLastLoginTime] Ice::Exception : " << e.what());
           } catch (std::exception& e){
                   MCE_INFO("[BatchUpdaterReplicaAdapter::setLastLoginTime] std::exception : " << e.what());
           } catch (...){
                   MCE_WARN("[BatchUpdaterReplicaAdapter::setLastLoginTime] exception");
           }
}

void BatchUpdaterReplicaAdapter::setLastLogoutTime(int id, string time) {
	try {
		_clientCS.getProxy(INDEX)->setLastLogoutTime(id, time);
	} catch (Ice::Exception& e){
		MCE_INFO("[BatchUpdaterReplicaAdapter::setLastLogoutTime] Ice::Exception : " << e.what());
	} catch (std::exception& e){
		MCE_INFO("[BatchUpdaterReplicaAdapter::setLastLogoutTime] std::exception : " << e.what());
	} catch (...){
		MCE_WARN("[BatchUpdaterReplicaAdapter::setLastLogoutTime] exception");
	}
}

void BatchUpdaterReplicaAdapter::setLastLogoutTime(long id, string time) {
	try {
        	MyUtil::LongSeq keyvalue;
                keyvalue.push_back(id);
                std::string type("LASTLOGOUT");
                _clientCS.getProxy(INDEX)->setLong(keyvalue, time, type);
 
	} catch (Ice::Exception& e){
                 MCE_INFO("[BatchUpdaterReplicaAdapter::setLastLogoutTime] Ice::Exception : " << e.what());
         } catch (std::exception& e){
                 MCE_INFO("[BatchUpdaterReplicaAdapter::setLastLogoutTime] std::exception : " << e.what());
         } catch (...){
                 MCE_WARN("[BatchUpdaterReplicaAdapter::setLastLogoutTime] exception");
         }
}

void BatchUpdaterReplicaAdapter::setLastModifyTime(int id, string time) {
	try {
		_clientCS.getProxy(INDEX)->setLastModifyTime(id, time);
	} catch (Ice::Exception& e){
		MCE_INFO("[BatchUpdaterReplicaAdapter::setLastModifyTime] Ice::Exception : " << e.what());
	} catch (std::exception& e){
		MCE_INFO("[BatchUpdaterReplicaAdapter::setLastModifyTime] std::exception : " << e.what());
	} catch (...){
		MCE_WARN("[BatchUpdaterReplicaAdapter::setLastModifyTime] exception");
	}
}

void BatchUpdaterReplicaAdapter::setLastModifyTime(long id, string time) {
	 try {
                 MyUtil::LongSeq keyvalue;
                 keyvalue.push_back(id);
                 std::string type("LASTMODIFY");
                 _clientCS.getProxy(INDEX)->setLong(keyvalue, time, type);
 
         } catch (Ice::Exception& e){
                  MCE_INFO("[BatchUpdaterReplicaAdapter::setLastModifyTime] Ice::Exception : " << e.what());
         } catch (std::exception& e){
                  MCE_INFO("[BatchUpdaterReplicaAdapter::setLastModifyTime] std::exception : " << e.what());
         } catch (...){
                  MCE_WARN("[BatchUpdaterReplicaAdapter::setLastModifyTime] exception");
         }

}

void BatchUpdaterReplicaAdapter::incFriendDist(int userId, int universityId, int count) {
	try {
		_clientCS.getProxy(INDEX)->incFriendDist(userId, universityId, count);
	} catch (Ice::Exception& e){
		MCE_INFO("[BatchUpdaterReplicaAdapter::incFriendDist] Ice::Exception : " << e.what());
	} catch (std::exception& e){
		MCE_INFO("[BatchUpdaterReplicaAdapter::incFriendDist] std::exception : " << e.what());
	} catch (...){
		MCE_WARN("[BatchUpdaterReplicaAdapter::incFriendDist] exception");
	}
}

void BatchUpdaterReplicaAdapter::incFriendDist(int userId, long universityId, int count){
	try {
                  MyUtil::LongSeq keyvalue;
		  keyvalue.push_back(userId);
                  keyvalue.push_back(universityId);
                  std::string type("FRIENDDISTCOUNTER");
                  _clientCS.getProxy(INDEX)->incLong(keyvalue, count, type);
 
          } catch (Ice::Exception& e){
                   MCE_INFO("[BatchUpdaterReplicaAdapter::incFriendDist] Ice::Exception : " << e.what());
          } catch (std::exception& e){
                   MCE_INFO("[BatchUpdaterReplicaAdapter::incFriendDist] std::exception : " << e.what());
          } catch (...){
                   MCE_WARN("[BatchUpdaterReplicaAdapter::incFriendDist] exception");
          }

}

void BatchUpdaterReplicaAdapter::decFriendDist(int userId, int universityId, int count) {
	try {
		_clientCS.getProxy(INDEX)->decFriendDist(userId, universityId, count);
	} catch (Ice::Exception& e){
		MCE_INFO("[BatchUpdaterReplicaAdapter::decFriendDist] Ice::Exception : " << e.what());
	} catch (std::exception& e){
		MCE_INFO("[BatchUpdaterReplicaAdapter::decFriendDist] std::exception : " << e.what());
	} catch (...){
		MCE_WARN("[BatchUpdaterReplicaAdapter::decFriendDist] exception");
	}
}

void BatchUpdaterReplicaAdapter::decFriendDist(int userId, long universityId, int count){
	try {
                   MyUtil::LongSeq keyvalue;
                   keyvalue.push_back(userId);
                   keyvalue.push_back(universityId);
                   std::string type("FRIENDDISTCOUNTER");
                   _clientCS.getProxy(INDEX)->decLong(keyvalue, count, type);
 
           } catch (Ice::Exception& e){
                    MCE_INFO("[BatchUpdaterReplicaAdapter::decFriendDist] Ice::Exception : " << e.what());
           } catch (std::exception& e){
                    MCE_INFO("[BatchUpdaterReplicaAdapter::decFriendDist] std::exception : " << e.what());
           } catch (...){
                    MCE_WARN("[BatchUpdaterReplicaAdapter::decFriendDist] exception");
           }

}

void BatchUpdaterReplicaAdapter::incExp(int userId, int count) {
	try {
		_clientCS.getProxy(INDEX)->incExp(userId, count);
	} catch (Ice::Exception& e){
		MCE_INFO("[BatchUpdaterReplicaAdapter::incExp] Ice::Exception : " << e.what());
	} catch (std::exception& e){
		MCE_INFO("[BatchUpdaterReplicaAdapter::incExp] std::exception : " << e.what());
	} catch (...){
		MCE_WARN("[BatchUpdaterReplicaAdapter::incExp] exception");
	}
}

void BatchUpdaterReplicaAdapter::decExp(int userId, int count) {
	try {
		_clientCS.getProxy(INDEX)->decExp(userId, count);
	} catch (Ice::Exception& e){
		MCE_INFO("[BatchUpdaterReplicaAdapter::decExp] Ice::Exception : " << e.what());
	} catch (std::exception& e){
		MCE_INFO("[BatchUpdaterReplicaAdapter::decExp] std::exception : " << e.what());
	} catch (...){
		MCE_WARN("[BatchUpdaterReplicaAdapter::decExp] exception");
	}
}

void BatchUpdaterReplicaAdapter::incTribeCount(int userId, int count) {
	try {
		_clientCS.getProxy(INDEX)->incTribeCount(userId, count);
	} catch (Ice::Exception& e){
		MCE_INFO("[BatchUpdaterReplicaAdapter::incTribeCount] Ice::Exception : " << e.what());
	} catch (std::exception& e){
		MCE_INFO("[BatchUpdaterReplicaAdapter::incTribeCount] std::exception : " << e.what());
	} catch (...){
		MCE_WARN("[BatchUpdaterReplicaAdapter::incTribeCount] exception");
	}
}

void BatchUpdaterReplicaAdapter::decTribeCount(int userId, int count) {
	try {
		_clientCS.getProxy(INDEX)->decTribeCount(userId, count);
	} catch (Ice::Exception& e){
		MCE_INFO("[BatchUpdaterReplicaAdapter::decTribeCount] Ice::Exception : " << e.what());
	} catch (std::exception& e){
		MCE_INFO("[BatchUpdaterReplicaAdapter::decTribeCount] std::exception : " << e.what());
	} catch (...){
		MCE_WARN("[BatchUpdaterReplicaAdapter::decTribeCount] exception");
	}
}

void BatchUpdaterReplicaAdapter::incTribeRequestCount(int userId, int count) {
	try {
		_clientCS.getProxy(INDEX)->incTribeRequestCount(userId, count);
	} catch (Ice::Exception& e){
		MCE_INFO("[BatchUpdaterReplicaAdapter::incTribeRequestCount] Ice::Exception : " << e.what());
	} catch (std::exception& e){
		MCE_INFO("[BatchUpdaterReplicaAdapter::incTribeRequestCount] std::exception : " << e.what());
	} catch (...){
		MCE_WARN("[BatchUpdaterReplicaAdapter::incTribeRequestCount] exception");
	}
}

void BatchUpdaterReplicaAdapter::decTribeRequestCount(int userId, int count) {
	try {
		_clientCS.getProxy(INDEX)->decTribeRequestCount(userId, count);
	} catch (Ice::Exception& e){
		MCE_INFO("[BatchUpdaterReplicaAdapter::decTribeRequestCount] Ice::Exception : " << e.what());
	} catch (std::exception& e){
		MCE_INFO("[BatchUpdaterReplicaAdapter::decTribeRequestCount] std::exception : " << e.what());
	} catch (...){
		MCE_WARN("[BatchUpdaterReplicaAdapter::decTribeRequestCount] exception");
	}
}

void BatchUpdaterReplicaAdapter::incNewMessageCount(int userId, int count) {
	try {
		_clientCS.getProxy(INDEX)->incNewMessageCount(userId, count);
	} catch (Ice::Exception& e){
		MCE_INFO("[BatchUpdaterReplicaAdapter::incNewMessageCount] Ice::Exception : " << e.what());
	} catch (std::exception& e){
		MCE_INFO("[BatchUpdaterReplicaAdapter::incNewMessageCount] std::exception : " << e.what());
	} catch (...){
		MCE_WARN("[BatchUpdaterReplicaAdapter::incNewMessageCount] exception");
	}
}

void BatchUpdaterReplicaAdapter::decNewMessageCount(int userId, int count) {
	try {
		_clientCS.getProxy(INDEX)->decNewMessageCount(userId, count);
	} catch (Ice::Exception& e){
		MCE_INFO("[BatchUpdaterReplicaAdapter::decNewMessageCount] Ice::Exception : " << e.what());
	} catch (std::exception& e){
		MCE_INFO("[BatchUpdaterReplicaAdapter::decNewMessageCount] std::exception : " << e.what());
	} catch (...){
		MCE_WARN("[BatchUpdaterReplicaAdapter::decNewMessageCount] exception");
	}
}

void BatchUpdaterReplicaAdapter::incMessageInCount(int id, int count) {
	try {
		_clientCS.getProxy(INDEX)->incMessageInCount(id, count);
	} catch (Ice::Exception& e){
		MCE_INFO("[BatchUpdaterReplicaAdapter::incMessageInCount] Ice::Exception : " << e.what());
	} catch (std::exception& e){
		MCE_INFO("[BatchUpdaterReplicaAdapter::incMessageInCount] std::exception : " << e.what());
	} catch (...){
		MCE_WARN("[BatchUpdaterReplicaAdapter::incMessageInCount] exception");
	}
}

void BatchUpdaterReplicaAdapter::decMessageInCount(int id, int count) {
	try {
		_clientCS.getProxy(INDEX)->decMessageInCount(id, count);
	} catch (Ice::Exception& e){
		MCE_INFO("[BatchUpdaterReplicaAdapter::decMessageInCount] Ice::Exception : " << e.what());
	} catch (std::exception& e){
		MCE_INFO("[BatchUpdaterReplicaAdapter::decMessageInCount] std::exception : " << e.what());
	} catch (...){
		MCE_WARN("[BatchUpdaterReplicaAdapter::decMessageInCount] exception");
	}
}

void BatchUpdaterReplicaAdapter::incMessageOutCount(int userId, int count) {
	try {
		_clientCS.getProxy(INDEX)->incMessageOutCount(userId, count);
	} catch (Ice::Exception& e){
		MCE_INFO("[BatchUpdaterReplicaAdapter::incMessageOutCount] Ice::Exception : " << e.what());
	} catch (std::exception& e){
		MCE_INFO("[BatchUpdaterReplicaAdapter::incMessageOutCount] std::exception : " << e.what());
	} catch (...){
		MCE_WARN("[BatchUpdaterReplicaAdapter::incMessageOutCount] exception");
	}
}

void BatchUpdaterReplicaAdapter::decMessageOutCount(int userId, int count) {
	try {
		_clientCS.getProxy(INDEX)->decMessageOutCount(userId, count);
	} catch (Ice::Exception& e){
		MCE_INFO("[BatchUpdaterReplicaAdapter::decMessageOutCount] Ice::Exception : " << e.what());
	} catch (std::exception& e){
		MCE_INFO("[BatchUpdaterReplicaAdapter::decMessageOutCount] std::exception : " << e.what());
	} catch (...){
		MCE_WARN("[BatchUpdaterReplicaAdapter::decMessageOutCount] exception");
	}
}

void BatchUpdaterReplicaAdapter::incFriendCount(int id, int count, const Ice::Context& ctx) {
	try {
		_clientCS.getProxy(INDEX)->incFriendCount(id, count, ctx);
	} catch (Ice::Exception& e){
		MCE_INFO("[BatchUpdaterReplicaAdapter::incFriendCount] Ice::Exception : " << e.what());
	} catch (std::exception& e){
		MCE_INFO("[BatchUpdaterReplicaAdapter::incFriendCount] std::exception : " << e.what());
	} catch (...){
		MCE_WARN("[BatchUpdaterReplicaAdapter::incFriendCount] exception");
	}
}

void BatchUpdaterReplicaAdapter::incFriendCount(long id, int count, const Ice::Context& ctx){
	 try {
		MyUtil::LongSeq keyvalue;
		keyvalue.push_back(id);
		std::string type("FRIENDCOUNTER");		
                _clientCS.getProxy(INDEX)->incLong(keyvalue, count,type, ctx);
         } catch (Ice::Exception& e){
                 MCE_INFO("[BatchUpdaterReplicaAdapter::incFriendCount] Ice::Exception : " << e.what());
         } catch (std::exception& e){
                 MCE_INFO("[BatchUpdaterReplicaAdapter::incFriendCount] std::exception : " << e.what());
         } catch (...){
                 MCE_WARN("[BatchUpdaterReplicaAdapter::incFriendCount] exception");
         }
}

void BatchUpdaterReplicaAdapter::decFriendCount(int id, int count, const Ice::Context& ctx) {
	try {	
		_clientCS.getProxy(INDEX)->decFriendCount(id, count, ctx);
	} catch (Ice::Exception& e){
		MCE_INFO("[BatchUpdaterReplicaAdapter::decFriendCount] Ice::Exception : " << e.what());
	} catch (std::exception& e){
		MCE_INFO("[BatchUpdaterReplicaAdapter::decFriendCount] std::exception : " << e.what());
	} catch (...){
		MCE_WARN("[BatchUpdaterReplicaAdapter::decFriendCount] exception");
	}
}

void BatchUpdaterReplicaAdapter::decFriendCount(long id, int count, const Ice::Context& ctx){
	try {
                 MyUtil::LongSeq keyvalue;
                 keyvalue.push_back(id);
                 std::string type("FRIENDCOUNTER");
                 _clientCS.getProxy(INDEX)->decLong(keyvalue, count,type, ctx);
          } catch (Ice::Exception& e){
                  MCE_INFO("[BatchUpdaterReplicaAdapter::decFriendCount] Ice::Exception : " << e.what());
          } catch (std::exception& e){
                  MCE_INFO("[BatchUpdaterReplicaAdapter::decFriendCount] std::exception : " << e.what());
          } catch (...){
                  MCE_WARN("[BatchUpdaterReplicaAdapter::decFriendCount] exception");
          }

}

void BatchUpdaterReplicaAdapter::incGuestRequestCount(int id, int count,
		const Ice::Context& ctx) {
	try {
		_clientCS.getProxy(INDEX)->incGuestRequestCount(id, count, ctx);
	} catch (Ice::Exception& e){
		MCE_INFO("[BatchUpdaterReplicaAdapter::incGuestRequestCount] Ice::Exception : " << e.what());
	} catch (std::exception& e){
		MCE_INFO("[BatchUpdaterReplicaAdapter::incGuestRequestCount] std::exception : " << e.what());
	} catch (...){
		MCE_WARN("[BatchUpdaterReplicaAdapter::incGuestRequestCount] exception");
	}
}

void BatchUpdaterReplicaAdapter::incGuestRequestCount(long id, int count, 
                         const Ice::Context& ctx){
	try {
                  MyUtil::LongSeq keyvalue;
                  keyvalue.push_back(id);
                  std::string type("GUESTREQUESTCOUNTER");
                  _clientCS.getProxy(INDEX)->incLong(keyvalue, count,type, ctx);
           } catch (Ice::Exception& e){
                   MCE_INFO("[BatchUpdaterReplicaAdapter::incGuestRequestCount] Ice::Exception : " << e.what());
           } catch (std::exception& e){
                   MCE_INFO("[BatchUpdaterReplicaAdapter::incGuestRequestCount] std::exception : " << e.what());
           } catch (...){
                   MCE_WARN("[BatchUpdaterReplicaAdapter::incGuestRequestCount] exception");
           }

}

void BatchUpdaterReplicaAdapter::decGuestRequestCount(int id, int count,
		const Ice::Context& ctx) {
	try {
		_clientCS.getProxy(INDEX)->decGuestRequestCount(id, count, ctx);
	} catch (Ice::Exception& e){
		MCE_INFO("[BatchUpdaterReplicaAdapter::decGuestRequestCount] Ice::Exception : " << e.what());
	} catch (std::exception& e){
		MCE_INFO("[BatchUpdaterReplicaAdapter::decGuestRequestCount] std::exception : " << e.what());
	} catch (...){
		MCE_WARN("[BatchUpdaterReplicaAdapter::decGuestRequestCount] exception");
	}
}

void BatchUpdaterReplicaAdapter::decGuestRequestCount(long id, int count,
                         const Ice::Context& ctx){
	try {
                   MyUtil::LongSeq keyvalue;
                   keyvalue.push_back(id);
                   std::string type("GUESTREQUESTCOUNTER");
                   _clientCS.getProxy(INDEX)->decLong(keyvalue, count,type, ctx);
            } catch (Ice::Exception& e){
                    MCE_INFO("[BatchUpdaterReplicaAdapter::decGuestRequestCount] Ice::Exception : " << e.what());
            } catch (std::exception& e){
                    MCE_INFO("[BatchUpdaterReplicaAdapter::decGuestRequestCount] std::exception : " << e.what());
            } catch (...){
                    MCE_WARN("[BatchUpdaterReplicaAdapter::decGuestRequestCount] exception");
            }

}

void BatchUpdaterReplicaAdapter::incLeavewordCount(int id, int count) {
	try {
		_clientCS.getProxy(INDEX)->incLeavewordCount(id, count);
	} catch (Ice::Exception& e){
		MCE_INFO("[BatchUpdaterReplicaAdapter::incLeavewordCount] Ice::Exception : " << e.what());
	} catch (std::exception& e){
		MCE_INFO("[BatchUpdaterReplicaAdapter::incLeavewordCount] std::exception : " << e.what());
	} catch (...){
		MCE_WARN("[BatchUpdaterReplicaAdapter::incLeavewordCount] exception");
	}
}

void BatchUpdaterReplicaAdapter::incLeavewordCount(long id, int count) {
	try {
                    MyUtil::LongSeq keyvalue;
                    keyvalue.push_back(id);
                    std::string type("LEAVEWORDNAME");
                    _clientCS.getProxy(INDEX)->incLong(keyvalue, count,type);
             } catch (Ice::Exception& e){
                     MCE_INFO("[BatchUpdaterReplicaAdapter::incLeavewordCount] Ice::Exception : " << e.what());
             } catch (std::exception& e){
                     MCE_INFO("[BatchUpdaterReplicaAdapter::incLeavewordCount] std::exception : " << e.what());
             } catch (...){
                     MCE_WARN("[BatchUpdaterReplicaAdapter::incLeavewordCount] exception");
             }

}

void BatchUpdaterReplicaAdapter::decLeavewordCount(int id, int count) {
	try {
		_clientCS.getProxy(INDEX)->decLeavewordCount(id, count);
	} catch (Ice::Exception& e){
		MCE_INFO("[BatchUpdaterReplicaAdapter::decLeavewordCount] Ice::Exception : " << e.what());
	} catch (std::exception& e){
		MCE_INFO("[BatchUpdaterReplicaAdapter::decLeavewordCount] std::exception : " << e.what());
	} catch (...){
		MCE_WARN("[BatchUpdaterReplicaAdapter::decLeavewordCount] exception");
	}

}

void BatchUpdaterReplicaAdapter::decLeavewordCount(long id, int count){
	try {
                     MyUtil::LongSeq keyvalue;
                     keyvalue.push_back(id);
                     std::string type("LEAVEWORDNAME");
                     _clientCS.getProxy(INDEX)->decLong(keyvalue, count,type);
              } catch (Ice::Exception& e){
                      MCE_INFO("[BatchUpdaterReplicaAdapter::decLeavewordCount] Ice::Exception : " << e.what());
              } catch (std::exception& e){
                      MCE_INFO("[BatchUpdaterReplicaAdapter::decLeavewordCount] std::exception : " << e.what());
              } catch (...){
                      MCE_WARN("[BatchUpdaterReplicaAdapter::decLeavewordCount] exception");
              }

}
