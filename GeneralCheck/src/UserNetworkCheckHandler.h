#ifndef _USER_NETWORK_CHECK_HANDLER_H__
#define _USER_NETWORK_CHECK_HANDLER_H__

#include "CheckHandler.h"

#include "UserNetwork.h"
#include "UserNetworkReaderAdapter.h"
#include "user/UserNetwork/hotback/share/UserNetworkDataI.h"
#include "user/UserNetwork/hotback/share/UserNetworkDataFactory.h"
#include <sstream>
#include <ext/hash_set>
#include <string.h>

namespace xce 
{
namespace generalcheck
{
	using namespace mop::hi::svc::model;
	using namespace mop::hi::svc::adapter;

	class UserNetworkCheckHandler : public CheckHandler
	{
		public:
		
			UserNetworkCheckHandler( const int sourceCluster, const int desCluster ) : CheckHandler( sourceCluster, desCluster, 15, 1000 )
			{
				_factory = new UserNetworkDataFactory();
			}
			
		public:
			
			virtual MyUtil::ObjectMap getSource( const MyUtil::LongSeq& ids, const int mod )
			{
				MyUtil::ObjectMap res;
				for( MyUtil::LongSeq::const_iterator it = ids.begin(); it != ids.end(); ++it )
				{
					NetworkDataPtr data;

					try
					{
						NetworkCacheDataPtr dataRaw = NetworkCacheDataPtr::dynamicCast(_factory->create(*it));
						if(dataRaw) {
							data = NetworkDataPtr::dynamicCast(dataRaw->getData());
						}
					} catch( ... )
					{
						std::ostringstream os;
						os<<"debug UserNetworkHandler::getSource Exception id = "<<*it<<" !";
						cout<<os.str()<<endl;
						continue;
					}
					if( data ) {
						res.insert( make_pair( *it, data ) );
					}
				}

				return res;
			}
		
			virtual MyUtil::ObjectMap getDes( const MyUtil::LongSeq& ids, const int mod )
			{
				MyUtil::ObjectMap res;
				for( MyUtil::LongSeq::const_iterator it = ids.begin(); it != ids.end(); ++it )
				{
					NetworkDataPtr data;

					try
					{
						data = UserNetworkReaderAdapter::instance().getNetworkFullData(*it);
					} catch( ... )
					{
						std::ostringstream os;
						os<<"debug UserNetworkHandler::getDes Exception id = "<<*it<<" !";
						cout<<os.str()<<endl;
						continue;
					}
					if( data ) {
						res.insert( make_pair( *it, data ) );
					}
				}

				return res;
			}
		
			virtual MyUtil::LongSeq compare( const MyUtil::ObjectMap& sourceData, const MyUtil::ObjectMap& desData )
			{
				__gnu_cxx::hash_set<long> wrongIdsSet;
				MyUtil::LongSeq wrongIds;

				for( MyUtil::ObjectMap::const_iterator it = sourceData.begin(); it != sourceData.end(); ++it )
				{
					MyUtil::ObjectMap::const_iterator findIt = desData.find( it->first );
					if( findIt == desData.end() )
					{
						wrongIdsSet.insert(it->first);
						continue;
					}
					if( isEqual( it->second, findIt->second ) == false )
					{
						wrongIdsSet.insert(it->first);
					}
				}
				
				for( MyUtil::ObjectMap::const_iterator it = desData.begin(); it != desData.end(); ++it )
				{
					MyUtil::ObjectMap::const_iterator findIt = sourceData.find( it->first );
					if( findIt == sourceData.end() )
					{
						wrongIdsSet.insert(it->first);
						continue;
					}
				}

				for(__gnu_cxx::hash_set<long>::const_iterator it = wrongIdsSet.begin(); it != wrongIdsSet.end(); ++it)
				{
					NetworkDataPtr sourceData;

					try
					{
						NetworkCacheDataPtr dataRaw = NetworkCacheDataPtr::dynamicCast(_factory->create(*it));
						if(dataRaw) {
							sourceData = NetworkDataPtr::dynamicCast(dataRaw->getData());
						}
					} catch( ... )
					{
						wrongIds.push_back(*it);
						std::ostringstream os;
						os<<"debug UserNetworkHandler::compare getSource Exception id = "<<*it<<" !";
						cout<<os.str()<<endl;
						continue;
					}
					
					NetworkDataPtr desData;

					try
					{
						desData = UserNetworkReaderAdapter::instance().getNetworkFullData(*it);
					} catch( ... )
					{
						wrongIds.push_back(*it);
						std::ostringstream os;
						os<<"debug UserNetworkHandler::compare getDes Exception id = "<<*it<<" !";
						cout<<os.str()<<endl;
						continue;
					}
					
					if( isEqual( sourceData, desData ) == false )
					{
						wrongIds.push_back(*it);
					}

				}
				return wrongIds;
			}
		
			virtual void repair( const MyUtil::LongSeq& ids, const int mod )
			{
				for(MyUtil::LongSeq::const_iterator it = ids.begin(); it != ids.end(); ++it)
				{
					try
					{
						UserNetworkAdapter::instance().reloadNetworkSeq(*it);
					}catch(...)
					{
						cout<<"UserNetworkCheckHander repair failed id = "<<*it;
						try
						{
							UserNetworkAdapter::instance().reloadNetworkSeq(*it);
						}catch(...)
						{
							cout<<"UserNetworkCheckHander repair failed again id = "<<*it;
						}
					}
				}
			}

		private:
			UserNetworkDataFactoryPtr _factory;

			//检测单个info中的各个属性项是否一致
			//返回数据是否一致。一致则返回true
			template <class T>
			bool checkItem(int userId, int recordId, string infoName, string itemName, T sItem, T dItem){
				bool result = (sItem == dItem);

				if(!result){
					cout << "Inconsistent UserNetwork[" << infoName << "::"
							<< itemName << "] : userId=" << userId << ", recordId=" << recordId
							<< ", source=" << sItem << ", des=" << dItem << endl;
				}
				return result;
			}

			//检测sInfo和dInfo是否一致
			//返回数据是否一致。一致则返回true
			bool checkInfo(NetworkPtr sInfo, NetworkPtr dInfo){
				bool result = true;
				int userId = sInfo->userId;
				int id = sInfo->id;

				result &= checkItem<int>(userId, id, "Network", "stage", sInfo->stage, dInfo->stage);
				result &= checkItem<int>(userId, id, "Network", "infoId", sInfo->infoId, dInfo->infoId);
				result &= checkItem<string>(userId, id, "Network", "joinTime", sInfo->joinTime, dInfo->joinTime);
				result &= checkItem<int>(userId, id, "Network", "status", sInfo->status, dInfo->status);
				result &= checkItem<string>(userId, id, "Network", "networkName", sInfo->networkName, dInfo->networkName);
				result &= checkItem<int>(userId, id, "Network", "networkId", sInfo->networkId, dInfo->networkId);

				return result;
			}

			//检测sInfo和dInfo是否一致
			//返回数据是否一致。一致则返回true
			bool checkInfo(HighSchoolInfoPtr sInfo, HighSchoolInfoPtr dInfo){
				bool result = true;
				int userId = sInfo->userId;
				int id = sInfo->id;

				result &= checkItem<int>(userId, id, "HighSchoolInfo", "HighSchoolId", sInfo->HighSchoolId, dInfo->HighSchoolId);
				result &= checkItem<string>(userId, id, "HighSchoolInfo", "HighSchoolName", sInfo->HighSchoolName, dInfo->HighSchoolName);
				result &= checkItem<int>(userId, id, "HighSchoolInfo", "enrollYear", sInfo->enrollYear, dInfo->enrollYear);
				result &= checkItem<string>(userId, id, "HighSchoolInfo", "hClass1", sInfo->hClass1, dInfo->hClass1);
				result &= checkItem<string>(userId, id, "HighSchoolInfo", "hClass2", sInfo->hClass2, dInfo->hClass2);
				result &= checkItem<string>(userId, id, "HighSchoolInfo", "hClass3", sInfo->hClass3, dInfo->hClass3);

				return result;
			}

			//检测sInfo和dInfo是否一致
			//返回数据是否一致。一致则返回true
			bool checkInfo(UniversityInfoPtr sInfo, UniversityInfoPtr dInfo){
				bool result = true;
				int userId = sInfo->userId;
				int id = sInfo->id;

				result &= checkItem<int>(userId, id, "UniversityInfo", "universityId", sInfo->universityId, dInfo->universityId);
				result &= checkItem<string>(userId, id, "UniversityInfo", "universityName", sInfo->universityName, dInfo->universityName);
				result &= checkItem<string>(userId, id, "UniversityInfo", "dorm", sInfo->dorm, dInfo->dorm);
				result &= checkItem<string>(userId, id, "UniversityInfo", "department", sInfo->department, dInfo->department);
				result &= checkItem<string>(userId, id, "UniversityInfo", "typeOfCourse", sInfo->typeOfCourse, dInfo->typeOfCourse);
				result &= checkItem<int>(userId, id, "UniversityInfo", "enrollYear", sInfo->enrollYear, dInfo->enrollYear);

				return result;
			}

			//检测sInfo和dInfo是否一致
			//返回数据是否一致。一致则返回true
			bool checkInfo(WorkspaceInfoPtr sInfo, WorkspaceInfoPtr dInfo){
				bool result = true;
				int userId = sInfo->userId;
				int id = sInfo->id;

				result &= checkItem<int>(userId, id, "WorkspaceInfo", "workplaceId", sInfo->workplaceId, dInfo->workplaceId);
				result &= checkItem<string>(userId, id, "WorkspaceInfo", "workplaceName", sInfo->workplaceName, dInfo->workplaceName);
				result &= checkItem<string>(userId, id, "WorkspaceInfo", "address", sInfo->address, dInfo->address);
				result &= checkItem<string>(userId, id, "WorkspaceInfo", "description", sInfo->description, dInfo->description);
				result &= checkItem<string>(userId, id, "WorkspaceInfo", "province", sInfo->province, dInfo->province);
				result &= checkItem<int>(userId, id, "WorkspaceInfo", "cityId", sInfo->cityId, dInfo->cityId);
				result &= checkItem<string>(userId, id, "WorkspaceInfo", "cityName", sInfo->cityName, dInfo->cityName);
				result &= checkItem<int>(userId, id, "WorkspaceInfo", "joinYear", sInfo->joinYear, dInfo->joinYear);
				result &= checkItem<int>(userId, id, "WorkspaceInfo", "joinMonth", sInfo->joinMonth, dInfo->joinMonth);
				result &= checkItem<int>(userId, id, "WorkspaceInfo", "quitYear", sInfo->quitYear, dInfo->quitYear);
				result &= checkItem<int>(userId, id, "WorkspaceInfo", "quitMonth", sInfo->quitMonth, dInfo->quitMonth);
				result &= checkItem<int>(userId, id, "WorkspaceInfo", "type", sInfo->type, dInfo->type);
				result &= checkItem<int>(userId, id, "WorkspaceInfo", "jobTitleId", sInfo->jobTitleId, dInfo->jobTitleId);
				result &= checkItem<int>(userId, id, "WorkspaceInfo", "positionId", sInfo->positionId, dInfo->positionId);

				return result;
			}

			//检测sInfo和dInfo是否一致
			//返回数据是否一致。一致则返回true
			bool checkInfo(CollegeInfoPtr sInfo, CollegeInfoPtr dInfo){
				bool result = true;
				int userId = sInfo->userId;
				int id = sInfo->id;

				result &= checkItem<int>(userId, id, "CollegeInfo", "collegeId", sInfo->collegeId, dInfo->collegeId);
				result &= checkItem<string>(userId, id, "CollegeInfo", "collegeName", sInfo->collegeName, dInfo->collegeName);
				result &= checkItem<string>(userId, id, "CollegeInfo", "department", sInfo->department, dInfo->department);
				result &= checkItem<int>(userId, id, "CollegeInfo", "enrollYear", sInfo->enrollYear, dInfo->enrollYear);

				return result;
			}

			//检测sInfo和dInfo是否一致
			//返回数据是否一致。一致则返回true
			bool checkInfo(ElementarySchoolInfoNPtr sInfo, ElementarySchoolInfoNPtr dInfo){
				bool result = true;
				int userId = sInfo->userId;
				int id = sInfo->id;

				result &= checkItem<int>(userId, id, "ElementarySchoolInfo", "elementarySchoolId", sInfo->elementarySchoolId, dInfo->elementarySchoolId);
				result &= checkItem<string>(userId, id, "ElementarySchoolInfo", "elementarySchoolName", sInfo->elementarySchoolName, dInfo->elementarySchoolName);
				result &= checkItem<int>(userId, id, "ElementarySchoolInfo", "elementarySchoolYear", sInfo->elementarySchoolYear, dInfo->elementarySchoolYear);

				return result;
			}

			//检测sInfo和dInfo是否一致
			//返回数据是否一致。一致则返回true
			bool checkInfo(JuniorHighSchoolInfoNPtr sInfo, JuniorHighSchoolInfoNPtr dInfo){
				bool result = true;
				int userId = sInfo->userId;
				int id = sInfo->id;

				result &= checkItem<int>(userId, id, "JuniorHighSchoolInfo", "juniorHighSchoolId", sInfo->juniorHighSchoolId, dInfo->juniorHighSchoolId);
				result &= checkItem<string>(userId, id, "JuniorHighSchoolInfo", "juniorHighSchoolName", sInfo->juniorHighSchoolName, dInfo->juniorHighSchoolName);
				result &= checkItem<int>(userId, id, "JuniorHighSchoolInfo", "juniorHighSchoolYear", sInfo->juniorHighSchoolYear, dInfo->juniorHighSchoolYear);

				return result;
			}

			//检测sInfo和dInfo是否一致
			//返回数据是否一致。一致则返回true
			bool checkInfo(RegionInfoPtr sInfo, RegionInfoPtr dInfo){
				bool result = true;
				int userId = sInfo->userId;
				int id = sInfo->id;

				result &= checkItem<int>(userId, id, "RegionInfo", "regionId", sInfo->regionId, dInfo->regionId);
				result &= checkItem<string>(userId, id, "RegionInfo", "provinceName", sInfo->provinceName, dInfo->provinceName);
				result &= checkItem<string>(userId, id, "RegionInfo", "cityName", sInfo->cityName, dInfo->cityName);

				return result;
			}

			//从infos数组中获取record值为id的info，不考虑重复record_id的情况
			template<class TPtr, class TSeq>
			typename TSeq::const_iterator getInfoById(int id, TSeq &infos) {
				typename TSeq::const_iterator it = infos.begin();
				for(; it != infos.end(); it++){
					if(TPtr::dynamicCast(*it)->id == id){
						break;
					}
				}
				return it;
			}

			//检测info数组sInfos和dInfos是否一致
			//返回数据是否一致。一致则返回true
			template<class TPtr, class TSeq>
			bool checkInfos(TSeq &sInfos, TSeq &dInfos){
				bool result = true;
				int recordId;
				MyUtil::LongSeq recordIds; //保存已经检测过的record id，减少检测次数

				//检测sInfos
				for (typename TSeq::const_iterator its = sInfos.begin(); its != sInfos.end(); ++its) {
					recordId = TPtr::dynamicCast(*its)->id;
					typename TSeq::const_iterator itd = getInfoById<TPtr, TSeq>(recordId, dInfos);

					if(itd != dInfos.end()){
						recordIds.push_back(recordId);
						result &= checkInfo(TPtr::dynamicCast(*its), TPtr::dynamicCast(*itd));
					}else{
						result &= false;
						cout << "Inconsistent UserNetwork[" <<  typeid(TSeq).name() <<"] : userId=" << (*its)->userId
								<< ", record not found in desData : id=" << recordId << endl;
					}
				}

				//检测dInfos
				for (typename TSeq::const_iterator itd = dInfos.begin(); itd != dInfos.end(); ++itd) {
					recordId = TPtr::dynamicCast(*itd)->id;
					if(find(recordIds.begin(), recordIds.end(), recordId) != recordIds.end()){
						continue;
					}
					typename TSeq::const_iterator its = getInfoById<TPtr, TSeq>(recordId, sInfos);
					if(its != sInfos.end()){
						result &= checkInfo(TPtr::dynamicCast(*its), TPtr::dynamicCast(*itd));
					}else{
						result &= false;
						cout << "Inconsistent UserNetwork[" <<  typeid(TSeq).name() <<"] : userId=" << (*itd)->userId
								<< ", record not found in sourceData : id=" << recordId << endl;
					}
				}

				return result;
			}

			//检测sData和dData是否一致
			//返回数据是否一致。一致则返回true
			bool checkAllInfos (NetworkDataPtr &sData, NetworkDataPtr &dData){
				bool result = true;

				try {
					result &= checkInfos<NetworkPtr, NetworkSeq> (sData->networks, dData->networks);
					result &= checkInfos<HighSchoolInfoPtr, HighSchoolInfoSeq> (sData->highschools, dData->highschools);
					result &= checkInfos<UniversityInfoPtr, UniversityInfoSeq> (sData->universities, dData->universities);
					result &= checkInfos<WorkspaceInfoPtr, WorkspaceInfoSeq> (sData->workspaces, dData->workspaces);
					result &= checkInfos<CollegeInfoPtr, CollegeInfoSeq> (sData->colleges, dData->colleges);
					result &= checkInfos<ElementarySchoolInfoNPtr, ElementarySchoolInfoSeq> (sData->elementaries, dData->elementaries);
					result &= checkInfos<JuniorHighSchoolInfoNPtr, JuniorHighSchoolInfoSeq> (sData->juniorhighschools, dData->juniorhighschools);
					result &= checkInfos<RegionInfoPtr, RegionInfoSeq> (sData->regions, dData->regions);
				} catch (Ice::Exception& e) {
					MCE_WARN("[checkAllInfos] Ice::Exception : " << e.what());
				} catch (std::exception& e) {
					MCE_WARN("[checkAllInfos] std::exception : " << e.what());
				} catch (...) {
					MCE_WARN("[checkAllInfos] UNKNOWN exception");
				}

				return result;
			}

			//判断缓存是否一致，并输出不一致的信息
			bool isEqual(const Ice::ObjectPtr& sourceData, const Ice::ObjectPtr& desData) {
				bool result = true;

				if (!sourceData && !desData) {//都空
					result = true;
				} else if (!sourceData) {//一个空，另外一个非空
					cout << "Inconsistent UserNetwork : userId=" << (NetworkDataPtr::dynamicCast(desData))->userId
							<< ", sourceData is null" << endl;
					result = false;
				} else if (!desData) {
					cout << "Inconsistent UserNetwork : userId=" << (NetworkDataPtr::dynamicCast(sourceData))->userId
							<< ", desData is null" << endl;
					result = false;
				} else {
					NetworkDataPtr sData = NetworkDataPtr::dynamicCast(sourceData);
					NetworkDataPtr dData = NetworkDataPtr::dynamicCast(desData);
					result = checkAllInfos(sData, dData);
				}

				return result;
			}
	};

}
}

#endif
