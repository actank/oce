package mop.hi.oce.domain.feed;

import java.util.Date;

import mop.hi.svc.feed.FeedQuery;
import mop.hi.svc.feed.FromBody;

public class FeedQueryHolder {
	private FeedQuery feed;

	public FeedQueryHolder(FeedQuery feed) {
		this.feed = feed;
	}

	public int getNoteId() {
		return this.feed.feedId;
	}

	public FromBody[] getFromBody() {
		return this.feed.fromBodies;
	}

	public int getOwnerId() {
		return this.feed.ownerId;
	}

	public String getOwnerName() {
		return this.feed.ownerName;
	}

	public int getResource() {
		return this.feed.resource;
	}

	public String getType() {
		return this.feed.type;
	}

	public String getTitle() {
		return this.feed.title;
	}

	public byte[] getContent() {
		return this.feed.content;
	}

	public Date getTimestamp() {
		return new Date(this.feed.timestamp * 1000L);
	}

	public boolean getUnknown() {
		return this.feed.unknown;
	}

	public int getWeight() {
		return 0;
	}
	
	public String toString() {
		String result = "";
		result += getNoteId() + "\t";
		result += getTimestamp() + "\t";
		result += getResource() + "\t";
		result += getType() + "\t";
		result += getUnknown() + "\t";
		result += getOwnerId() + "\t";
		result += getOwnerName() + "\t";
		for (FromBody fb : getFromBody()) {
			result += fb.id + "|" + fb.name + ",";
		}
		result += "\t";		
//		result += "OwnerId: " + getOwnerId() + "\t";
//		result += "OwnerName: " + getOwnerName() + "\t";
		result += getTitle() + "\t";
//		result += "Content: " + getContent().length + "\t";
		
//		try {
//			ByteArrayInputStream bis = new ByteArrayInputStream(getContent());
//			ObjectInputStream is = new ObjectInputStream(bis);
//			is.close();
//		} catch (Exception e) {
//			e.printStackTrace();
//		}
		

		return result;
	}
}
