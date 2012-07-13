package com.xiaonei.xce.idcache;

import idcache.IdRelationInfo;

public class IdInfoAdapter {
	
	private IdInfoReaderAdapter _readerAdapter = null;
	private IdInfoWriterAdapter _writerAdapter = null;
	private IdFilter _idFilter = null;

	public IdInfoAdapter() {
		_idFilter = new IdFilter();
		_readerAdapter = new IdInfoReaderAdapter();
		_writerAdapter = new IdInfoWriterAdapter();
	}


	public boolean isMaybeVirtualId(int id){
		return _idFilter.isMaybeVirtualId(id);
	}

	public boolean isVirtualId(int id){
		if(!_idFilter.isMaybeVirtualId(id)){
			return false;
		}
		return _readerAdapter.isVirtualId(id);
	}
	public int getRelatedUserId(int virtualId){
		if(!_idFilter.isMaybeVirtualId(virtualId)){
			return virtualId;
		}
		return _readerAdapter.getRelatedUserId(virtualId);
	}
	
	public int getRelatedTransId(int virtualId){
		if(!_idFilter.isMaybeVirtualId(virtualId)){
			return virtualId;
		}
		return _readerAdapter.getRelatedTransId(virtualId);
	}
	
	public IdRelationInfo getIdRelationInfo(int id){
		return _readerAdapter.getIdRelationInfo(id);
	}
	
	public void createIdRelation(int id, int transId, int virtualId)
	{
		if(!_idFilter.isMaybeVirtualId(virtualId)){
			return;
		}
		_writerAdapter.createIdRelation(id, transId, virtualId);
	}
	
	public void deleteIdRelation(int id, int virtualId)
	{
		if(!_idFilter.isMaybeVirtualId(virtualId)){
			return;
		}
		_writerAdapter.deleteIdRelation(id, virtualId);
	}
	
	public static void main(String[] args){
		IdInfoAdapter adapter = new IdInfoAdapter();
		
		int id = 6813;
		int transId = id +1;
		int virtualId = 1 + 2000000000;
		
		if(adapter.isMaybeVirtualId(0)){
			System.out.println("maybe failed! 0");
		}
		
		if(adapter.isMaybeVirtualId(2000000000)){
			System.out.println("maybe failed! 2000000000");
		}
		
		if(!adapter.isMaybeVirtualId(2000000001)){
			System.out.println("maybe failed! 2000000001");
		}
		
		if(adapter.isMaybeVirtualId(2100000000)){
			System.out.println("maybe failed! 2100000000");
		}
		
		adapter.createIdRelation(id, transId, virtualId);
		
		if(adapter.getRelatedTransId(virtualId) != transId){
			System.out.println("create failed! transId");
		}
		
		if(adapter.getRelatedUserId(virtualId) != id){
			System.out.println("create failed! userId");
		}
		
		if(!adapter.isVirtualId(virtualId)){
			System.out.println("create failed! virtualId");
		}
	
		adapter.deleteIdRelation(id, virtualId);
		
		
		if(adapter.getRelatedTransId(virtualId) != -1){
			System.out.println("delete failed! transId");
		}
		
		if(adapter.getRelatedUserId(virtualId) != -1){
			System.out.println("delete failed! userId");
		}
		
		IdInfoReaderAdapter reader = new IdInfoReaderAdapter();
		IdRelationInfo data = reader.getIdRelationInfo(id);
		
		System.out.println(data.relatedIds.get(6813).length);
		System.out.println(data.relatedIds.get(6814).length);
		
		System.exit(0);
	}

}
