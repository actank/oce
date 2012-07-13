package com.xiaonei.xce.icon2urlinfo;

import java.util.List;
import java.util.Map;

public class Icon2UrlInfoAdapter <T extends Icon2UrlInfo>{
	
	private Icon2UrlInfoReaderAdapter<T> _readerAdapter;
	private Icon2UrlInfoWriterAdapter _writerAdapter;
	
	public Icon2UrlInfoAdapter(Icon2UrlInfoFactory<T> factory) {
		_readerAdapter = new Icon2UrlInfoReaderAdapter<T>(factory);
		_writerAdapter = new Icon2UrlInfoWriterAdapter();
	}
	
	public T getIcon2UrlInfo(int id) {
		return _readerAdapter.getIcon2UrlInfo(id);
	}
	
	public Map<Integer, T> getIcon2UrlInfoMap(List<Integer> ids) {
		return _readerAdapter.getIcon2UrlInfoMap(ids);
	}

	public void createIcon2UrlInfo(int id, Icon2UrlInfo obj) {
		_writerAdapter.createIcon2UrlInfo(id, obj);
	}
	
	public void setIcon2UrlInfo(int id, Icon2UrlInfo obj) {
		_writerAdapter.setIcon2UrlInfo(id, obj);
	}
	
	public static void main(String[] args) {
		Icon2UrlInfoAdapter<DefaultIcon2UrlInfo> adapter = new Icon2UrlInfoAdapter<DefaultIcon2UrlInfo>(new DefaultIcon2UrlInfoFactory());
		int id = 0;
		Icon2UrlInfo oldData = adapter.getIcon2UrlInfo(id);
		oldData.setHideUrl(oldData.getHideUrl());
		
		Icon2UrlInfo data = new Icon2UrlInfo();
		
		data.setIconId(id);
		data.setHideUrl(String.valueOf(id+1));
		adapter.setIcon2UrlInfo(id, data);
		if(!data.getHideUrl().equals(adapter.getIcon2UrlInfo(id).getHideUrl())) {
			System.out.println(adapter.getIcon2UrlInfo(id).getHideUrl());
			System.out.println("set failed!");
		}
		
		adapter.setIcon2UrlInfo(id, oldData);

		System.exit(0);
	}
}
