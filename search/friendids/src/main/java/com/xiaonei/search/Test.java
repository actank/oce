package com.xiaonei.search;

import java.io.IOException;
import java.util.ArrayList;
import java.util.List;
import java.util.Map;
import java.util.Map.Entry;

import org.apache.lucene.search.DocIdSet;
import org.apache.lucene.search.DocIdSetIterator;

import com.kamikaze.docidset.impl.IntArrayDocIdSet;
import com.kamikaze.docidset.impl.OrDocIdSet;
import com.kamikaze.docidset.impl.PForDeltaDocIdSet;
import com.xiaonei.xce.buddybyidcache.BuddyByIdCacheAdapter;

public class Test {

	static int totalA = 0;
	static int totalB = 0;
	static int totalC = 0;

	public static long compressVInt(List<Integer> ids) throws IOException {
		ByteOutput out = new ByteOutput();
		int pre = 0;
		for (Integer id : ids) {
			out.writeVInt(id - pre);
			if (id - pre <= 0)
				System.out.println("error");
			pre = id;
		}
		return out.getByteSize();
	}

	public static long compressP4D(List<Integer> ids) throws IOException {
		PForDeltaDocIdSet p4dset = new PForDeltaDocIdSet(256);
		int pre = 0;
		for (Integer id : ids) {
			p4dset.addDoc(id - pre);
			pre = id;
		}
		System.out.println(p4dset.getCompressedBitSize()/8);
		return p4dset.sizeInBytes();
	}

	public static List<Integer> OrSet(Map<Integer, List<Integer>> fids)
			throws IOException {
		List<DocIdSet> docSets = new ArrayList<DocIdSet>(fids.size());
		for (Entry<Integer, List<Integer>> entry : fids.entrySet()) {
			List<Integer> ids = entry.getValue();
			IntArrayDocIdSet intSet = new IntArrayDocIdSet(ids.size());
			for (Integer id : ids) {
				intSet.addDoc(id);
			}
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
		return res;
	}

	public static void main(String[] args) throws IOException {
		List<Integer> ids = BuddyByIdCacheAdapter.getInstance()
				.getFriendListAsc(220840375, 6000);

		Map<Integer, List<Integer>> fids = BuddyByIdCacheAdapter.getInstance()
				.getFriendListsAsc(ids);
		List<Integer> res = OrSet(fids);
		System.out.println("ids size:" + res.size() + " IntSize:" + res.size()
				* 4 + " VIntSize:" + compressVInt(res) + " PForDelta:"
				+ compressP4D(res));
		// for (Entry<Integer, List<Integer>> entry : fids.entrySet()) {
		// totalA += entry.getValue().size() * 4;
		// System.out.println("id:" + entry.getKey() + " friend size:"
		// + entry.getValue().size() + " IntSize:"
		// + entry.getValue().size() * 4 + " VIntSize:"
		// + compressVInt(entry.getValue()));
		// // break;
		// }
	}
}
