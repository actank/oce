package com.xiaonei.xce.friendrank;

import java.util.Comparator;

public class ComparatorRankByScore implements Comparator<Rank> {
	public int compare(Rank o1, Rank o2) {
		return o2.getWeight() - o1.getWeight();
	}
}

