package mop.hi.oce.domain.feed;

import java.util.Date;

import mop.hi.svc.feed.FeedPublish;

public class FeedPublishHolder {
	private FeedPublish feed;

	public FeedPublishHolder() {
		this.feed = new FeedPublish();
	}

	public int getFromId() {
		return this.feed.fromId;
	}

	public void setFromId(int fromId) {
		this.feed.fromId = fromId;
	}

	public String getFromName() {
		return this.feed.fromName;
	}

	public void setFromName(String fromName) {
		this.feed.fromName = fromName;
	}

	public int getOwnerId() {
		return this.feed.ownerId;
	}

	public void setOwnerId(int ownerId) {
		this.feed.ownerId = ownerId;
	}

	public String getOwnerName() {
		return this.feed.ownerName;
	}

	public void setOwnerName(String ownerName) {
		this.feed.ownerName = ownerName;
	}

	public int getResource() {
		return this.feed.resource;
	}

	public void setResource(int resource) {
		this.feed.resource = resource;
	}
	public String getType() {
		return this.feed.type;
	}

	public void setType(String type) {
		this.feed.type = type;
	}
	
	public String getTitle() {
		return this.feed.title;
	}

	public void setTitle(String title) {
		this.feed.title = title;
	}

	public byte[] getContent() {
		return this.feed.content;
	}

	public void setContent(byte[] content) {
		this.feed.content = content;
	}
	public Date getTimestamp() {
		return new Date(this.feed.timestamp * 1000L);
	}

	public void setTimestamp(Date timestamp) {
		this.feed.timestamp = (int) (timestamp.getTime() / 1000L);
	}


	public FeedPublish getFeed() {
		return feed;
	}

	public String toString() {
		String result = "";
		result += "FromId: " + getFromId() + "\t";
		result += "FromName: " + getFromName() + "\t";
		result += "Resource: " + getResource() + "\t";
		result += "Type: " + getType() + "\t";
		result += "OwnerId: " + getOwnerId() + "\t";
		result += "OwnerName: " + getOwnerName() + "\t";
		result += "Title: " + getTitle() + "\t";
		result += "Content: " + getContent() + "\t";
		result += "Timestamp: " + getTimestamp() + "\t";
		return result;
	}
}
