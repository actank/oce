#ifndef _INVITEREWARD_ICE
#define _INVITEREWARD_ICE
module com{
	module xiaonei {
		module wService {
			module invitereward {
				struct InviteeNode{
					int inviteeId;
					int inviterId;
					int addTime;
					int addStarTime;
					int inviteeRewardState;
				};
				//["java:type:java.util.ArrayList"]  sequence<InviteeNode> InviteeNodeSeq; 
				sequence<InviteeNode> InviteeNodeSeq; 
				class InviterNode{
					InviteeNodeSeq rewardList;
					InviteeNodeSeq unRewardList;
					int inviterRewardState;
					int nextRewardLeft;
					int nextRewardState;
				};
				interface InviteRewardManager {
					void addInvite(int inviterId,int inviteeId);
					void addInvite4MM(int inviterId,int inviteeId);
					void addInvite4SS(int inviterId,int inviteeId);
					void addInvite4SSMM(int inviterId,int inviteeId);
					void addInvite4WC(int inviterId,int inviteeId);
					void addStar(int userId);
					InviterNode getInviterData(int inviterId);
					InviteeNode getInviteeData(int inviteeId);
				};
			};
		};
	};
};
#endif
