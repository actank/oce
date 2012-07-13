#ifndef __BUDDY_DATA_COMPARE_HANDLER_H__
#define __BUDDY_DATA_COMPARE_HANDLER_H__

#include "ServiceI.h"
#include "../interface/CompareHandler.h"
#include "../BuddyCacheCheckHandler.h"

namespace xce {
	namespace generalcheck {
		class BuddyDataCompareHandler : public CompareHandler {
			public:
				virtual MyUtil::LongSeq compare(const MyUtil::ObjectMap& sourceData, const MyUtil::ObjectMap& desData, 
												const string& sourceName, const string& desName) {
					MyUtil::LongSeq wrongIds;
					for( MyUtil::ObjectMap::const_iterator it = sourceData.begin(); it != sourceData.end(); ++it ) {
						MyUtil::ObjectMap::const_iterator findIt = desData.find( it->first );
						if( findIt == desData.end() ) {
							MCE_INFO("Compare Error : id = " << it->first << " in " << sourceName << " not in " << desName);
							wrongIds.push_back( it->first );
							continue;
						}

						if( isEqual( it->second, findIt->second ) == false ) {
							MCE_INFO("Compare Error : id = " << it->first << " " << sourceName << " value = " 
										<< (CheckDataPtr::dynamicCast(it->second))->count << " " << desName << " value =  " 
										<< (CheckDataPtr::dynamicCast(findIt->second))->count);
							wrongIds.push_back(it->first);
						}
					}
				
					for( MyUtil::ObjectMap::const_iterator it = desData.begin(); it != desData.end(); ++it ) {
						MyUtil::ObjectMap::const_iterator findIt = sourceData.find( it->first );
						if( findIt == sourceData.end() ) {
							MCE_INFO("Compare Error : id = " << it->first << " in " << desName << " not in " << sourceName);
							wrongIds.push_back( it->first );
						}
					}

					return wrongIds;
				}
			private:
				bool isEqual(const Ice::ObjectPtr& sourceData, const Ice::ObjectPtr& desData) {
					return (CheckDataPtr::dynamicCast(sourceData))->count == (CheckDataPtr::dynamicCast(desData))->count ? true : false;
				}
		};
	}
}

#endif
