package com.xiaonei.xce.vipmemberinfo;

import java.util.ArrayList;
import java.util.List;
import java.util.Map;

public class VipMemberInfoAdapter <T extends VipMemberInfo>{
	
	private VipMemberInfoReaderAdapter<T> _readerAdapter;
	private VipMemberInfoWriterAdapter _writerAdapter;
	
	public VipMemberInfoAdapter(VipMemberInfoFactory<T> factory) {
		_readerAdapter = new VipMemberInfoReaderAdapter<T>(factory);
		_writerAdapter = new VipMemberInfoWriterAdapter();
	}
	
	public T getOneTypeVipMemberInfo(int id, int type) {
		return _readerAdapter.getOneTypeVipMemberInfo(id, type);
	}
	
	public Map<Integer, T> getOneTypeVipMemberInfoSeq(List<Integer> ids, int type) {
		return _readerAdapter.getOneTypeVipMemberInfoSeqMap(ids, type);
	}
	
	public Map<Integer, List<T> > getVipMemberInfoSeqMap(List<Integer> ids, List<Integer> types) {
		return _readerAdapter.getVipMemberInfoSeqMap(ids, types);
	}

	public List<T> getVipMemberInfoSeq(int id, List<Integer> types) {
		return _readerAdapter.getVipMemberInfoSeq(id, types);
	}
	
	public void createVipMemberInfo(int id, VipMemberInfo obj) throws Exception {
		_writerAdapter.createVipMemberInfo(id, obj);
	}
	
	public void setVipMemberInfo(int id, VipMemberInfo obj) throws Exception {
		_writerAdapter.setVipMemberInfo(id, obj);
	}
	
	public static void main(String[] args) {
		VipMemberInfoAdapter<DefaultVipMemberInfo> adapter = new VipMemberInfoAdapter<DefaultVipMemberInfo>(new DefaultVipMemberInfoFactory());
		int id = 386246921;
		List<Integer> types = new ArrayList<Integer>();
//		VipMemberInfo data = new VipMemberInfo();
//		data.setId(id);
//		data.setCharInfo1(String.valueOf(id));
//		data.setCharInfo2(String.valueOf(id));
//		data.setIconId((id)%10);
//		data.setIntInfo1(id);
//		data.setIntInfo2(id);
//		data.setLevel(id);
//		data.setStatus((id)%10);
//		data.setType(0);
//		adapter.createVipMemberInfo(id, data);
//		data.setId(id);
//		data.setCharInfo1(String.valueOf(id));
//		data.setCharInfo2(String.valueOf(id));
//		data.setIconId((id)%10);
//		data.setIntInfo1(id);
//		data.setIntInfo2(id);
//		data.setLevel(id);
//		data.setStatus((id)%10);
//		data.setType(1);
//		adapter.createVipMemberInfo(id, data);
//		data.setId(id);
//		data.setCharInfo1(String.valueOf(id));
//		data.setCharInfo2(String.valueOf(id));
//		data.setIconId((id)%10);
//		data.setIntInfo1(id);
//		data.setIntInfo2(id);
//		data.setLevel(id);
//		data.setStatus((id)%10);
//		data.setType(2);
//		adapter.createVipMemberInfo(id, data);
//		data.setId(id);
//		data.setCharInfo1(String.valueOf(id));
//		data.setCharInfo2(String.valueOf(id));
//		data.setIconId((id)%10);
//		data.setIntInfo1(id);
//		data.setIntInfo2(id);
//		data.setLevel(id);
//		data.setStatus((id)%10);
//		data.setType(3);
//		adapter.createVipMemberInfo(id, data);
//		
//		types.add(0);
//		types.add(1);
//		types.add(2);
//		types.add(3);
//		List<DefaultVipMemberInfo> dataSeq = adapter.getVipMemberInfoSeq(id, types);
//		for(DefaultVipMemberInfo nData : dataSeq) {
//			if(!nData.getCharInfo1().equals(data.getCharInfo1()) |
//					!nData.getCharInfo2().equals(data.getCharInfo2() ) |
//					nData.getIconId() != data.getIconId() |
//					nData.getIntInfo1() != data.getIntInfo1() |
//					nData.getIntInfo2() != data.getIntInfo2() |
//					nData.getLevel() != data.getLevel() |
//					nData.getStatus() != data.getStatus() |
//					nData.getType() != data.getType() )
//			{
//				System.out.println("Create failed!");
//			}
//		}
//		
		
		types.clear();
		types.add(0);
		List<DefaultVipMemberInfo> oldDataSeq = adapter.getVipMemberInfoSeq(id, types);
		
		DefaultVipMemberInfo oldGetData = oldDataSeq.get(0);
		
		oldGetData.setCharInfo1(oldGetData.getCharInfo1());
		oldGetData.setCharInfo2(oldGetData.getCharInfo2());
		oldGetData.setIntInfo1(oldGetData.getIntInfo1());
		oldGetData.setIntInfo1(oldGetData.getIntInfo1());
		oldGetData.setType(oldGetData.getType());
		oldGetData.setStatus(oldGetData.getStatus());
		oldGetData.setLevel(oldGetData.getLevel());
		oldGetData.setIconId(oldGetData.getIconId());
		
		VipMemberInfo newData = new VipMemberInfo();
		newData.setId(id);
		newData.setCharInfo1(String.valueOf(id+1));
		newData.setCharInfo2(String.valueOf(id+2));
		newData.setIconId((id+3)%10);
		newData.setIntInfo1(id+4);
		newData.setIntInfo2(id+5);
		newData.setLevel(id+6);
		newData.setStatus((id+7)%10);
		newData.setType(0);
		try {
			adapter.setVipMemberInfo(id, newData);
		} catch (Exception e1) {
			// TODO Auto-generated catch block
			e1.printStackTrace();
		}
		
		List<DefaultVipMemberInfo> newDataSeq = adapter.getVipMemberInfoSeq(id, types);
		
		DefaultVipMemberInfo newGetData = newDataSeq.get(0);
		
		if(!newGetData.getCharInfo1().equals(newData.getCharInfo1()) |
				!newGetData.getCharInfo2().equals(newData.getCharInfo2() ) |
				newGetData.getIconId() != newData.getIconId() |
				newGetData.getIntInfo1() != newData.getIntInfo1() |
				newGetData.getIntInfo2() != newData.getIntInfo2() |
				newGetData.getLevel() != newData.getLevel() |
				newGetData.getStatus() != newData.getStatus() |
				newGetData.getType() != newData.getType() )
		{
			System.out.println("Set failed!");
		}
		
		try {
			adapter.setVipMemberInfo(id, oldGetData);
		} catch (Exception e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}

		System.exit(0);
	}

	
}
