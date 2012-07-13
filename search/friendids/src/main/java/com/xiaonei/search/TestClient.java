package com.xiaonei.search;

import java.io.IOException;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.HashSet;
import java.util.List;
import java.util.Map;
import java.util.Map.Entry;
import java.util.Random;
import java.util.Scanner;
import java.util.Set;
import java.util.concurrent.ExecutionException;
import java.util.concurrent.Future;
import java.util.concurrent.TimeUnit;
import java.util.concurrent.TimeoutException;

import org.apache.lucene.search.DocIdSet;
import org.apache.lucene.search.DocIdSetIterator;
import org.jboss.netty.logging.InternalLoggerFactory;
import org.jboss.netty.logging.Log4JLoggerFactory;

import com.kamikaze.docidset.impl.IntArrayDocIdSet;
import com.kamikaze.docidset.impl.OrDocIdSet;
import com.linkedin.norbert.javacompat.cluster.ClusterClient;
import com.linkedin.norbert.javacompat.cluster.Node;
import com.linkedin.norbert.javacompat.cluster.ZooKeeperClusterClient;
import com.linkedin.norbert.javacompat.network.NettyNetworkClient;
import com.linkedin.norbert.javacompat.network.NetworkClient;
import com.linkedin.norbert.javacompat.network.NetworkClientConfig;
import com.linkedin.norbert.javacompat.network.RoundRobinLoadBalancerFactory;
import com.xiaonei.xce.buddybyidcache.BuddyByIdCacheAdapter;
import com.xiaonei.xce.fof.service.impl.CoreFriendOfFriendServiceImpl;
import com.xiaonei.xce.fof.service.impl.FriendOfFriendRequest;
import com.xiaonei.xce.fof.service.impl.FriendOfFriendResult;
import com.xiaonei.xce.fof.service.impl.PreloadFriendOfFriendServiceImpl;
import com.xiaonei.xce.fof.service.impl.PreloadRequest;
import com.xiaonei.xce.fof.service.impl.PreloadResult;

public class TestClient {
	private Map<Integer, List<Node>> partition2Node = new HashMap<Integer, List<Node>>();
	private Set<Node> nodes;
	public NetworkClient nc;
	private Random random = new Random();
	int[] totalOrder = new int[] { 222905898, 229399879, 235405804, 243009105,
			253925514, 265700373, 283859063, 323640180, 358516004 };

	public TestClient(String[] args) {
		InternalLoggerFactory.setDefaultFactory(new Log4JLoggerFactory());

		ClusterClient cc = new ZooKeeperClusterClient(null, args[0], args[1],
				30000);
		NetworkClientConfig config = new NetworkClientConfig();
		config.setClusterClient(cc);
		nc = new NettyNetworkClient(config, new RoundRobinLoadBalancerFactory());
		nodes = cc.getNodes();
		for (Node node : nodes) {
			Set<Integer> parts = node.getPartitionIds();
			for (Integer part : parts) {
				List<Node> pnodes = partition2Node.get(part);
				if (pnodes == null) {
					pnodes = new ArrayList<Node>();
					partition2Node.put(part, pnodes);
				}
				pnodes.add(node);
			}
		}
	}

	public Set<Node> getQueryNode() {
		Set<Node> res = new HashSet<Node>();
		for (Entry<Integer, List<Node>> entry : partition2Node.entrySet()) {
			List<Node> pnodes = entry.getValue();
			if (pnodes == null || pnodes.isEmpty()) {
				continue;
			}
			int size = pnodes.size();
			int index = random.nextInt(size);
			Node curNode = pnodes.get(index);
			res.add(curNode);
		}
		return res;
	}

	public Set<Node> getAllNodes() {
		return nodes;
	}

	public static int lower_bound(Integer id, List<Integer> friends) {
		if (friends.size() > 0 && friends.get(friends.size() - 1) > id) {
			return friends.size();
		}
		int low = 0, mid, high = friends.size() - 1;
		while (low < high) {
			mid = (low + high) >> 1;
			if (friends.get(mid).compareTo(id) >= 0)
				high = mid;
			else
				low = mid + 1;
		}
		return low;
	}

	public Map<Integer, Map<Integer, int[]>> getSubFriendList(
			Map<Integer, List<Integer>> f2f) {
		Map<Integer, Map<Integer, int[]>> res = new HashMap<Integer, Map<Integer, int[]>>();
		for (int i = 0; i <= totalOrder.length; ++i) {
			res.put(i, new HashMap<Integer, int[]>());
		}
		for (Entry<Integer, List<Integer>> entry : f2f.entrySet()) {
			List<Integer> friends = entry.getValue();
			List<List<Integer>> bucket = new ArrayList<List<Integer>>();
			for (int i = 0; i < 10; ++i) {
				bucket.add(new ArrayList<Integer>());
			}
			for (Integer id : friends) {
				int index = id % 10;
				bucket.get(index).add(id);
			}
			for (int i = 0; i < 10; ++i) {
				List<Integer> tmp = bucket.get(i);
				int size = tmp.size();
				int[] ids = new int[size];
				for (int j = 0; j < size; ++j) {
					ids[j] = tmp.get(j);
				}
				res.get(i).put(entry.getKey(), ids);
			}
			// int i = 0, j = 0;
			// int preIndex = 0;
			// while (i < totalOrder.length && j < friends.size()) {
			// if (friends.get(j) < totalOrder[i]) {
			// j++;
			// } else if (friends.get(j) >= totalOrder[i]) {
			// if (preIndex < j) {
			// List<Integer> subList = friends.subList(preIndex, j);
			// int[] ids = new int[subList.size()];
			// for (int k = 0; k < subList.size(); ++k) {
			// ids[k] = subList.get(k);
			// }
			// res.get(i).put(entry.getKey(), ids);
			// }
			// i++;
			// preIndex = j;
			// }
			// }
			// if (j < friends.size()) {
			// List<Integer> subList = friends.subList(j, friends.size());
			// int[] ids = new int[subList.size()];
			// for (int k = 0; k < subList.size(); ++k) {
			// ids[k] = subList.get(k);
			// }
			// res.get(i).put(entry.getKey(), ids);
			// }

		}
		return res;
	}

	static public void loadIds(TestClient client, List<Integer> ids) {

		long t1 = System.currentTimeMillis();
		Map<Integer, List<Integer>> f2f = BuddyByIdCacheAdapter.getInstance()
				.getFriendListsAsc(ids);
		long t2 = System.currentTimeMillis();
		System.out.println("read from buddy cost " + (t2 - t1));
		Map<Integer, Map<Integer, int[]>> res = client.getSubFriendList(f2f);
		long t3 = System.currentTimeMillis();
		System.out.println("split buddy cost " + (t3 - t2));
		Set<Node> nodes = client.getAllNodes();
		List<Future> futureList = new ArrayList<Future>(nodes.size());
		for (Node node : nodes) {
			PreloadRequest request = new PreloadRequest();
			Set<Integer> partitions = node.getPartitionIds();
			Map<Integer, Map<Integer, int[]>> tmp = new HashMap<Integer, Map<Integer, int[]>>();
			for (Integer part : partitions) {
				Map<Integer, int[]> value = res.get(part);
				if (value != null)
					tmp.put(part, value);
			}
			request.setUpdate(tmp);
			Future<PreloadResult> f = client.nc.sendRequestToNode(request,
					node, PreloadFriendOfFriendServiceImpl.SERIALIZER);
			futureList.add(f);
		}
		for (Future<PreloadResult> f : futureList) {
			try {
				PreloadResult result = f.get();
			} catch (InterruptedException e) {
				e.printStackTrace();
			} catch (ExecutionException e) {
				e.printStackTrace();
			}
		}
	}

	public static List<Integer> OrSet(Map<Integer, List<Integer>> fids)
			throws IOException {
		List<DocIdSet> docSets = new ArrayList<DocIdSet>(fids.size());
		for (Entry<Integer, List<Integer>> entry : fids.entrySet()) {
			List<Integer> ids = entry.getValue();
			if (!ids.isEmpty()) {
				IntArrayDocIdSet intSet = new IntArrayDocIdSet(ids.size());
				for (Integer id : ids) {
					intSet.addDoc(id);
				}
				docSets.add(intSet);
			}
		}
		OrDocIdSet orSet = new OrDocIdSet(docSets);
		DocIdSetIterator iter = orSet.iterator();
		List<Integer> res = new ArrayList<Integer>();

		int docId = iter.nextDoc();
		while (docId != DocIdSetIterator.NO_MORE_DOCS) {
			res.add(docId);
			docId = iter.nextDoc();
		}
		return res;
	}

	static public List<FriendOfFriendResult> getFriendOfFriendList(
			TestClient client, int uid, List<Integer> ids) {
		List<FriendOfFriendResult> res = new ArrayList<FriendOfFriendResult>();
		Set<Node> nodes = client.getQueryNode();

		List<Future<FriendOfFriendResult>> futureList = new ArrayList<Future<FriendOfFriendResult>>(
				nodes.size());

		long t = System.currentTimeMillis();
		FriendOfFriendRequest request = new FriendOfFriendRequest();
		request.setUid(uid);
		request.setFriends(ids);
		for (Node node : nodes) {
			Future<FriendOfFriendResult> f = client.nc.sendRequestToNode(
					request, node, CoreFriendOfFriendServiceImpl.SERIALIZER);
			futureList.add(f);
		}
		System.out.println("send request " + (System.currentTimeMillis() - t));
		for (Future<FriendOfFriendResult> f : futureList) {
			try {
				long t1 = System.currentTimeMillis();
				FriendOfFriendResult result = f.get();
				long t2 = System.currentTimeMillis();
				System.out.println("waiting future " + (t2 - t1));
				res.add(result);
			} catch (InterruptedException e) {
				e.printStackTrace();
			} catch (ExecutionException e) {
				e.printStackTrace();
			}
		}

		return res;
	}

	public static void main(String[] args) throws IOException {
		TestClient client = new TestClient(args);
		Scanner scanner = new Scanner(System.in);
		String line;
		String mode = args[2];
		while ((line = scanner.nextLine()) != null) {
			System.out.println(line);
			Integer uid = Integer.valueOf(line);
			// String[] cmd = line.split(" ");
			if (mode.equals("load")) {
				long t1 = System.currentTimeMillis();
				List<Integer> ids = BuddyByIdCacheAdapter.getInstance()
						.getFriendListAsc(uid, 6000);
				long t2 = System.currentTimeMillis();
				loadIds(client, ids);
				long t3 = System.currentTimeMillis();
				System.err.println("read buddy " + (t2 - t1) + " upload cost "
						+ (t3 - t2));
			} else if (mode.equals("get")) {
				long t1 = System.currentTimeMillis();
				List<Integer> ids = BuddyByIdCacheAdapter.getInstance()
						.getFriendListAsc(uid, 6000);
				List<FriendOfFriendResult> res = getFriendOfFriendList(client,
						uid, ids);
				long t2 = System.currentTimeMillis();

				int total = 0;
				StringBuilder builder = new StringBuilder();
				for (FriendOfFriendResult fr : res) {
					builder.append(" " + fr.getRes().length);
					total += fr.getRes().length;
				}
				System.err.println("getNorbert uid: " + uid + " 1degree: "
						+ ids.size() + " 2degree: " + total + " cost: "
						+ (t2 - t1));
				System.out.println("total : " + total + " partitions : "
						+ builder.toString());
			} else if (mode.equals("old")) {
				long t1 = System.currentTimeMillis();

				List<Integer> ids = BuddyByIdCacheAdapter.getInstance()
						.getFriendListAsc(uid, 6000);
				Map<Integer, List<Integer>> fids = BuddyByIdCacheAdapter
						.getInstance().getFriendListsAsc(ids);
				long t2 = System.currentTimeMillis();
				System.out.println("get from buddy cost " + (t2 - t1));
				int total = 0;
				if (!fids.isEmpty())
					total = OrSet(fids).size();
				long t3 = System.currentTimeMillis();
				System.err.println("getCurrent uid: " + uid + " 1degree: "
						+ ids.size() + " 2degree: " + total + " getCost: "
						+ (t2 - t1) + " mergeCost: " + (t3 - t2) + " cost: "
						+ (t3 - t1));
			}
		}
		// cc.shutdown();
	}
}
