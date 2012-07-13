package com.xiaonei.xce.fof.service.impl;

import java.io.IOException;
import java.util.ArrayList;
import java.util.List;
import java.util.concurrent.Callable;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;
import java.util.concurrent.Future;
import java.util.concurrent.TimeUnit;

import org.apache.log4j.Logger;
import org.apache.lucene.search.DocIdSet;
import org.apache.lucene.search.DocIdSetIterator;

import com.kamikaze.docidset.impl.IntArrayDocIdSet;
import com.kamikaze.docidset.impl.OrDocIdSet;
import com.linkedin.norbert.network.JavaSerializer;
import com.linkedin.norbert.network.Serializer;
import com.sensei.search.req.protobuf.SenseiReqProtoSerializer;
import com.xiaonei.xce.fof.service.AbstractNorbertCoreService;

public class CoreFriendOfFriendServiceImpl extends
		AbstractNorbertCoreService<FriendOfFriendRequest, FriendOfFriendResult> {
	// public static final Serializer<FriendOfFriendRequest,
	// FriendOfFriendResult> SERIALIZER = JavaSerializer
	// .apply("FriendOfFriendRequest", FriendOfFriendRequest.class,
	// FriendOfFriendResult.class);
	public static final Serializer<FriendOfFriendRequest, FriendOfFriendResult> SERIALIZER = new SenseiReqProtoSerializer();

	private final ExecutorService _executorService = Executors
			.newCachedThreadPool();
	private static final Logger logger = Logger
			.getLogger(CoreFriendOfFriendServiceImpl.class);

	private FriendCore _core;

	public CoreFriendOfFriendServiceImpl(FriendCore core) {
		_core = core;
	}

	FriendOfFriendResult process(int partition, List<Integer> friends)
			throws IOException {
		List<DocIdSet> docSets = new ArrayList<DocIdSet>(friends.size());
		List<int[]> fids = _core.getFriendIds(partition, friends);
		System.out.println("find finds " + fids.size());

		for (int[] ids : fids) {
			IntArrayDocIdSet intSet = new IntArrayDocIdSet();
			intSet.addDocs(ids, 0, ids.length);
			docSets.add(intSet);
		}
		OrDocIdSet orSet = new OrDocIdSet(docSets);
		DocIdSetIterator iter = orSet.iterator();
		List<Integer> res = new ArrayList<Integer>();

		int docId = iter.nextDoc();
		while (docId != DocIdSetIterator.NO_MORE_DOCS) {
			res.add(docId);
			docId = iter.nextDoc();
		}

		int[] arr = new int[res.size()];
		for (int i = 0; i < res.size(); ++i) {
			arr[i] = res.get(i);
		}
		FriendOfFriendResult result = new FriendOfFriendResult();
		result.setRes(arr);
		return result;
	}

	FriendOfFriendResult merge(int total, List<FriendOfFriendResult> results) {
		long t1 = System.currentTimeMillis();
		int[] res = new int[total];
		int index = 0;
		// List<Integer> res = new ArrayList<Integer>(total);
		for (int i = 0; i < results.size(); ++i) {
			// res.addAll(results.get(i).getRes());
			System.arraycopy(results.get(i).getRes(), 0, res, index, results
					.get(i).getRes().length);
			index += results.get(i).getRes().length;
		}
		FriendOfFriendResult result = new FriendOfFriendResult();
		result.setRes(res);
		long t2 = System.currentTimeMillis();
		System.out.println("merge cost : " + (t2 - t1));
		return result;
	}

	@Override
	public FriendOfFriendResult getEmptyResultInstance(Throwable error) {
		return new FriendOfFriendResult();
	}

	@Override
	public Serializer<FriendOfFriendRequest, FriendOfFriendResult> getSerializer() {
		return SERIALIZER;
	}

	@Override
	public FriendOfFriendResult execute(final FriendOfFriendRequest req) {

		System.out.println("execute friends " + req.getUid() + " timestamp: "
				+ System.currentTimeMillis() + " friends: "
				+ req.getFriends().size());
		int[] partitions = _core._partitions;
		if (partitions != null && partitions.length > 0) {
			if (logger.isDebugEnabled()) {
				logger.debug("serving partitions: " + partitions.toString());
			}
			final ArrayList<FriendOfFriendResult> resultList = new ArrayList<FriendOfFriendResult>(
					partitions.length);
			Future<FriendOfFriendResult>[] futures = new Future[partitions.length - 1];
			int i = 0;
			int total = 0;
			final long start = System.currentTimeMillis();
			for (final int partition : partitions) {

				if (i < partitions.length - 1) // Search simultaneously.
				{
					try {
						futures[i] = (Future<FriendOfFriendResult>) _executorService
								.submit(new Callable<FriendOfFriendResult>() {
									public FriendOfFriendResult call()
											throws Exception {
										return process(partition,
												req.getFriends());
									}
								});
					} catch (Exception e) {
						logger.error(e.getMessage(), e);
					}
				} else // Reuse current thread.
				{
					try {
						FriendOfFriendResult res = process(partition,
								req.getFriends());
						resultList.add(res);
						total += res.getRes().length;
					} catch (Exception e) {
						logger.error(e.getMessage(), e);
						resultList.add(getEmptyResultInstance(e));
					}
				}
				++i;
			}

			for (i = 0; i < futures.length; ++i) {
				try {
					FriendOfFriendResult res = futures[i].get(1000,
							TimeUnit.MILLISECONDS);
					resultList.add(res);
					total += res.getRes().length;
				} catch (Exception e) {
					logger.error(e.getMessage(), e);
					resultList.add(getEmptyResultInstance(e));
				}
			}
			long end = System.currentTimeMillis();
			System.out.println("nodeId: " + _core._nodeId + " get fof cost : "
					+ (end - start) + " total: " + total);
			return merge(total, resultList);
		} else {
			if (logger.isInfoEnabled()) {
				logger.info("no partitions specified");
			}
			return getEmptyResultInstance(null);
		}
	}
}
