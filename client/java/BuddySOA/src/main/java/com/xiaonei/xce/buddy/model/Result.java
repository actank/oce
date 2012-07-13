package com.xiaonei.xce.buddy.model;

/**
 * 好友申请的结束对象
 * 
 * @author michael(liyong@opi-corp.com)
 * 
 */
public class Result {

	private final static String DEFAULT_EMPTY_MESSAGE = "";

	private final static String DEFAULT_ERROR_MESSAGE = "内部错误，请稍后再试";

	private final static String DEFAULT_NO_USER_MESSAGE = "该用户不存在!";

	private final static String DEFAULT_SUCCESS_ADD_FRIEND_APPLY_MESSAGE = "申请已发送，请等待对方确认";

	private final static String DEFAULT_SUCCESS_ACCEPT_FRIEND_APPLY_MESSAGE = "加好友操作成功";

	private static final String DEFAULT_TODAY_OPERATION_FULL_MESSAGE = "对不起,您今天的好友申请数已经用光!";

	private final static String DEFAULT_ADD_FRIEND_PROHIBITION_MESSAGE = "由于对方的隐私设置，无法加其为好友";

	private final static String DEFAULT_INVALID_CONTENT_MESSAGE = "请不要在好友申请描述里加站外链接地址!";

	private xce.buddy.FriendApplyResult fr = null;

	public Result(xce.buddy.FriendApplyResult fr) {
		if (fr != null) {
			this.fr = fr;
		} else {
			this.fr = new xce.buddy.FriendApplyResult();
			this.fr.result = Constants.ERROR;
		}
		setMessage();
	}

	private void setMessage() {
		switch (getResult()) {
		case Constants.ERROR:
			fr.message = DEFAULT_ERROR_MESSAGE;
			break;
		case Constants.FAKE:
			fr.message = DEFAULT_SUCCESS_ADD_FRIEND_APPLY_MESSAGE;
			break;
		case Constants.INVALID_CONTENT:
			fr.message = DEFAULT_INVALID_CONTENT_MESSAGE;
			break;
		case Constants.OPERATION_PROHIBITION:
			fr.message = DEFAULT_ADD_FRIEND_PROHIBITION_MESSAGE;
			break;
		case Constants.INVALID_USER:
			fr.message = DEFAULT_NO_USER_MESSAGE;
			break;
		case Constants.NO_USER:
			fr.message = DEFAULT_NO_USER_MESSAGE;
			break;
		case Constants.OPERATION_TOO_FAST:
			break;
		case Constants.OPERATION_FULL:
			fr.message = DEFAULT_TODAY_OPERATION_FULL_MESSAGE;
			break;
		case Constants.SUCCESS_ADD_FRIEND_APPLY:
			fr.message = DEFAULT_SUCCESS_ADD_FRIEND_APPLY_MESSAGE;
			break;
		case Constants.SUCCESS_ACCEPT_FRIEND_APPLY:
			fr.message = DEFAULT_SUCCESS_ACCEPT_FRIEND_APPLY_MESSAGE;
			break;
		default:
			break;
		}
	}

	public int getResult() {
		return fr.result;
	}

	public String getMessage() {
		return fr.message;
	}
}
