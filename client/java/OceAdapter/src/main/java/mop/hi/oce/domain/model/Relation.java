package mop.hi.oce.domain.model;


public class Relation {

	public static final int uuBiblock = 101;
	
	public static final int uuBlocked = 100;

	public static final int uuNoPath = 99;

	public static final int uuBlock = 90;

	public static final int uuRequestGuest = 4;

	public static final int uuSchoolmate = 3;

	public static final int uuGuestRequest = 1;

	public static final int uuFriend = 0;

	public static final int uuSelf = -1;

	public static final String ID = "ID";

	public static final String HOST = "HOST";

	public static final String GUEST = "GUEST";

	public static final String FLAG = "FLAG";

	public static final String DISC = "DISC";

	public static final String TIME = "TIME";

	public static int coverRelation(int type) {
		switch (type) {
		case uuBlocked:
			return uuBlock;
		case uuBlock:
			return uuBlocked;
		case uuRequestGuest:
			return uuGuestRequest;
		case uuGuestRequest:
			return uuRequestGuest;
		default:
			return type;
		}
	}
}
