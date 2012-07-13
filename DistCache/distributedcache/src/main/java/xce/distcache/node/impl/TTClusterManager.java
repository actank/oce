package xce.distcache.node.impl;

import java.io.IOException;
import java.net.InetSocketAddress;
import java.util.ArrayList;
import java.util.List;

import org.apache.log4j.Logger;

import tokyotyrant.RDB;
import tokyotyrant.transcoder.ByteArrayTranscoder;

public class TTClusterManager {
	private Logger l = Logger.getLogger(this.getClass().getName());
	private ByteArrayTranscoder byteArrayTranscoder = new ByteArrayTranscoder();
	private List<RDB> _TTList = new ArrayList<RDB>();
	private int _TTClusterSize;
	private int _TTCurrentPosition;

	private String _prefix;

	/**
	 * 把一个TT集群信息进行解析，默认在一台机器上，ip：port0:port1...:prefix
	 * 
	 * @param name
	 * @param config
	 */
	public int initialize(String name, String config) {
		_TTClusterSize = 0;
		String[] split = null;
		try {
			split = config.split(":");
		} catch (Exception e) {
			l.error("TTLoadNode.initialize, ", e);
			return 0;
		}

		int lastIndex = split.length - 1;
		for (int i = 1; i < lastIndex; i++) {
			try {
				RDB db = new RDB();
				db.open(new InetSocketAddress(split[0], Integer
						.parseInt(split[i])));
				db.setValueTranscoder(byteArrayTranscoder);
				_TTList.add(db);
				_TTClusterSize ++;
			} catch (NumberFormatException e) {
				l.error("TTLoadNode.initialize, ", e);
				return 0;
			} catch (IOException e) {
				l.error("TTLoadNode.initialize, ", e);
				return 0;
			}
		}

		this._prefix = split[lastIndex];

		return _TTClusterSize;
	}
	
	public synchronized RDB getRDB() {
		System.out.println("TTClusterManager.getRDB, _TTCurrentPosition: " + _TTCurrentPosition 
				+ " TTList.size: " + _TTList.size());
		_TTCurrentPosition %= _TTClusterSize;
		RDB db = _TTList.get(_TTCurrentPosition);
		_TTCurrentPosition ++;
		return db;
	}
	
	public synchronized RDB getRDB(int index) {
		return _TTList.get(index);
	}
	
	public synchronized RDB getRDB(Long id) {
		return _TTList.get((int) (id/10 % _TTClusterSize));
	}
	
	public String getPrefix() {
		return this._prefix;
	}
	
	public int getClusterSize() {
		return this._TTClusterSize;
	}
	
	public void close() {
		for (RDB db : _TTList) {
			db.close();
		}
	}
}