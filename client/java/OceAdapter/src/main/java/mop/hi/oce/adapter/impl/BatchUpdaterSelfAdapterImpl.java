package mop.hi.oce.adapter.impl;

import com.xiaonei.xce.log.Oce;
import mop.hi.oce.adapter.BatchUpdaterSelfAdapter;
import mop.hi.svc.batchtraits.UgcUserScoreName;
import mop.hi.svc.batchtraits.UserScoreName;
import mop.hi.svc.model.BatchUpdaterManagerPrx;
import mop.hi.svc.model.BatchUpdaterManagerPrxHelper;
import xce.clusterstate.ReplicatedClusterAdapter;

public class BatchUpdaterSelfAdapterImpl extends ReplicatedClusterAdapter implements
		BatchUpdaterSelfAdapter {
	
	private static final String ctr_endpoints = "ControllerBatchUpdaterSelf";
	private static int _interval = 120;	

	public BatchUpdaterSelfAdapterImpl() {
		super(ctr_endpoints,_interval);
	}	
	
	private BatchUpdaterManagerPrx getBatchUpdaterManager() {
		
		Ice.ObjectPrx prx0 = getValidProxy(0, 300);
		if ( prx0 != null ) {
			BatchUpdaterManagerPrx prx = BatchUpdaterManagerPrxHelper.uncheckedCast(prx0.ice_timeout(300).ice_oneway());
			return prx;
		} else {
			return null;
		}
	}

	public void incUgcUserScore(int userId, int type, int score) {
		BatchUpdaterManagerPrx prx=null;
		try {
			prx=getBatchUpdaterManager();
			prx.inc(new int[] { userId, type }, score, UgcUserScoreName.value);
		} catch (Ice.ConnectTimeoutException e) {
			Oce.getLogger().error(this.getClass().getName() + ".incUgcUserScore  [ " + prx + " ]  Ice.ConnectTimeoutException");
		} catch (Ice.TimeoutException e) {
			Oce.getLogger().error(this.getClass().getName() + ".incUgcUserScore  [ " + prx + " ]  Ice.TimeoutException");			
		} catch (Exception e) {
			Oce.getLogger().error(this.getClass().getName() + ".incUgcUserScore", e);
		}
	}

	public void decUgcUserScore(int userId, int type, int score) {
		BatchUpdaterManagerPrx prx=null;
		try {
			prx=getBatchUpdaterManager();
			prx.dec(new int[] { userId, type }, score, UgcUserScoreName.value);
		} catch (Ice.ConnectTimeoutException e) {
			Oce.getLogger().error(this.getClass().getName() + ".decUgcUserScore  [ " + prx + " ]  Ice.ConnectTimeoutException");
		} catch (Ice.TimeoutException e) {
			Oce.getLogger().error(this.getClass().getName() + ".decUgcUserScore  [ " + prx + " ]  Ice.TimeoutException");			
		} catch (Exception e) {
			Oce.getLogger().error(this.getClass().getName() + ".decUgcUserScore", e);
		}
	}
	
	public void incUserScore(int userId, int score){
		BatchUpdaterManagerPrx prx=null;
		try {
			prx=getBatchUpdaterManager();
			prx.inc(new int[] { userId}, score, UserScoreName.value);
		} catch (Ice.ConnectTimeoutException e) {
			Oce.getLogger().error(this.getClass().getName() + ".incUserScore  [ " + prx + " ]  Ice.ConnectTimeoutException");
		} catch (Ice.TimeoutException e) {
			Oce.getLogger().error(this.getClass().getName() + ".incUserScore  [ " + prx + " ]  Ice.TimeoutException");			
		} catch (Exception e) {
			Oce.getLogger().error(this.getClass().getName() + ".incUserScore", e);
		}
	}
	
	public void decUserScore(int userId, int score){
		BatchUpdaterManagerPrx prx=null;
		try {
			prx=getBatchUpdaterManager();
			prx.dec(new int[] { userId}, score, UserScoreName.value);
		} catch (Ice.ConnectTimeoutException e) {
			Oce.getLogger().error(this.getClass().getName() + ".decUserScore  [ " + prx + " ]  Ice.ConnectTimeoutException");
		} catch (Ice.TimeoutException e) {
			Oce.getLogger().error(this.getClass().getName() + ".decUserScore  [ " + prx + " ]  Ice.TimeoutException");			
		} catch (Exception e) {
			Oce.getLogger().error(this.getClass().getName() + ".decUserScore", e);
		}
	}

	public void decMessageInCount(int id) {
		BatchUpdaterManagerPrx prx=null;
		try {
			prx=getBatchUpdaterManager();
			prx.decMessageInCount(id, 1);
		} catch (Ice.ConnectTimeoutException e) {
			Oce.getLogger().error(this.getClass().getName() + ".decMessageInCount  [ " + prx + " ]  Ice.ConnectTimeoutException");
		} catch (Ice.TimeoutException e) {
			Oce.getLogger().error(this.getClass().getName() + ".decMessageInCount  [ " + prx + " ]  Ice.TimeoutException");			
		} catch (Exception e) {
			Oce.getLogger().error(this.getClass().getName() + ".decMessageInCount", e);
		}
	}

	public void decMessageOutCount(int id) {
		BatchUpdaterManagerPrx prx=null;
		try {
			prx=getBatchUpdaterManager();
			prx.decMessageOutCount(id, 1);
		} catch (Ice.ConnectTimeoutException e) {
			Oce.getLogger().error(this.getClass().getName() + ".decMessageOutCount  [ " + prx + " ]  Ice.ConnectTimeoutException");
		} catch (Ice.TimeoutException e) {
			Oce.getLogger().error(this.getClass().getName() + ".decMessageOutCount  [ " + prx + " ]  Ice.TimeoutException");			
		} catch (Exception e) {
			Oce.getLogger().error(this.getClass().getName() + ".decMessageOutCount", e);
		}
	}

	public void incMessageInCount(int id) {
		BatchUpdaterManagerPrx prx=null;
		try {
			prx=getBatchUpdaterManager();
			prx.incMessageInCount(id, 1);
		} catch (Ice.ConnectTimeoutException e) {
			Oce.getLogger().error(this.getClass().getName() + ".incMessageInCount  [ " + prx + " ]  Ice.ConnectTimeoutException");
		} catch (Ice.TimeoutException e) {
			Oce.getLogger().error(this.getClass().getName() + ".incMessageInCount  [ " + prx + " ]  Ice.TimeoutException");			
		} catch (Exception e) {
			Oce.getLogger().error(this.getClass().getName() + ".incMessageInCount", e);
		}
	}

	public void incMessageOutCount(int id) {
		BatchUpdaterManagerPrx prx=null;
		try {
			prx=getBatchUpdaterManager();
			prx.incMessageOutCount(id, 1);
		} catch (Ice.ConnectTimeoutException e) {
			Oce.getLogger().error(this.getClass().getName() + ".incMessageOutCount  [ " + prx + " ]  Ice.ConnectTimeoutException");
		} catch (Ice.TimeoutException e) {
			Oce.getLogger().error(this.getClass().getName() + ".incMessageOutCount  [ " + prx + " ]  Ice.TimeoutException");			
		} catch (Exception e) {
			Oce.getLogger().error(this.getClass().getName() + ".incMessageOutCount", e);
		}
	}

	public void decMessageInCount(int id, int value) {
		BatchUpdaterManagerPrx prx=null;
		try {
			prx=getBatchUpdaterManager();
			prx.decMessageInCount(id, value);
		} catch (Ice.ConnectTimeoutException e) {
			Oce.getLogger().error(this.getClass().getName() + ".decMessageInCount  [ " + prx + " ]  Ice.ConnectTimeoutException");
		} catch (Ice.TimeoutException e) {
			Oce.getLogger().error(this.getClass().getName() + ".decMessageInCount  [ " + prx + " ]  Ice.TimeoutException");			
		} catch (Exception e) {
			Oce.getLogger().error(this.getClass().getName() + ".decMessageInCount", e);
		}
	}

	public void decMessageOutCount(int id, int value) {
		BatchUpdaterManagerPrx prx=null;
		try {
			prx=getBatchUpdaterManager();
			prx.decMessageOutCount(id, value);
		} catch (Ice.ConnectTimeoutException e) {
			Oce.getLogger().error(this.getClass().getName() + ".decMessageOutCount  [ " + prx + " ]  Ice.ConnectTimeoutException");
		} catch (Ice.TimeoutException e) {
			Oce.getLogger().error(this.getClass().getName() + ".decMessageOutCount  [ " + prx + " ]  Ice.TimeoutException");			
		} catch (Exception e) {
			Oce.getLogger().error(this.getClass().getName() + ".decMessageOutCount", e);
		}
	}

	public void incMessageInCount(int id, int value) {
		BatchUpdaterManagerPrx prx=null;
		try {
			prx=getBatchUpdaterManager();
			prx.incMessageInCount(id, value);
		} catch (Ice.ConnectTimeoutException e) {
			Oce.getLogger().error(this.getClass().getName() + ".incMessageInCount  [ " + prx + " ]  Ice.ConnectTimeoutException");
		} catch (Ice.TimeoutException e) {
			Oce.getLogger().error(this.getClass().getName() + ".incMessageInCount  [ " + prx + " ]  Ice.TimeoutException");			
		} catch (Exception e) {
			Oce.getLogger().error(this.getClass().getName() + ".incMessageInCount", e);
		}
	}

	public void incMessageOutCount(int id, int value) {
		BatchUpdaterManagerPrx prx=null;
		try {
			prx=getBatchUpdaterManager();
			prx.incMessageOutCount(id, value);
		} catch (Ice.ConnectTimeoutException e) {
			Oce.getLogger().error(this.getClass().getName() + ".incMessageOutCount  [ " + prx + " ]  Ice.ConnectTimeoutException");
		} catch (Ice.TimeoutException e) {
			Oce.getLogger().error(this.getClass().getName() + ".incMessageOutCount  [ " + prx + " ]  Ice.TimeoutException");			
		} catch (Exception e) {
			Oce.getLogger().error(this.getClass().getName() + ".incMessageOutCount", e);
		}
	}

	public void decBlogViewCount(int id, int userId, int count) {
		BatchUpdaterManagerPrx prx=null;
		try {
			prx=getBatchUpdaterManager();
			prx.decBlogViewCount(id, userId, count);
		} catch (Ice.ConnectTimeoutException e) {
			Oce.getLogger().error(this.getClass().getName() + ".decBlogViewCount  [ " + prx + " ]  Ice.ConnectTimeoutException");
		} catch (Ice.TimeoutException e) {
			Oce.getLogger().error(this.getClass().getName() + ".decBlogViewCount  [ " + prx + " ]  Ice.TimeoutException");			
		} catch (Exception e) {
			Oce.getLogger().error(this.getClass().getName() + ".decBlogViewCount", e);
		}
	}

	@Deprecated
	public void decPhotoViewCount(int id, int userId, int count) {
		BatchUpdaterManagerPrx prx=null;
		try {
			prx=getBatchUpdaterManager();
			prx.decPhotoViewCount(id, userId, count);
		} catch (Ice.ConnectTimeoutException e) {
			Oce.getLogger().error(this.getClass().getName() + ".decPhotoViewCount  [ " + prx + " ]  Ice.ConnectTimeoutException");
		} catch (Ice.TimeoutException e) {
			Oce.getLogger().error(this.getClass().getName() + ".decPhotoViewCount  [ " + prx + " ]  Ice.TimeoutException");			
		} catch (Exception e) {
			Oce.getLogger().error(this.getClass().getName() + ".decPhotoViewCount", e);
		}
	}

	public void incBlogViewCount(int id, int userId, int count) {
		BatchUpdaterManagerPrx prx=null;
		try {
			prx=getBatchUpdaterManager();
			prx.incBlogViewCount(id, userId, count);
		} catch (Ice.ConnectTimeoutException e) {
			Oce.getLogger().error(this.getClass().getName() + ".incBlogViewCount  [ " + prx + " ]  Ice.ConnectTimeoutException");
		} catch (Ice.TimeoutException e) {
			Oce.getLogger().error(this.getClass().getName() + ".incBlogViewCount  [ " + prx + " ]  Ice.TimeoutException");			
		} catch (Exception e) {
			Oce.getLogger().error(this.getClass().getName() + ".incBlogViewCount", e);
		}
	}
	
	@Deprecated
	public void incPhotoViewCount(int id, int userId, int count) {
		BatchUpdaterManagerPrx prx=null;
		try {
			prx=getBatchUpdaterManager();
			prx.incPhotoViewCount(id, userId, count);
		} catch (Ice.ConnectTimeoutException e) {
			Oce.getLogger().error(this.getClass().getName() + ".incPhotoViewCount  [ " + prx + " ]  Ice.ConnectTimeoutException");
		} catch (Ice.TimeoutException e) {
			Oce.getLogger().error(this.getClass().getName() + ".incPhotoViewCount  [ " + prx + " ]  Ice.TimeoutException");			
		} catch (Exception e) {
			Oce.getLogger().error(this.getClass().getName() + ".incPhotoViewCount", e);
		}
	}
	
	public void decBlogViewCount(long id, int userId, int count) {
		BatchUpdaterManagerPrx prx=null;
		try {
			prx=getBatchUpdaterManager();
			prx.decLong(new long[] {userId, id }, count, "BLOGVIEWCOUNTER");
		} catch (Ice.ConnectTimeoutException e) {
			Oce.getLogger().error(this.getClass().getName() + ".decBlogViewCount  [ " + prx + " ]  Ice.ConnectTimeoutException");
		} catch (Ice.TimeoutException e) {
			Oce.getLogger().error(this.getClass().getName() + ".decBlogViewCount  [ " + prx + " ]  Ice.TimeoutException");			
		} catch (Exception e) {
			Oce.getLogger().error(this.getClass().getName() + ".decBlogViewCount", e);
		}
	}

	public void decMessageInCount(long id) {
		BatchUpdaterManagerPrx prx=null;
		try {
			prx=getBatchUpdaterManager();
			prx.decLong(new long[] { id }, 1, "MESSAGEINCOUNTER");
		} catch (Ice.ConnectTimeoutException e) {
			Oce.getLogger().error(this.getClass().getName() + ".decMessageInCount  [ " + prx + " ]  Ice.ConnectTimeoutException");
		} catch (Ice.TimeoutException e) {
			Oce.getLogger().error(this.getClass().getName() + ".decMessageInCount  [ " + prx + " ]  Ice.TimeoutException");			
		} catch (Exception e) {
			Oce.getLogger().error(this.getClass().getName() + ".decMessageInCount", e);
		}
	}

	public void decMessageInCount(long id, int value) {
		BatchUpdaterManagerPrx prx=null;
		try {
			prx=getBatchUpdaterManager();
			prx.decLong(new long[] { id }, value, "MESSAGEINCOUNTER");
		} catch (Ice.ConnectTimeoutException e) {
			Oce.getLogger().error(this.getClass().getName() + ".decMessageInCount  [ " + prx + " ]  Ice.ConnectTimeoutException");
		} catch (Ice.TimeoutException e) {
			Oce.getLogger().error(this.getClass().getName() + ".decMessageInCount  [ " + prx + " ]  Ice.TimeoutException");			
		} catch (Exception e) {
			Oce.getLogger().error(this.getClass().getName() + ".decMessageInCount", e);
		}
	}

	public void decMessageOutCount(long id) {
		BatchUpdaterManagerPrx prx=null;
		try {
			prx=getBatchUpdaterManager();
			prx.decLong(new long[] { id }, 1, "MESSAGEOUTCOUNTER");
		} catch (Ice.ConnectTimeoutException e) {
			Oce.getLogger().error(this.getClass().getName() + ".decMessageOutCount  [ " + prx + " ]  Ice.ConnectTimeoutException");
		} catch (Ice.TimeoutException e) {
			Oce.getLogger().error(this.getClass().getName() + ".decMessageOutCount  [ " + prx + " ]  Ice.TimeoutException");			
		} catch (Exception e) {
			Oce.getLogger().error(this.getClass().getName() + ".decMessageOutCount", e);
		}
	}

	public void decMessageOutCount(long id, int value) {
		BatchUpdaterManagerPrx prx=null;
		try {
			prx=getBatchUpdaterManager();
			prx.decLong(new long[] { id }, value, "MESSAGEOUTCOUNTER");
		} catch (Ice.ConnectTimeoutException e) {
			Oce.getLogger().error(this.getClass().getName() + ".decMessageOutCount  [ " + prx + " ]  Ice.ConnectTimeoutException");
		} catch (Ice.TimeoutException e) {
			Oce.getLogger().error(this.getClass().getName() + ".decMessageOutCount  [ " + prx + " ]  Ice.TimeoutException");			
		} catch (Exception e) {
			Oce.getLogger().error(this.getClass().getName() + ".decMessageOutCount", e);
		}
	}

	@Deprecated
	public void decPhotoViewCount(long id, int userId, int count) {
		BatchUpdaterManagerPrx prx=null;
		try {
			prx=getBatchUpdaterManager();
			prx.decLong(new long[] {userId, id }, count, "PHOTOVIEWCOUNTER");
		} catch (Ice.ConnectTimeoutException e) {
			Oce.getLogger().error(this.getClass().getName() + ".decPhotoViewCount  [ " + prx + " ]  Ice.ConnectTimeoutException");
		} catch (Ice.TimeoutException e) {
			Oce.getLogger().error(this.getClass().getName() + ".decPhotoViewCount  [ " + prx + " ]  Ice.TimeoutException");			
		} catch (Exception e) {
			Oce.getLogger().error(this.getClass().getName() + ".decPhotoViewCount", e);
		}
	}

	public void incBlogViewCount(long id, int userId, int count) {
		BatchUpdaterManagerPrx prx=null;
		try {
			prx=getBatchUpdaterManager();
			prx.incLong(new long[] { userId ,id}, count, "BLOGVIEWCOUNTER");
		} catch (Ice.ConnectTimeoutException e) {
			Oce.getLogger().error(this.getClass().getName() + ".incBlogViewCount  [ " + prx + " ]  Ice.ConnectTimeoutException");
		} catch (Ice.TimeoutException e) {
			Oce.getLogger().error(this.getClass().getName() + ".incBlogViewCount  [ " + prx + " ]  Ice.TimeoutException");			
		} catch (Exception e) {
			Oce.getLogger().error(this.getClass().getName() + ".incBlogViewCount", e);
		}
	}

	public void incMessageInCount(long id) {
		BatchUpdaterManagerPrx prx=null;
		try {
			prx=getBatchUpdaterManager();
			prx.incLong(new long[] { id }, 1, "MESSAGEINCOUNTER");
		} catch (Ice.ConnectTimeoutException e) {
			Oce.getLogger().error(this.getClass().getName() + ".incMessageInCount  [ " + prx + " ]  Ice.ConnectTimeoutException");
		} catch (Ice.TimeoutException e) {
			Oce.getLogger().error(this.getClass().getName() + ".incMessageInCount  [ " + prx + " ]  Ice.TimeoutException");			
		} catch (Exception e) {
			Oce.getLogger().error(this.getClass().getName() + ".incMessageInCount", e);
		}		
	}

	public void incMessageInCount(long id, int value) {
		BatchUpdaterManagerPrx prx=null;
		try {
			prx=getBatchUpdaterManager();
			prx.incLong(new long[] { id }, value, "MESSAGEINCOUNTER");
		} catch (Ice.ConnectTimeoutException e) {
			Oce.getLogger().error(this.getClass().getName() + ".incMessageInCount  [ " + prx + " ]  Ice.ConnectTimeoutException");
		} catch (Ice.TimeoutException e) {
			Oce.getLogger().error(this.getClass().getName() + ".incMessageInCount  [ " + prx + " ]  Ice.TimeoutException");			
		} catch (Exception e) {
			Oce.getLogger().error(this.getClass().getName() + ".incMessageInCount", e);
		}		
	}

	public void incMessageOutCount(long id) {
		BatchUpdaterManagerPrx prx=null;
		try {
			prx=getBatchUpdaterManager();
			prx.incLong(new long[] { id }, 1, "MESSAGEOUTCOUNTER");
		} catch (Ice.ConnectTimeoutException e) {
			Oce.getLogger().error(this.getClass().getName() + ".incMessageOutCount  [ " + prx + " ]  Ice.ConnectTimeoutException");
		} catch (Ice.TimeoutException e) {
			Oce.getLogger().error(this.getClass().getName() + ".incMessageOutCount  [ " + prx + " ]  Ice.TimeoutException");			
		} catch (Exception e) {
			Oce.getLogger().error(this.getClass().getName() + ".incMessageOutCount", e);
		}		
	}

	public void incMessageOutCount(long id, int value) {
		BatchUpdaterManagerPrx prx=null;
		try {
			prx=getBatchUpdaterManager();
			prx.incLong(new long[] { id }, value, "MESSAGEOUTCOUNTER");
		} catch (Ice.ConnectTimeoutException e) {
			Oce.getLogger().error(this.getClass().getName() + ".incMessageOutCount  [ " + prx + " ]  Ice.ConnectTimeoutException");
		} catch (Ice.TimeoutException e) {
			Oce.getLogger().error(this.getClass().getName() + ".incMessageOutCount  [ " + prx + " ]  Ice.TimeoutException");			
		} catch (Exception e) {
			Oce.getLogger().error(this.getClass().getName() + ".incMessageOutCount", e);
		}		
	}

	@Deprecated
	public void incPhotoViewCount(long id, int userId, int count) {
		BatchUpdaterManagerPrx prx=null;
		try {
			prx=getBatchUpdaterManager();
			prx.incLong(new long[] { userId,id}, count, "PHOTOVIEWCOUNTER");
		} catch (Ice.ConnectTimeoutException e) {
			Oce.getLogger().error(this.getClass().getName() + ".incPhotoViewCount  [ " + prx + " ]  Ice.ConnectTimeoutException");
		} catch (Ice.TimeoutException e) {
			Oce.getLogger().error(this.getClass().getName() + ".incPhotoViewCount  [ " + prx + " ]  Ice.TimeoutException");			
		} catch (Exception e) {
			Oce.getLogger().error(this.getClass().getName() + ".incPhotoViewCount", e);
		}		
	}
	
	/**
	 * 照片浏览次数批量更新数据库的接口
	 * @param photoId 照片ID
	 * @param albumId 相册ID
	 * @param ownerId 用户ID
	 * @param increment 增量，一般为正整数，负数时减少浏览量，特殊情况下才需要用到。
	 */
	public void incPhotoViewCount(long photoId, long albumId, long ownerId, int increment) {
		BatchUpdaterManagerPrx prx=null;
		try {
			prx=getBatchUpdaterManager();
			if(increment >= 0){
				prx.incLong(new long[] {ownerId, photoId, albumId}, increment, "PHOTOVIEWNCOUNTER");
			}else{
				prx.decLong(new long[] {ownerId, photoId, albumId}, -1 * increment, "PHOTOVIEWNCOUNTER");
			}
		} catch (Ice.ConnectTimeoutException e) {
			Oce.getLogger().error(this.getClass().getName() + ".incPhotoViewNCount  [ " + prx + " ]  Ice.ConnectTimeoutException");
		} catch (Ice.TimeoutException e) {
			Oce.getLogger().error(this.getClass().getName() + ".incPhotoViewNCount  [ " + prx + " ]  Ice.TimeoutException");			
		} catch (Exception e) {
			Oce.getLogger().error(this.getClass().getName() + ".incPhotoViewNCount", e);
		}		
	}

//	// 仅供内部使用，方便快速调用
//	public static void main(String[] args) {
//		long photoId = 5093218419L;
//		long albumId = 454779769L;
//		long ownerId = 35178L;
//		int count = 5;
//		int option = 1;
//
//		if (args.length == 5) {
//			option = Integer.parseInt(args[0]);
//			photoId = Long.parseLong(args[1]);
//			albumId = Long.parseLong(args[2]);
//			ownerId = Long.parseLong(args[3]);
//			count = Integer.parseInt(args[4]);
//		}
//
//		System.out.println("args order:option,photoId,albumId,ownerId,count");
//		System.out.println("args value:" + option + "," + photoId + "," + albumId + "," + ownerId + "," + count);
//		System.out.println("option=1 : incPhotoViewCount(photoId, albumId, ownerId, count)");
//		System.out.println("option=2 : incPhotoViewCount(photoId, (int)ownerId, count)");
//		System.out.println("option=3 : decPhotoViewCount(photoId, (int)ownerId, count)");
//
//		try {
//			BatchUpdaterSelfAdapterImpl adapter = new BatchUpdaterSelfAdapterImpl();
//			switch (option) {
//			case 1:
//				adapter.incPhotoViewCount(photoId, albumId, ownerId, count);
//				break;
//			case 2:
//				adapter.incPhotoViewCount(photoId, (int) ownerId, count);
//				break;
//			case 3:
//				adapter.decPhotoViewCount(photoId, (int) ownerId, count);
//				break;
//			default:
//				break;
//			}
//
//			System.out.println("Ok");
//		} catch (Exception e) {
//			e.printStackTrace();
//		}
//		System.exit(0);
//	}
//
}
