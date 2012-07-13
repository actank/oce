package com.xiaonei.xce.hatinfo;

import java.util.List;
import java.util.Map;

public class HatInfoAdapter <T extends HatInfo>{
	
	private HatInfoReaderAdapter<T> _readerAdapter;
	private HatInfoWriterAdapter _writerAdapter;
	
	public HatInfoAdapter(HatInfoFactory<T> factory) {
		_readerAdapter = new HatInfoReaderAdapter<T>(factory);
		_writerAdapter = new HatInfoWriterAdapter();
	}
	
	public T getHatInfo(int id) {
		return _readerAdapter.getHatInfo(id);
	}
	
	public Map<Integer, T> getHatInfoMap(List<Integer> ids) {
		return _readerAdapter.getHatInfoMap(ids);
	}

	public void createHatInfo(int id, HatInfo obj) {
		_writerAdapter.createHatInfo(id, obj);
	}
	
	public void setHatInfo(int id, HatInfo obj) {
		_writerAdapter.setHatInfo(id, obj);
	}
	
	public static void main(String[] args) {
		HatInfoAdapter<DefaultHatInfo> adapter = new HatInfoAdapter<DefaultHatInfo>(new DefaultHatInfoFactory());
		int id = 0;
		int status = 0;
		HatInfo oldData = adapter.getHatInfo(id);
		oldData.setHideUrl(oldData.getHideUrl());
		
		HatInfo data = new HatInfo();
		
		data.setHatId(id);
		data.setStatus(status);
		data.setHideUrl(String.valueOf(id+1));
		adapter.setHatInfo(id, data);
		if(!data.getHideUrl().equals(adapter.getHatInfo(id).getHideUrl())) {
			System.out.println(adapter.getHatInfo(id).getHideUrl());
			System.out.println("set failed!");
		}
		
		adapter.setHatInfo(id, oldData);

		System.exit(0);
	}
}
