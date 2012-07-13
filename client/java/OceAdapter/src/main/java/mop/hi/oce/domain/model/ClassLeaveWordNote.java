package mop.hi.oce.domain.model;

import java.util.Date;

import mop.hi.svc.classleaveword.Note;
import xce.util.tools.DateFormatter;


public class ClassLeaveWordNote {

	private Note note = new Note();

	public Note getNote() {
		return note;
	}

	public String getBody() {
		return note.body;

	}

	public void setBody(String body) {
		this.note.body = body;

	}

	public long getMemberId() {
		return this.note.memberId;
	}

	public void setMemberId(long memberId) {
		this.note.memberId = memberId;

	}

	public String getMemberName() {
		return this.note.memberName;
	}

	public void setMemberName(String memberName) {
		this.note.memberName = memberName;

	}

	public Date getSubjectTime() {
		return DateFormatter.parse(note.subjectTime);
	}

	public void setSubjectTime(Date subjectTime) {
		this.note.subjectTime = DateFormatter.format(subjectTime);

	}

	public int getSubjectType() {
		return this.note.subjectType;
	}

	public void setSubjectType(int subjectType) {
		this.note.subjectType = subjectType;

	}

	public String getTinyUrl() {
		return this.note.tinyUrl;
	}

	public void setTinyUrl(String tinyUrl) {
		this.note.tinyUrl = tinyUrl;

	}

	public String getSubjectLink() {
		return this.note.subjectLink;
	}

	public void setSubjectLink(String subjectLink) {
		this.note.subjectLink = subjectLink;
	}

}
