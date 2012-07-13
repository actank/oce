package mop.hi.oce.domain.feed;

import java.util.ArrayList;
import java.util.List;

public class FeedQuerySetHolder {
	private List<FeedQueryHolder> feeds = new ArrayList<FeedQueryHolder>();

	private int unknowns;

	private int total;

	public List<FeedQueryHolder> getFeeds() {
		return feeds;
	}

	public void setFeeds(List<FeedQueryHolder> events) {
		this.feeds = events;
	}

	public int getTotal() {
		return total;
	}

	public void setTotal(int totalCount) {
		this.total = totalCount;
	}

	public int getUnknowns() {
		return unknowns;
	}

	public void setUnknowns(int unreadCount) {
		this.unknowns = unreadCount;
	}
}
