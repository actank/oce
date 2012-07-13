package com.renren.messagepipe.producer;

import com.renren.messagepipe.common.Message;

public class MessageData {

	private String cluster;
	private String subject;
	private byte[] data;

	public MessageData(String cluster, String subject, byte[] bytes) {
		this.cluster = cluster;
		this.subject = subject;
		this.data = bytes;
	}
	

	public String getCluster() {
		return cluster;
	}

	public String getSubject() {
		return subject;
	}

	public byte[] getData() {
		return data;
	}

}
