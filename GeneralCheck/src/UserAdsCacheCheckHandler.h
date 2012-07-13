#ifndef _USERADSCACHE_CHECK_HANDLER_H__
#define _USERADSCACHE_CHECK_HANDLER_H__

#include "CheckHandler.h"

#include "UserAdsCacheAdapter.h"
#include "UserAdsCacheLoaderAdapter.h"
#include "UserBaseAdapter.h"
#include "UserNetworkAdapter.h"
#include "DistUserCacheReloaderAdapter.h"
#include <sstream>

namespace xce 
{
namespace generalcheck
{
	using mop::hi::svc::model::WorkspaceInfoSeq;
	using mop::hi::svc::model::NetworkSeq;
	using mop::hi::svc::model::HighSchoolInfoSeq;
	using mop::hi::svc::model::UniversityInfoSeq;
	using mop::hi::svc::model::CollegeInfoSeq;
	using xce::useradscache::UserAdsCacheNetworkPtr;
	using xce::useradscache::UserAdsCacheWorkplacePtr;
	using xce::useradscache::UserAdsCacheSchoolPtr;
	using xce::useradscache::Elementary;	
	using xce::useradscache::SchoolType;	
	using xce::useradscache::High;	
	using xce::useradscache::University;	
	using xce::useradscache::College;	
	using xce::useradscache::Junior;	
	using mop::hi::svc::model::NetworkBigObjectNPtr;
	using xce::adapter::userbase::UserBaseAdapter;
	using mop::hi::svc::adapter::UserNetworkAdapter;
	using xce::useradscache::UserAdsCacheUrlDataPtr;
	using xce::useradscache::UserAdsCacheUrlData;
	using xce::useradscache::NetworkInfoPtr;
	using xce::useradscache::NetworkInfo;
	using xce::useradscache::WorkplaceInfoPtr;
	using xce::useradscache::WorkplaceInfo;
		
	class UserAdsCacheCheckHandler : public CheckHandler
	{
		public:
		
			UserAdsCacheCheckHandler( const int sourceCluster, const int desCluster ) : CheckHandler( sourceCluster, desCluster , 1, 100 ) 
			{
			}
			
		public:
			
			virtual MyUtil::ObjectMap getSource( const MyUtil::LongSeq& ids, const int mod )
			{
				MyUtil::ObjectMap res;
				for(MyUtil::LongSeq::const_iterator it = ids.begin(); it != ids.end(); ++it) {
					xce::adapter::userbase::UserBaseFullViewPtr bornInfoPtr = 0;
					NetworkBigObjectNPtr networkPtr = 0; 
					try {
						bornInfoPtr = xce::adapter::userbase::UserBaseAdapter::instance().getUserBaseFullView( *it );
						networkPtr = UserNetworkAdapter::instance().getBigObjectN(*it);
					} catch( ... ) {
						std::cout<<"debug UserAdsCacheCheckHandler::getSource Exception!"<<std::endl;
					}
					if(bornInfoPtr != 0 && networkPtr != 0) {
						UserAdsCacheUrlDataPtr adsCacheDataPtr = new UserAdsCacheUrlData();
						adsCacheDataPtr->id = bornInfoPtr->id();
						adsCacheDataPtr->stage = bornInfoPtr->stage();
						adsCacheDataPtr->sex = bornInfoPtr->gender();
						adsCacheDataPtr->birthyear = bornInfoPtr->birthyear();
						adsCacheDataPtr->birthmonth = bornInfoPtr->birthmonth();
						adsCacheDataPtr->birthday = bornInfoPtr->birthday();
						adsCacheDataPtr->homeprovince = bornInfoPtr->homeprovince();
						adsCacheDataPtr->homecity = bornInfoPtr->homecity();
						adsCacheDataPtr->tinyurl = bornInfoPtr->tinyurl();
						adsCacheDataPtr->seatcity = "";
						adsCacheDataPtr->seatprovince = "";
					
						for(NetworkSeq::const_iterator nsIt = networkPtr->networks.begin(); nsIt != networkPtr->networks.end(); ++nsIt) {
							NetworkInfoPtr networkPtr = new NetworkInfo();
							networkPtr->networkid = (*nsIt)->networkId; 
							networkPtr->stage = (*nsIt)->stage;
							networkPtr->status = (*nsIt)->status;
							adsCacheDataPtr->networks.push_back(networkPtr);
						}

						for(WorkspaceInfoSeq::const_iterator wpIt = networkPtr->workspaces.begin(); wpIt != networkPtr->workspaces.end(); ++wpIt) {
							WorkplaceInfoPtr workplacePtr = new WorkplaceInfo();
							workplacePtr->workplacename = (*wpIt)->workplaceName;
							workplacePtr->jobtitleid = (*wpIt)->jobTitleId;
							workplacePtr->positionid = 	(*wpIt)->positionId;
							adsCacheDataPtr->workplaces.push_back(workplacePtr);
						}

						for(HighSchoolInfoSeq::const_iterator hsIt = networkPtr->highschools.begin(); hsIt != networkPtr->highschools.end(); ++hsIt) {
							xce::useradscache::SchoolInfoPtr schoolPtr = new xce::useradscache::SchoolInfo();
							schoolPtr->id = (*hsIt)->HighSchoolId;
							schoolPtr->name = (*hsIt)->HighSchoolName;
							schoolPtr->type = xce::useradscache::High;
							schoolPtr->enrollyear = (*hsIt)->enrollYear;
							schoolPtr->department = "";
							schoolPtr->dorm = "";
							adsCacheDataPtr->schools.push_back(schoolPtr);
						}
						for(UniversityInfoSeq::const_iterator usIt = networkPtr->universitys.begin(); usIt != networkPtr->universitys.end(); ++usIt) {
							xce::useradscache::SchoolInfoPtr schoolPtr = new xce::useradscache::SchoolInfo();
							schoolPtr->id = (*usIt)->universityId;
							schoolPtr->name = (*usIt)->universityName;
							schoolPtr->type = xce::useradscache::University;
							schoolPtr->enrollyear = (*usIt)->enrollYear;
							schoolPtr->department = (*usIt)->department;
							schoolPtr->dorm = (*usIt)->dorm;
							adsCacheDataPtr->schools.push_back(schoolPtr);
						}
						for(CollegeInfoSeq::const_iterator csIt = networkPtr->collages.begin(); csIt != networkPtr->collages.end(); ++csIt) {
							xce::useradscache::SchoolInfoPtr schoolPtr = new xce::useradscache::SchoolInfo();
							schoolPtr->id = (*csIt)->collegeId;
							schoolPtr->name = (*csIt)->collegeName;
							schoolPtr->type = xce::useradscache::College;
							schoolPtr->enrollyear = (*csIt)->enrollYear;
							schoolPtr->department = (*csIt)->department;
							schoolPtr->dorm = "";
							adsCacheDataPtr->schools.push_back(schoolPtr);
						}

						if( networkPtr->elementary != 0 )
						{
							xce::useradscache::SchoolInfoPtr schoolPtr = new xce::useradscache::SchoolInfo();
							schoolPtr->id = networkPtr->elementary->elementarySchoolId;
							schoolPtr->name = networkPtr->elementary->elementarySchoolName;
							schoolPtr->type = xce::useradscache::Elementary;
							schoolPtr->enrollyear = networkPtr->elementary->elementarySchoolYear;
							schoolPtr->department = "";
							schoolPtr->dorm = "";
							adsCacheDataPtr->schools.push_back(schoolPtr);
						}

						if( networkPtr->juniorhighschool != 0 )
						{
							xce::useradscache::SchoolInfoPtr schoolPtr = new xce::useradscache::SchoolInfo();
							schoolPtr->id = networkPtr->juniorhighschool->juniorHighSchoolId;
							schoolPtr->name = networkPtr->juniorhighschool->juniorHighSchoolName;
							schoolPtr->type = xce::useradscache::Junior;
							schoolPtr->enrollyear = networkPtr->juniorhighschool->juniorHighSchoolYear;
							schoolPtr->department = "";
							schoolPtr->dorm = "";
							adsCacheDataPtr->schools.push_back(schoolPtr);
						}
						
						xce::useradscache::UserAdsCachePtr adsCachePtr = new xce::useradscache::UserAdsCache(adsCacheDataPtr);
						res.insert(make_pair(*it, adsCachePtr)); 
					}
				}
				return res;
			}
		
			virtual MyUtil::ObjectMap getDes(const MyUtil::LongSeq& ids, const int mod) {
				std::map<int, xce::useradscache::UserAdsCachePtr> data = 
					xce::useradscache::UserAdsCacheAdapter::instance().getUserAdsCacheList(MyUtil::IntSeq(ids.begin(), ids.end()));
				return MyUtil::ObjectMap(data.begin(), data.end());
			}
		
			virtual MyUtil::LongSeq compare( const MyUtil::ObjectMap& sourceData, const MyUtil::ObjectMap& desData )
			{
				MyUtil::LongSeq wrongIds;

				for( MyUtil::ObjectMap::const_iterator it = sourceData.begin(); it != sourceData.end(); ++it )
				{
					MyUtil::ObjectMap::const_iterator findIt = desData.find( it->first );
					if( findIt == desData.end() )
					{
						wrongIds.push_back( it->first );
						continue;
					}

					if( isEqual( it->second, findIt->second ) == false )
					{
						wrongIds.push_back( it->first );
					}
				}
				
				for( MyUtil::ObjectMap::const_iterator it = desData.begin(); it != desData.end(); ++it )
				{
					MyUtil::ObjectMap::const_iterator findIt = sourceData.find( it->first );
					if( findIt == sourceData.end() )
					{
						wrongIds.push_back( it->first );
					}
				}

				return wrongIds;
			}
		
			virtual void repair( const MyUtil::LongSeq& ids, const int mod )
			{
				xce::useradscache::UserAdsCacheLoaderAdapter::instance().load( MyUtil::IntSeq( ids.begin(), ids.end() ) );
			}

		private:

			bool isEqual( const Ice::ObjectPtr& sourceData, const Ice::ObjectPtr& desData )
			{
				xce::useradscache::UserAdsCachePtr dData = xce::useradscache::UserAdsCachePtr::dynamicCast( desData );
				xce::useradscache::UserAdsCachePtr sData = xce::useradscache::UserAdsCachePtr::dynamicCast( sourceData );
				string s_str = toString(sData);
				string d_str = toString(dData);
				if(s_str.compare("NULL") != 0 && s_str.compare(d_str) == 0) {
					return true;
				} else {
					string dif1 = "", dif2 = "";
					findDiff(s_str, d_str, dif1, dif2);
					std::cout <<"debug wrong id = " << sData->getId() << "\t" << dif1 << endl;
					std::cout <<"debug wrong id = " << dData->getId() << "\t" << dif2 << endl;
					std::cout <<"debug id = " << sData->getId() << "\t" << s_str << endl;
					std::cout <<"debug id = " << dData->getId() << "\t" << d_str << endl;
					return false;
				}
			}
		
			void findDiff(const string& str1, const string& str2, string& dif1, string& dif2) {
				int spos = 0;
				int size1 = str1.length();
				int size2 = str2.length();
				while(spos < size1 && spos < size2) {
					size_t fpos1 = str1.find("\t", spos);
					size_t fpos2 = str2.find("\t", spos);
					if(fpos1 != string::npos) {
						dif1 = str1.substr(spos, fpos1 - spos);
					} else {
						dif1 = str1.substr(spos);
					}
					if(fpos2 != string::npos) {
						dif2 = str2.substr(spos, fpos2 - spos);
					} else {
						dif2 = str2.substr(spos);
					}
					if(dif1.compare(dif2) != 0) {
						return;
					}
					spos = fpos1 + 1;
				}
			}
				
			class CompWorkplace {
				public:
				bool operator()(const xce::useradscache::UserAdsCacheWorkplacePtr ptr1, const xce::useradscache::UserAdsCacheWorkplacePtr ptr2) { 
					string name1 = ptr1->getWorkplaceName();
					string name2 = ptr2->getWorkplaceName();
					int jobId1 = ptr1->getJobTitleId();
					int jobId2 = ptr2->getJobTitleId();
					int positionId1 = ptr1->getPositionId();
					int positionId2 = ptr2->getPositionId();
					return name1 < name2 || name1 == name2 && jobId1 < jobId2 || name1 == name2 && jobId1 == jobId2 && positionId1 < positionId2;
				}
			};

			class CompNetwork {
				public:
					bool operator()(const UserAdsCacheNetworkPtr ptr1, const UserAdsCacheNetworkPtr ptr2) {
						int networkId1 = ptr1->getNetworkId();
						int networkId2 = ptr2->getNetworkId();
						int stage1 = ptr1->getStage();
						int stage2 = ptr2->getStage();
						int status1 = ptr1->getStatus();
						int status2 = ptr2->getStatus();
						return networkId1 < networkId2 || networkId1 == networkId2 && stage1 < stage2 
								|| networkId1 == networkId2 && stage1 == stage2 && status1 < status2;
					}
			};

			class CompSchool {
				public:
					bool operator()(const UserAdsCacheSchoolPtr ptr1, const UserAdsCacheSchoolPtr ptr2) {
						int id1 = ptr1->getId();
						string name1 = ptr1->getName();
						SchoolType type1 = ptr1->getType();
						int enrollyear1 = ptr1->getEnrollYear();
						string department1 = ptr1->getDepartment();
						string dorm1 = ptr1->getDorm();
						int id2 = ptr2->getId();
						string name2 = ptr2->getName();
						SchoolType type2 = ptr2->getType();
						int enrollyear2 = ptr2->getEnrollYear();
						string department2 = ptr2->getDepartment();
						string dorm2 = ptr2->getDorm();
						return type1 < type2 || type1 == type2 && id1 < id2 || type1 == type2 && id1 == id2 && name1 < name2 
								|| type1 == type2 && id1 == id2 && name1 == name2 && enrollyear1 < enrollyear2 
								|| type1 == type2 && id1 == id2 && name1 == name2 && enrollyear1 == enrollyear2 && department1 < department2
								|| type1 == type2 && id1 == id2 && name1 == name2 && enrollyear1 == enrollyear2 && department1 == department2 && dorm1 < dorm2;
					}
			};

			string toString(const xce::useradscache::UserAdsCachePtr ptr) {
				ostringstream str;	
				if(ptr == 0) {
					str << "NULL";
				} else {
					str << "UserAdsCache : id = " << ptr->getId() << "\tstage = " << ptr->getStage() << "\tsex = " << ptr->getSex() << "\tbirthyear = "
						<< ptr->getBirthYear() << "\tbirthmonth = " << ptr->getBirthMonth() << "\tbirthday = " << ptr->getBirthDay() << "\thomeprovince = "
						<< ptr->getHomeProvince() << "\thomecity = " << ptr->getHomeCity();
						//<< "\ttinyurl = " << ptr->getTinyUrl();
					std::vector<UserAdsCacheWorkplacePtr> workPlaces = ptr->getWorkplaces();
					sort(workPlaces.begin(), workPlaces.end(), CompWorkplace());
					for(std::vector<UserAdsCacheWorkplacePtr>::const_iterator it = workPlaces.begin(); it != workPlaces.end(); ++it) {
						str << "\t" << toString(*it);
					}
					std::vector<UserAdsCacheNetworkPtr> networks = ptr->getNetworks();
					sort(networks.begin(), networks.end(), CompNetwork());
					for(std::vector<UserAdsCacheNetworkPtr>::const_iterator it = networks.begin(); it != networks.end(); ++it) {
						str << "\t" << toString(*it);
					}
					std::vector<UserAdsCacheSchoolPtr> schools = ptr->getSchools();
					sort(schools.begin(), schools.end(), CompSchool());
					for(std::vector<UserAdsCacheSchoolPtr>::const_iterator it = schools.begin(); it != schools.end(); ++it) {
						str << "\t" << toString(*it);
					}
				}
				return str.str();
			}

			string toString(const xce::useradscache::UserAdsCacheWorkplacePtr ptr) {
				ostringstream str;
				if(ptr == 0) {
					str << "NULL";
				} else {
					str << "UserAdsCacheWorkplace : workplacename = " << ptr->getWorkplaceName() << "\tjobtitleid = " << ptr->getJobTitleId() << "\tpositionid = " 
						<< ptr->getPositionId();
				}
				return str.str();
			}

			string toString(const xce::useradscache::UserAdsCacheSchoolPtr ptr) {
				ostringstream str;
				if(ptr == 0) {
					str << "NULL";
				} else {
					//str << "UserAdsCacheSchool : id = " << ptr->getId() << "\tname = " << ptr->getName() << "\ttype = " << ptr->getType() << "\tdepartment = "
					str << "UserAdsCacheSchool : name = " << ptr->getName() << "\ttype = " << ptr->getType() << "\tdepartment = "
						<< ptr->getDepartment() << "\tdorm = " << ptr->getDorm();
				}
				return str.str();
			}

			string toString(SchoolType type) {
				switch(type) {
					case Elementary:
						return "Elementary";
					case High:
						return "High";
					case University:
						return "University";
					case Junior:
						return "Junior";
					case College:
						return "College";
				}
			}

			string toString(const xce::useradscache::UserAdsCacheNetworkPtr ptr) {
				ostringstream str;
				if(ptr == 0) {
					str << "NULL";
				} else {
					str << "UserAdsCacheNetwork : networkid = " << ptr->getNetworkId() << "\tstage = " << ptr->getStage() << "\tstatus = " << ptr->getStatus();
				}
				return str.str();
			}
	};

}
}

#endif
