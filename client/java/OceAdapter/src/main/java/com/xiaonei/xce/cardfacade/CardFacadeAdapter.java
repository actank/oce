package com.xiaonei.xce.cardfacade;

import java.util.ArrayList;
import java.util.Collections;
import java.util.List;
import java.util.Map;

import javax.mail.search.ReceivedDateTerm;
import javax.management.relation.RelationType;

import mop.hi.oce.adapter.AdapterFactory;

import com.xiaonei.xce.buddybynamecache.BuddyByNameCacheAdapter;
import com.xiaonei.xce.usercache.UserCacheAdapter;
import com.xiaonei.xce.usercache.DefaultUserCache;
import com.xiaonei.xce.usercache.DefaultUserCacheFactory;

import xce.cardcache.CardInfo;
import xce.cardcache.ImportEmail;
import xce.cardcache.ImportIdImportDetail;
import xce.cardcache.ImportInfo;
import xce.cardcache.ImportInfoNew;
import xce.cardcache.ImportValue;
import xce.cardcache.ImportValueII;
import xce.cardcache.ImportValueNew;
import xce.cardcache.PartNewInfo;
import xce.cardcache.PartOfCardInfo;
import xce.cardcache.RelationInfo;
import xce.cardsearch.CardSearcher;
import xce.cardsearch.FriendCardList;

public class CardFacadeAdapter {

	private final static int ADD = 0;
	private final static int RECEIVE = 1;
	private final static int ACCEPT = 2;
	private final static int REMOVE = 3;
	private CardRelationCacheAdapter relationAdapter = new CardRelationCacheAdapter();
	private CardInfoCacheAdapter infoAdapter = new CardInfoCacheAdapter();
	private CardImportCacheAdapter importAdapter = new CardImportCacheAdapter();
	private CardSearcherAdapter searchAdapter = new CardSearcherAdapter();
	private CardImportSearcherAdapter importSearchAdapter = new CardImportSearcherAdapter();
	private UserCacheAdapter<DefaultUserCache> userCacheAdapter = new UserCacheAdapter<DefaultUserCache>(
			new DefaultUserCacheFactory());

	public CardDetailInfo getCardDetailInfo(int userId) {
		List<Integer> userIds = new ArrayList<Integer>();
		userIds.add(userId);
		CardInfo[] infos = infoAdapter.get(userIds);
		if (infos.length == 1) {
			CardDetailInfo info = new CardDetailInfo(infos[0]);
			return info;
		} else
			return null;
	}

	public boolean setCardDetailInfo(CardDetailInfo info) {
		return infoAdapter.set(info.getCardInfo());
	}

	public String getPrivacy(int userId) {
		return infoAdapter.getPrivacy(userId);
	}

	public boolean setPrivacy(int userId, String privacy) {
		return infoAdapter.setPrivacy(userId, privacy);
	}

	int binary_search(int[] seq, int id) {
		int lower = 0;
		int high = seq.length - 1;
		while (lower <= high) {
			int mid = (lower + high) / 2;
			if (seq[mid] == id) {
				return mid;
			} else if (seq[mid] < id) {
				lower = mid + 1;
			} else {
				high = mid - 1;
			}
		}
		return -1;
	}

	CardShowInfo getShowInfo(int userId, CardInfo[] infos, int type, String name) {
		int low = 0;
		int high = infos.length;
		int mid = -1;
		while (low <= high) {
			mid = (low + high) / 2;
			if (infos[mid].userId == userId) {
				break;
			} else if (infos[mid].userId < userId) {
				low = mid + 1;
			} else {
				high = mid - 1;
			}
		}
		if (mid != -1 && low <= high) {
			CardShowInfo info = new CardShowInfo();
			info.setMobile(infos[mid].mobile);
			info.setPrivacy(infos[mid].privacy);
			info.setQq(infos[mid].qq);
			info.setType(type);
			info.setUserId(userId);
			info.setName(name);
			return info;
		}
		return null;
	}

	public List<Integer> getUnExchange(int userId) {
		int[] exchange = relationAdapter.get(userId, 0, -1, ACCEPT);
		int[] receive = relationAdapter.get(userId, 0, -1, RECEIVE);
		int[] add = relationAdapter.get(userId, 0, -1, ADD);
		List<Integer> result = new ArrayList<Integer>();
		List<Integer> friends = BuddyByNameCacheAdapter.getInstance()
				.getFriendList(userId, -1);

		for (Integer friend : friends) {
			int x = binary_search(exchange, friend);
			if (x != -1) {
				continue;
			}

			int y = binary_search(receive, friend);
			if (y != -1) {
				result.add(friend);
				continue;
			}
			int z = binary_search(add, friend);
			if (z != -1) {
				continue;
			}
			result.add(friend);
		}
		return result;
	}

	public int[] getTypeCards(int userId, int type) {
		if (type != REMOVE) {
			return relationAdapter.get(userId, 0, -1, type);
		} else {
			List<Integer> list = searchAdapter.getUnExchangeList(userId);
			int[] res = new int[list.size()];
			for (int i = 0; i < list.size(); ++i) {
				res[i] = list.get(i);
			}
			return res;
		}
	}

	public int[] getAcceptUser(int userId) {
		return relationAdapter.get(userId, 0, -1, ACCEPT);
	}

	public List<CardShowInfo> getFriendsCards(int userId) {
		PartNewInfo[] infos = searchAdapter.getFriendCardNew(userId);

		List<CardShowInfo> shows = new ArrayList<CardShowInfo>(infos.length);
		for (PartNewInfo info : infos) {
			CardShowInfo show = new CardShowInfo(info);
			shows.add(show);
		}
		return shows;
	}

	public List<Integer> getApplyList(int userId, int begin, int limit) {
		int[] ids = relationAdapter.get(userId, 0, -1, RECEIVE);
		List<Integer> res = new ArrayList<Integer>();
		for (int id : ids) {
			res.add(id);
		}
		return res;
	}

	public int getApplySize(int userId) {
		return relationAdapter.getSize(userId, ADD);
	}

	public int getReceiveSize(int userId) {
		return relationAdapter.getSize(userId, RECEIVE);
	}

	public int[] getReceive(int userId, int begin, int limit) {
		return relationAdapter.getReceive(userId, begin, limit);
	}

	public boolean ignoreReceive(int userId, int[] friendIds) {
		return relationAdapter.ignoreReceive(userId, friendIds);
	}

	public boolean ignoreAll(int userId) {
		return relationAdapter.ignoreAll(userId);
	}

	public Map<Integer, int[]> getAccept(long time, int lastId, int limit) {
		return relationAdapter.getAccept(time, lastId, limit);
	}

	public int getImportCount(int userId) {
		return 0;
	}

	public CardShowInfo[] searchCard(int userId, String query, int limit) {
		Map<Integer, Integer> result = searchAdapter.search(userId, query,
				limit);
		List<Integer> uids = new ArrayList<Integer>();

		for (Integer id : result.keySet()) {
			uids.add(id);
		}
		Map<Integer, DefaultUserCache> userCache = userCacheAdapter
				.getUserCacheMap(uids);
		System.out.println(uids.size());
		PartOfCardInfo[] infos = infoAdapter.getPart(uids);
		CardShowInfo[] showInfos = new CardShowInfo[infos.length];
		for (int i = 0; i < infos.length; ++i) {
			showInfos[i] = new CardShowInfo();
			showInfos[i].setMobile(infos[i].mobile);
			showInfos[i].setQq(infos[i].qq);
			showInfos[i].setPrivacy(infos[i].privacy);
			showInfos[i].setUserId(infos[i].userId);
			showInfos[i].setName(userCache.get(infos[i].userId).getName());
			showInfos[i].setType(result.get(infos[i].userId));
		}
		return showInfos;
	}

	public CardShowInfo[] searchCardNew(int userId, String query, int limit) {
		Map<Integer, Integer> result = searchAdapter.search(userId, query,
				limit);
		List<Integer> uids = new ArrayList<Integer>();

		for (Integer id : result.keySet()) {
			uids.add(id);
		}
		Map<Integer, DefaultUserCache> userCache = userCacheAdapter
				.getUserCacheMap(uids);
		System.out.println(uids.size());
		CardInfo[] infos = infoAdapter.get(uids);
		CardShowInfo[] showInfos = new CardShowInfo[infos.length];
		for (int i = 0; i < infos.length; ++i) {
			showInfos[i] = new CardShowInfo();
			showInfos[i].setMobile(infos[i].mobile);
			showInfos[i].setQq(infos[i].qq);
			showInfos[i].setPrivacy(infos[i].privacy);
			showInfos[i].setUserId(infos[i].userId);
			showInfos[i].setEmail(infos[i].email);
			showInfos[i].setMsn(infos[i].msn);
			showInfos[i].setName(userCache.get(infos[i].userId).getName());
			showInfos[i].setType(result.get(infos[i].userId));
		}
		return showInfos;
	}

	public void exchangeCards(int userId, int[] friendIds) {
		RelationInfo[] infos = new RelationInfo[friendIds.length];
		for (int i = 0; i < friendIds.length; ++i) {
			RelationInfo info = new RelationInfo();
			info.userId = userId;
			info.friendId = friendIds[i];
			info.requestFlag = 0;
			infos[i] = info;
		}
		relationAdapter.setBatch(infos);
	}

	public int exchangeCard(int userId, int friendId) {
		RelationInfo info = new RelationInfo();
		info.userId = userId;
		info.friendId = friendId;
		info.requestFlag = 0;
		return relationAdapter.set(info);
	}

	public boolean acceptCard(int userId, int friendId) {
		RelationInfo info = new RelationInfo();
		info.userId = userId;
		info.friendId = friendId;
		info.requestFlag = 1;
		relationAdapter.set(info);
		return true;
	}

	public int getRelation(int userId, int friendId) {
		return relationAdapter.getRelation(userId, friendId);
	}

	public ImportDetailInfo[] getImportInfo(int userId, int limit) {
		ImportInfo[] infos = importAdapter.get(userId, limit);
		ImportDetailInfo[] detailInfos = new ImportDetailInfo[infos.length];
		for (int i = 0; i < infos.length; ++i) {
			detailInfos[i] = new ImportDetailInfo();
			detailInfos[i].setUserId(infos[i].userId);
			detailInfos[i].setImportWay(infos[i].importWay);
			detailInfos[i].setImportDetail(infos[i].importDetail);
			detailInfos[i].setMsn(infos[i].msn);
			detailInfos[i].setEmail(infos[i].email);
			detailInfos[i].setName(infos[i].name);
			detailInfos[i].setMobile(infos[i].mobile);
			detailInfos[i].setQq(infos[i].qq);
			detailInfos[i].setAddress(infos[i].address);
			detailInfos[i].setExtra(infos[i].extra);
			detailInfos[i].setRequestCount(infos[i].requestCount);
		}
		return detailInfos;
	}

	public ImportDetailInfo getImportDetailInfo(int userId, byte importWay,
			String importDetail) {
		ImportInfo importInfo = importAdapter.getDetail(userId, importWay,
				importDetail);
		if (importInfo != null) {
			ImportDetailInfo info = new ImportDetailInfo(importInfo);
			return info;
		} else {
			return null;
		}
	}

	public ImportDetailInfo[] getImportList(int userId) {
		ImportValue[] infos = importAdapter.getList(userId);
		ImportDetailInfo[] values = new ImportDetailInfo[infos.length];
		for (int i = 0; i < infos.length; ++i) {
			values[i] = new ImportDetailInfo();
			values[i].setImportWay(infos[i].importWay);
			values[i].setImportDetail(infos[i].importDetail);
			values[i].setName(infos[i].name);
		}
		return values;
	}

	public boolean setImportDetailInfo(ImportDetailInfo[] infos) {
		ImportInfo[] importinfos = new ImportInfo[infos.length];

		for (int i = 0; i < infos.length; ++i) {
			importinfos[i] = infos[i].getImportInfo();
		}

		importAdapter.set(importinfos);
		return true;
	}

	public boolean importDetailInfo(ImportDetailInfo[] infos) {
		ImportInfo[] importinfos = new ImportInfo[infos.length];

		for (int i = 0; i < infos.length; ++i) {
			importinfos[i] = infos[i].getImportInfo();
		}

		importAdapter.load(importinfos);
		return true;
	}

	public ImportDetailInfo[] getImportInfoNew(int userId, int limit) {
		ImportInfoNew[] infos = importAdapter.getNew(userId, limit);
		ImportDetailInfo[] detailInfos = new ImportDetailInfo[infos.length];
		for (int i = 0; i < infos.length; ++i) {
			detailInfos[i] = new ImportDetailInfo();
			detailInfos[i].setId(infos[i].id);
			detailInfos[i].setUserId(infos[i].userId);
			detailInfos[i].setImportWay(infos[i].importWay);
			detailInfos[i].setImportDetail(infos[i].importDetail);
			detailInfos[i].setMsn(infos[i].msn);
			detailInfos[i].setEmail(infos[i].email);
			detailInfos[i].setName(infos[i].name);
			detailInfos[i].setMobile(infos[i].mobile);
			detailInfos[i].setQq(infos[i].qq);
			detailInfos[i].setAddress(infos[i].address);
			detailInfos[i].setExtra(infos[i].extra);
			detailInfos[i].setRequestCount(infos[i].requestCount);
			detailInfos[i].setSubscribeFlag(infos[i].subscribeFlag);
		}
		return detailInfos;
	}

	public ImportDetailInfo getImportDetailInfoNew(int userId, byte importWay,
			String importDetail) {
		ImportInfoNew importInfo = importAdapter.getDetailNew(userId,
				importWay, importDetail);
		if (importInfo != null) {
			ImportDetailInfo info = new ImportDetailInfo(importInfo);
			return info;
		} else {
			return null;
		}
	}

	public ImportDetailInfo getImportDetailInfoByEmail(int userId, String email) {
		ImportInfoNew importInfo = importAdapter
				.getDetailByEmail(userId, email);
		if (importInfo != null) {
			ImportDetailInfo info = new ImportDetailInfo(importInfo);
			return info;
		} else {
			return null;
		}
	}

	public ImportDetailInfo[] getImportListNew(int userId) {
		ImportValueNew[] infos = importAdapter.getListNew(userId);
		ImportDetailInfo[] values = new ImportDetailInfo[infos.length];
		for (int i = 0; i < infos.length; ++i) {
			values[i] = new ImportDetailInfo();
			values[i].setId(infos[i].id);
			values[i].setImportWay(infos[i].importWay);
			values[i].setImportDetail(infos[i].importDetail);
			values[i].setName(infos[i].name);
		}
		return values;
	}

	public boolean setImportDetailInfoNew(ImportDetailInfo[] infos) {
		ImportInfoNew[] importinfos = new ImportInfoNew[infos.length];

		for (int i = 0; i < infos.length; ++i) {
			importinfos[i] = infos[i].getImportInfoNew();
		}

		importAdapter.setNew(importinfos);
		return true;
	}

	public boolean importDetailInfoNew(ImportDetailInfo[] infos) {
		ImportInfoNew[] importinfos = new ImportInfoNew[infos.length];

		for (int i = 0; i < infos.length; ++i) {
			importinfos[i] = infos[i].getImportInfoNew();
		}
		importAdapter.loadNew(importinfos);
		return true;
	}

	public ImportDetailInfo[] importDetailInfoII(ImportDetailInfo[] infos) {
		ImportInfoNew[] importinfos = new ImportInfoNew[infos.length];

		for (int i = 0; i < infos.length; ++i) {
			importinfos[i] = infos[i].getImportInfoNew();
		}
		ImportInfoNew[] results = importAdapter.loadII(importinfos);
		ImportDetailInfo[] detailInfos = new ImportDetailInfo[results.length];
		for (int i = 0; i < infos.length; ++i) {
			detailInfos[i] = new ImportDetailInfo();
			detailInfos[i].setId(results[i].id);
			detailInfos[i].setUserId(results[i].userId);
			detailInfos[i].setImportWay(results[i].importWay);
			detailInfos[i].setImportDetail(results[i].importDetail);
			detailInfos[i].setMsn(results[i].msn);
			detailInfos[i].setEmail(results[i].email);
			detailInfos[i].setName(results[i].name);
			detailInfos[i].setMobile(results[i].mobile);
			detailInfos[i].setQq(results[i].qq);
			detailInfos[i].setAddress(results[i].address);
			detailInfos[i].setExtra(results[i].extra);
			detailInfos[i].setRequestCount(results[i].requestCount);
			detailInfos[i].setSubscribeFlag(results[i].subscribeFlag);
		}
		return detailInfos;
	}

	public ImportDetailInfo[] getIdImportDetail(int userId) {
		ImportIdImportDetail[] infos = importAdapter.getIdImportDetail(userId);
		ImportDetailInfo[] values = new ImportDetailInfo[infos.length];
		for (int i = 0; i < infos.length; ++i) {
			values[i] = new ImportDetailInfo();
			values[i].setId(infos[i].id);
			values[i].setImportDetail(infos[i].importDetail);
		}
		return values;
	}

	public boolean setSubscribeFlag(ImportDetailInfo[] infos) {
		ImportInfoNew[] importinfos = new ImportInfoNew[infos.length];

		for (int i = 0; i < infos.length; ++i) {
			importinfos[i] = new ImportInfoNew();
			importinfos[i].id = infos[i].getId();
			importinfos[i].userId = infos[i].getUserId();
			importinfos[i].subscribeFlag = infos[i].getSubscribeFlag();
		}
		importAdapter.setSubscribeFlag(importinfos);
		return true;
	}

	public boolean isContact(int userId, String email, int eType) {
		return importAdapter.isContact(userId, email, eType);
	}

	public void removeImportDetailInfo(ImportDetailInfo[] infos) {
		ImportInfo[] importinfos = new ImportInfo[infos.length];

		for (int i = 0; i < infos.length; ++i) {
			importinfos[i] = infos[i].getImportInfo();
		}
		importAdapter.remove(importinfos);
	}

	public ImportValueII[] getListII(int userId) {
		return importAdapter.getListII(userId);
	}

	public ImportDetailInfo[] getListIINew(int userId) {
		ImportValueII[] values = importAdapter.getListII(userId);
		ImportDetailInfo[] infos = new ImportDetailInfo[values.length];
		for (int i = 0; i < values.length; ++i) {
			infos[i] = new ImportDetailInfo();
			infos[i].setId(values[i].id);
			infos[i].setImportDetail(values[i].importDetail);
			infos[i].setImportWay(values[i].importWay);
			infos[i].setName(values[i].name);
			infos[i].setSubscribeFlag(values[i].subscribeFlag);
		}
		return infos;
	}

	public int[] getImportUser(int userId) {
		return importAdapter.getImportUser(userId);
	}

	public ImportDetailInfo[] getImportEmail(int offset, int limit) {
		ImportEmail[] emails = importAdapter.getImportEmail(offset, limit);

		ImportDetailInfo[] values = new ImportDetailInfo[emails.length];

		for (int i = 0; i < emails.length; ++i) {
			values[i] = new ImportDetailInfo();
			values[i].setId(emails[i].id);
			values[i].setUserId(emails[i].userId);
			values[i].setEmail(emails[i].email);
		}
		return values;
	}

	public boolean isImported(int userId) {
		return importAdapter.isImported(userId);
	}

	public int[] friendsImported(int[] friendIds) {
		return importAdapter.friendsImported(friendIds);
	}

	public int[] getImportUserId(int begin, int limit) {
		return importAdapter.getImportUserId(begin, limit);
	}

	public ImportDetailInfo[] searchImportCard(int userId, String query,
			int limit) {
		ImportValue[] infos = importSearchAdapter.searchImport(userId, query,
				limit);
		ImportDetailInfo[] values = new ImportDetailInfo[infos.length];
		for (int i = 0; i < infos.length; ++i) {
			values[i] = new ImportDetailInfo();
			values[i].setImportWay(infos[i].importWay);
			values[i].setImportDetail(infos[i].importDetail);
			values[i].setName(infos[i].name);
		}
		return values;
	}

	public ImportDetailInfo[] searchImportCardNew(int userId, String query,
			int limit) {
		ImportValueNew[] infos = importSearchAdapter.searchImportNew(userId,
				query, limit);
		ImportDetailInfo[] values = new ImportDetailInfo[infos.length];
		for (int i = 0; i < infos.length; ++i) {
			values[i] = new ImportDetailInfo();
			values[i].setId(infos[i].id);
			values[i].setImportWay(infos[i].importWay);
			values[i].setImportDetail(infos[i].importDetail);
			values[i].setName(infos[i].name);
		}
		return values;
	}

	/**
	 * @param args
	 */
	public static void main(String[] args) {
		// TODO Auto-generated method stub
		CardFacadeAdapter adapter = new CardFacadeAdapter();

		CardDetailInfo info = adapter.getCardDetailInfo(322012570);
		System.out.println(info.toString());

		// CardDetailInfo info = adapter.getCardDetailInfo(67067835);
		// System.out.println("info : " + info.toString());

		// adapter.ignoreAll(2001);
		// int userId = 67067835;
		//
		// ImportDetailInfo[] detail = new ImportDetailInfo[2];
		//
		// detail[0] = new ImportDetailInfo();
		// detail[0].setUserId(33333333);
		// detail[0].setImportWay((byte) 10);
		// detail[0].setImportDetail("6@gmail.com");
		// detail[0].setMsn("");
		// detail[0].setEmail("");
		// detail[0].setName(")({)ccChen(})");
		// detail[0].setMobile("B");
		// detail[0].setQq("B");
		// detail[0].setAddress("B");
		// detail[0].setExtra("B");
		// detail[0].setRequestCount(5);
		//
		// detail[1] = new ImportDetailInfo();
		// detail[1].setUserId(33333333);
		// detail[1].setImportWay((byte) 10);
		// detail[1].setImportDetail("15@gmail.com");
		// detail[1].setMsn("m");
		// detail[1].setEmail("test1@gmail.com");
		// detail[1].setName("wangliu");
		// detail[1].setMobile("e");
		// detail[1].setQq("E");
		// detail[1].setAddress("E");
		// detail[1].setExtra("e");
		// detail[1].setRequestCount(5);
		//
		// long t1 = System.currentTimeMillis();
		// Map<Integer, int[]> acceptInfo = adapter.getAccept(t1, 1001, 10);
		// for (Map.Entry<Integer, int[]> entry : acceptInfo.entrySet()) {
		// System.out.print("userid:" + entry.getKey() + ":");
		// for (int fid : entry.getValue()) {
		// System.out.print(fid + " ");
		// }
		// System.out.println("");
		// }

		// boolean res1 = adapter.importDetailInfo(detail);
		// System.out.println(res1);
		//

		// ImportDetailInfo[] infos = adapter.importDetailInfoII(detail);
		// for (ImportDetailInfo info : infos) {
		// System.out.println(info.toStringNew());
		// }

		System.exit(0);
	}
}
