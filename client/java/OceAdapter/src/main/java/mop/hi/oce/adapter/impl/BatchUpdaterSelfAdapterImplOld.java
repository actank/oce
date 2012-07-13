package mop.hi.oce.adapter.impl;

import mop.hi.oce.adapter.BatchUpdaterSelfAdapter;
import mop.hi.svc.batchtraits.UgcUserScoreName;
import mop.hi.svc.batchtraits.UserScoreName;
import mop.hi.svc.model.BatchUpdaterManagerPrx;
import mop.hi.svc.model.BatchUpdaterManagerPrxHelper;
import xce.util.channel.Channel;
import xce.util.service.ServiceAdapter;

public class BatchUpdaterSelfAdapterImplOld extends ServiceAdapter implements
		BatchUpdaterSelfAdapter {
	private static final String endpoints = "@BatchUpdaterSelf";
	private BatchUpdaterManagerPrx managerOneway;

	public BatchUpdaterSelfAdapterImplOld() {
		super(endpoints, 0, Channel.newOceChannel(endpoints));
		managerOneway = locateProxy("M", Channel.Invoke.Oneway,
				BatchUpdaterManagerPrxHelper.class, 300);
	}

	public void incUgcUserScore(int userId, int type, int score) {
		managerOneway.inc(new int[] { userId, type }, score, UgcUserScoreName.value);
	}

	public void decUgcUserScore(int userId, int type, int score) {
		managerOneway.dec(new int[] { userId, type }, score, UgcUserScoreName.value);
	}
	
	public void incUserScore(int userId, int score){
		managerOneway.inc(new int[] { userId}, score, UserScoreName.value);
	}
	
	public void decUserScore(int userId, int score){
		managerOneway.dec(new int[] { userId}, score, UserScoreName.value);
	}

	public void decMessageInCount(int id) {
		managerOneway.decMessageInCount(id, 1);
	}

	public void decMessageOutCount(int id) {
		managerOneway.decMessageOutCount(id, 1);
	}

	public void incMessageInCount(int id) {
		managerOneway.incMessageInCount(id, 1);
	}

	public void incMessageOutCount(int id) {
		managerOneway.incMessageOutCount(id, 1);
	}

	public void decMessageInCount(int id, int value) {
		managerOneway.decMessageInCount(id, value);
	}

	public void decMessageOutCount(int id, int value) {
		managerOneway.decMessageOutCount(id, value);
	}

	public void incMessageInCount(int id, int value) {
		managerOneway.incMessageInCount(id, value);
	}
	
	public void incMessageOutCount(int id, int value) {
		managerOneway.incMessageOutCount(id, value);
	}

	public void decBlogViewCount(int id, int userId, int count) {
		managerOneway.decBlogViewCount(id, userId, count);
	}
	
	public void decPhotoViewCount(int id, int userId, int count) {
		managerOneway.decPhotoViewCount(id, userId, count);
	}

	public void incBlogViewCount(int id, int userId, int count) {
		managerOneway.incBlogViewCount(id, userId, count);
	}

	public void incPhotoViewCount(int id, int userId, int count) {
		managerOneway.incPhotoViewCount(id, userId, count);
	}
	
	public static void main(String[] args) {
		BatchUpdaterSelfAdapterImplOld test = new BatchUpdaterSelfAdapterImplOld();
		
		int idInt=700006077;
	
		long idLong=700006077l;
		
		int userId=700000602;
		
		int count=1;	
	
		test.decBlogViewCount(idInt, userId, count);
		
		test.decBlogViewCount(idLong, userId, count);
		
		System.exit(0);
	}

	public void decBlogViewCount(long id, int userId, int count) {
		managerOneway.decLong(new long[] {userId, id }, count, "BLOGVIEWCOUNTER");
	}

	public void decMessageInCount(long id) {
		managerOneway.decLong(new long[] { id }, 1, "MESSAGEINCOUNTER");
	}

	public void decMessageInCount(long id, int value) {
		managerOneway.decLong(new long[] { id }, value, "MESSAGEINCOUNTER");
	}

	public void decMessageOutCount(long id) {
		managerOneway.decLong(new long[] { id }, 1, "MESSAGEOUTCOUNTER");
	}

	public void decMessageOutCount(long id, int value) {
		managerOneway.decLong(new long[] { id }, value, "MESSAGEOUTCOUNTER");
	}

	public void decPhotoViewCount(long id, int userId, int count) {
		managerOneway.decLong(new long[] {userId, id }, count, "PHOTOVIEWCOUNTER");
	}

	public void incBlogViewCount(long id, int userId, int count) {
		managerOneway.incLong(new long[] { userId ,id}, count, "BLOGVIEWCOUNTER");		
	}

	public void incMessageInCount(long id) {
		managerOneway.incLong(new long[] { id }, 1, "MESSAGEINCOUNTER");
		
	}

	public void incMessageInCount(long id, int value) {
		managerOneway.incLong(new long[] { id }, value, "MESSAGEINCOUNTER");
		
	}

	public void incMessageOutCount(long id) {
		managerOneway.incLong(new long[] { id }, 1, "MESSAGEOUTCOUNTER");
		
	}

	public void incMessageOutCount(long id, int value) {
		managerOneway.incLong(new long[] { id }, value, "MESSAGEOUTCOUNTER");
		
	}

	public void incPhotoViewCount(long id, int userId, int count) {
		managerOneway.incLong(new long[] { userId,id}, count, "PHOTOVIEWCOUNTER");
		
	}	

	public void incPhotoViewCount(long photoId, long albumId, long ownerId, int increment) {
	}
}
