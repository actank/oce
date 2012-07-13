#ifndef __STATUS_DATA_COMPARE_HANDLER_H__
#define __STATUS_DATA_COMPARE_HANDLER_H__

#include "ServiceI.h"
#include "../interface/CompareHandler.h"
#include "../common.h"

namespace xce {
	namespace generalcheck {
		class StatusDataCompareHandler : public CompareHandler {
			public:
				virtual MyUtil::LongSeq compare(const MyUtil::ObjectMap& sourceData, const MyUtil::ObjectMap& desData, 
												const string& sourceName, const string& desName) {
					MyUtil::LongSeq wrongIds;
					MCE_INFO("Compare status data begins");
					if(sourceData.find(0) == sourceData.end()) {
						MCE_INFO("can't compare " << sourceName << " and " << desName << " because " << sourceName << " data is null");
						return wrongIds;
					}
					MCE_INFO("step 1")	
					if(desData.find(0) == desData.end()) {
						MCE_INFO("can't compare " << sourceName << " and " << desName << " because " << desName << " data is null");
						return wrongIds;
					}
					//std::cout << "Starting to compare data" << std::endl;	
					MCE_INFO("Starting to compare data");	
					const std::bitset<STATUS_CACHE_SIZE>& source = StatusDataPtr::dynamicCast(sourceData.find(0)->second)->status_;
					const std::bitset<STATUS_CACHE_SIZE>& dist = StatusDataPtr::dynamicCast(desData.find(0)->second)->status_;
					for(int i = 0; i < STATUS_CACHE_SIZE; ++i) {
						if(i % 10000 == 0) {
							//std::cout << "Compare process to " << i << std::endl;	
							MCE_INFO("Compare process to " << i);
						}
						if(source.test(i) != dist.test(i)) {
							MCE_INFO("CompareError id = " << i << " " << sourceName << " value = " << source.test(i) << " " << desName << " value = " << dist.test(i));
							wrongIds.push_back(i);
						}
					}	
					return wrongIds;
				}
		};
	}
}

#endif
