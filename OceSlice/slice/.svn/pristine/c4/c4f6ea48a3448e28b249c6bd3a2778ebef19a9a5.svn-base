#ifndef	CLASSAPPLYCOUNTER_ICE
#define	CLASSAPPLYCOUNTER_ICE

module com
{
	module xiaonei
	{
		module service
		{
			module classapplycounter
			{
				struct ClassApply
				{
					int classId;
					int count; 
					string className;
				};
				sequence<ClassApply> ClassApplySeq;
			
				interface ClassApplyManager
				{
					ClassApplySeq getCounterSeq(int userId);
					void sendApply(int applicant, int classId);
					void acceptApply(int userId, int classId);
					void denyApply(int userId, int classId);
					void addAdmin(int adminId, int classId);
					void removeAdmin(int adminId, int classId);
					void reloadAdmin(int adminId);
					void reloadClass(int classId);
				};
			};
		};
	};
};

#endif

