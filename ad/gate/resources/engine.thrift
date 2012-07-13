namespace cpp com.renren.ad.engine
namespace java com.renren.ad.engine

enum ActionType {
	PAGEVIEW, HEALTH_CHECK, DIAGONOSIS, DEBUG
}

struct UserAndAction {
	1: i32 id = -1,
	2: i16 stage = -1,
	3: i16 gender = -1,
	4: i16 age = -1,
	5: i16 school = -1,
	6: i32 fmajor = -1,
	7: i16 grade = -1,
	8: string homeArea,
	9: string currentArea,
	10: i32 ip,
	11: string ipArea,
	12: ActionType actionType,
	13: string object
}

struct AdRequest {
	1: UserAndAction userAndAction,
	2: string referer,
	3: i32 channelId,
	4: i32 syndicationId, 
	5: i32 memberId,
	6: i32 matchId,
	7: i32 requestAdNum,
	8: map<string, string> attributes
}

struct ClickAction {
	1: string field1,
	2: string field2,
	3: i32 field3
}

struct Ad {
	1: i64 creativeId,
	2: i64 adGroupId,
	3: string extra
}

struct AdResponse {
	1: map<i32, list<Ad>> candidates
}

service ImprServer {
	AdResponse handleRequest(1: AdRequest request),
	void handleClick(1: ClickAction clickAction)
}
