package com.xiaonei.xce.personalinfo;

public final class PersonalInfoAdapter<T extends LinkViewInfo> {
	private PersonalInfoWriterAdapter _personalInfowriter;
	private LinkViewReaderAdapter<T> _linkViewReader;
	private Link2UserIdReaderAdapter<T> _link2UserIdReader;

	public PersonalInfoAdapter(LinkViewInfoFactory<T> factory) {
		_personalInfowriter = new PersonalInfoWriterAdapter();
		_linkViewReader = new LinkViewReaderAdapter<T>(factory);
		_link2UserIdReader = new Link2UserIdReaderAdapter<T>(factory);
	}

	/**
	 * 从用户ID获取LinkViewInfo，如果不命中则异步查数据库加载
	 * @param userId
	 * @return LinkViewInfo
	 * 如果没有用户ID，则返回[id:0, link:null, linkStatus:0]
	 * 如果有用户ID，则返回[id:1007, link:NULL, linkStatus:0]
	 * 其中null是空串，NULL是长度为4的字符串
	 * @throws Exception
	 */
	public LinkViewInfo getLinkView(int userId) throws Exception {
		return _linkViewReader.getLinkView(userId);
	}

	/**
	 * 从用户ID获取LinkViewInfo，如果不命中则同步查数据库加载
	 * @param userId
	 * @return LinkViewInfo
	 * 如果没有用户ID，则返回[id:0, link:null, linkStatus:0]
	 * 如果有用户ID，则返回[id:1007, link:NULL, linkStatus:0]
	 * 其中null是空串，NULL是长度为4的字符串
	 * @throws Exception
	 */
	public LinkViewInfo getLinkViewSync(int userId) throws Exception {
		return _linkViewReader.getLinkViewSync(userId);
	}

	/**
	 * 从个人网址获取LinkViewInfo，如果不命中，不从数据库加载
	 * @param link
	 * @return LinkViewInfo
	 * 如果没有用户ID，则返回[id:0, link:null, linkStatus:0]
	 * 如果有用户ID，则返回[id:1007, link:NULL, linkStatus:0]
	 * 其中null是空串，NULL是长度为4的字符串
	 * @throws Exception
	 */
	public LinkViewInfo getLinkView(String link) throws Exception {
		return _link2UserIdReader.getLinkView(link);
	}

	/**
	 * 从个人网址获取LinkViewInfo，如果不命中，同步从数据库加载
	 * @param link
	 * @return LinkViewInfo
	 * 如果没有用户ID，则返回[id:0, link:null, linkStatus:0]
	 * 如果有用户ID，则返回[id:1007, link:NULL, linkStatus:0]
	 * 其中null是空串，NULL是长度为4的字符串
	 * @throws Exception
	 */
	public LinkViewInfo getLinkViewSync(String link) throws Exception {
		return _link2UserIdReader.getLinkViewSync(link);
	}

	/**
	 * 从个人网址获取用户ID，如果不命中，不从数据库加载
	 * @param link
	 * @return 对应的用户ID，如果link不存在，则返回0
	 * @throws Exception
	 */
	public int getUserId(String link) throws Exception {
		return _link2UserIdReader.getUserId(link);
	}

	/**
	 * 从个人网址获取用户ID，如果不命中，同步从数据库加载
	 * @param link
	 * @return 对应的用户ID，如果link不存在，则返回0
	 * @throws Exception
	 */
	public int getUserIdSync(String link) throws Exception {
		return _link2UserIdReader.getUserIdSync(link);
	}

	/**
	 * 在DB创建新的行，同时同步缓存
	 * @param userId
	 * @param info 需要写DB的信息
	 * @throws Exception
	 */
	public void createPersonalInfo(int userId, PersonalInfo info) throws Exception {
		_personalInfowriter.createPersonalInfo(userId, info);
	}

	/**
	 * 在DB修改已经存在的行，同时同步缓存
	 * @param userId
	 * @param info 需要修改DB的信息
	 * @throws Exception
	 */
	public void setPersonalInfo(int userId, PersonalInfo info) throws Exception {
		_personalInfowriter.setPersonalInfo(userId, info);
	}
//
//	//供中间层快速调用，不对外开放
//	public static void main(String[] args) {
//		int userId = 35178;
//		String link = "joyeru";
//		PersonalInfoAdapter<DefaultLinkViewInfo> adapter
//			= new PersonalInfoAdapter<DefaultLinkViewInfo>(new DefaultLinkViewInfoFactory());
//
//		//写入操作
//		PersonalInfo info = new PersonalInfo();
//		info.setLink(link);
//		try {
//			adapter.setPersonalInfo(userId, info);
//		} catch (Exception e) {
//			e.printStackTrace();
//		}
//
//		//从int userId得到linkViewInfo
//		try {
//			LinkViewInfo linkViewInfo = adapter.getLinkView(userId);
//			System.out.println("getLinkView(" + userId + ") = " + linkViewInfo);
//		} catch (Exception e) {
//			e.printStackTrace();
//		}
//
//		//从String link得到linkViewInfo和userId
//		try {
//			LinkViewInfo linkViewInfo = adapter.getLinkView(link);
//			System.out.println("getLinkView(\"" + link + "\") = " + linkViewInfo);
//			int userIdGet = adapter.getUserId(link);
//			System.out.println("getUserId(\"" + link + "\") = " + userIdGet);
//		} catch (Exception e) {
//			e.printStackTrace();
//		}
//
//
//		System.out.println("Ok");
//		System.exit(0);
//	}

}
