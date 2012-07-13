import java.util.ArrayList;
import java.util.Iterator;
import java.util.List;
import java.util.Map;
import java.util.Map.Entry;
import java.util.PriorityQueue;

import com.xiaonei.xce.buddybyidcache.BuddyByIdCacheAdapter;
import com.xiaonei.xce.log.TimeCost;

public class FriendUtil {

	public static class KNode implements Comparable<KNode> {
		public KNode(int type, Iterator<Integer> iter) {
			this.cur = iter.next();
			this.type = type;
			this.iter = iter;
		}

		int cur;
		int type;
		Iterator<Integer> iter;

		@Override
		public int compareTo(KNode other) {
			// TODO Auto-generated method stub
			if (this.cur == other.cur) {
				return this.type - other.type;
			} else {
				return this.cur - other.cur;
			}
		}
	}

	public static void getFriendOfFriend(int userId, ArrayList<Integer> fids,
			ArrayList<Integer> commons) {
	//	TimeCost cost = TimeCost.begin("buddy cost");
		List<Integer> friend = BuddyByIdCacheAdapter.getInstance()
				.getFriendListAsc(userId, 1000);
		System.out.println("get 1 degree friends : " + friend.size());
		Map<Integer, List<Integer>> friends = BuddyByIdCacheAdapter
				.getInstance().getFriendListsAsc(friend);
	//	cost.endStep("Get Buddy");
		PriorityQueue<KNode> pq = new PriorityQueue<KNode>();
		if (friend.iterator().hasNext()) {
			pq.add(new KNode(1, friend.iterator()));
		}

		for (Entry<Integer, List<Integer>> entry : friends.entrySet()) {
			if (entry.getValue().iterator().hasNext())
				pq.add(new KNode(2, entry.getValue().iterator()));
		}

		int pre = -1;
		int cnt = 0;
		int type = -1;
		while (!pq.isEmpty()) {
			KNode node = pq.poll();
			if (node.cur == pre) {
				cnt++;
			} else {
				FriendCommonInfo info = new FriendCommonInfo();
				info.commonNum = (short) cnt;
				info.type = (short) type;
				info.userid = pre;
				if (pre == userId) {
					info.type = 0;
					info.commonNum = (short) friend.size();
				}
				if (pre > 0) {
					fids.add(info.userid);
					commons.add(info.type << 16 | (info.commonNum & 0xFFFF));
				}
				pre = node.cur;
				cnt = 1;
				type = node.type;
			}
			if (node.iter.hasNext()) {
				node.cur = node.iter.next();
				pq.add(node);
			}
		}
	//	cost.endStep("Merge Cost");
	}

	public static void main(String[] args) {
		int Cluster = 8;
		ArrayList<Integer> fids = new ArrayList<Integer>();
		ArrayList<Integer> commons = new ArrayList<Integer>();
		getFriendOfFriend(220840375, fids, commons);
		ArrayList<ArrayList<Integer>> suffixFids = new ArrayList<ArrayList<Integer>>();
		ArrayList<ArrayList<Integer>> suffixCommons = new ArrayList<ArrayList<Integer>>();
		for (int i = 0; i < Cluster; ++i) {
			suffixFids.add(new ArrayList<Integer>());
			suffixCommons.add(new ArrayList<Integer>());
		}
		for (int i = 0; i < fids.size(); ++i) {
			int n = fids.get(i) % Cluster;
			suffixFids.get(n).add(fids.get(i));
			suffixCommons.get(n).add(commons.get(i));
		}
	}
}
