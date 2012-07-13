package xce.distcache.node.impl;

import java.io.IOException;

import java.net.SocketAddress;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.Map.Entry;

import org.apache.log4j.Logger;

import tokyotyrant.RDB;
import xce.distcache.node.ReadNode;
/**
 * @author guanghe.ge@opi-corp.com
 * @since 2010-08-18
 *
 * 如果是一台机器就一个TT对应多个服务，key里名字必不可少，关键时候可能还要加上block
 * 如果一份服务对应一个TT，只要id就可以了，现在是自己定义的prefix＋id，比较统一。
 */
public class TTReadNode extends BaseNode implements ReadNode {
	private Logger l = Logger.getLogger(this.getClass().getName());
	private TTClusterManager _TTCluster = new TTClusterManager();
	private int _TTClusterSize;
	private String _prefix;
	
	public TTReadNode() {
	}
	
	@Override
	public void initialize(String name, String config) {
		super.initialize(name, config);
		_TTClusterSize = _TTCluster.initialize(name, config);
		this._prefix = _TTCluster.getPrefix();
		if (0 == _TTClusterSize) {
			l.warn("TTReadNode has not TT to Load!");
		}
		return;
	}
	
	@Override
	public void add(Item item, byte[] data) {
		try {
			_TTCluster.getRDB(item.id).put(getKey(item.id), data);
		} catch (IOException e) {
			l.error("TTLoadNode.add, key: " + getKey(item.id), e);
			return;
		}
		return;
	}

	public void add(Map<Long, byte[]> bulkDatas) {
		for (Entry<Long, byte[]> entry : bulkDatas.entrySet()) {
			try {
				_TTCluster.getRDB(entry.getKey()).put(getKey(entry.getKey()), entry.getValue());
			} catch (IOException e) {
				l.error("TTLoadNode.add, key: " + getKey(entry.getKey()), e);
				return;
			}
		}
	}

	@Override
	public byte[] get(Item id) {
		try {
			return (byte[]) _TTCluster.getRDB(id.id).get(getKey(id.id));
		} catch (IOException e) {
			l.error("TTLoadNode.get, key", e);
		}
		return null;
	}

	@Override
	/**
	 * 批量读写需要id重新分批，现在默认传入id所属为同一个TT
	 */
	public Map<Item, byte[]> get(List<Item> id) {
		Map<Object, Object> value;
		try {
			value = _TTCluster.getRDB(id.get(0).id).mget(getKeys(id));
		} catch (IOException e) {
			l.error("TTLoadNode.gets " + e);
			return null;
		}
		
		Map<Item, byte[]> result = new HashMap<Item, byte[]>();
		
		for (Entry<Object, Object> el : value.entrySet()) {
			Integer idValue = Integer.getInteger(((String)el.getKey()).split("_")[1]);
			Item item = new Item(idValue, 0);
			result.put(item, (byte[]) el.getValue());
		}
		
		return result;
	}

	public RDB getRDB() {
		return _TTCluster.getRDB();
	}
	
	public RDB getRDB(int index) {
		return _TTCluster.getRDB(index);
	}
	
	public int getTTClusterSize() {
		return _TTClusterSize;
	}
	
	@Override
	public void close() {
		_TTCluster.close();
	}

	private String getKey(Long id) {
//		return _prefix + "_" + id;
		return String.valueOf(id);
	}
	
	@SuppressWarnings("null")
	private String[] getKeys(List<Item> ids) {
		String[] keys = null;
		for(int i = 0; i < ids.size(); i++) {
			keys[i] = getKey(ids.get(i).id);
		}
		return keys;
	}

	@Override
	public Map<SocketAddress, Map<String, String>> getStats() {
		// TODO Auto-generated method stub
		return null;
	}

	@Override
	public boolean internalSet(Item id, byte[] data) {
		try {
			_TTCluster.getRDB(id.id).put(getKey(id.id), data);
		} catch (IOException e) {
			l.error("TTLoadNode.add, key: " + getKey(id.id), e);
			return false;
		}
		return true;
		
	}

	@Override
	public boolean casSet(Item id, byte[] data) {
		// TODO Auto-generated method stub
		return false;
	}

}
