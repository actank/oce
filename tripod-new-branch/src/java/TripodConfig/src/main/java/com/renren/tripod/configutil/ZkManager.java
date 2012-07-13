package com.renren.tripod.configutil;

import java.io.IOException;

import org.apache.zookeeper.WatchedEvent;
import org.apache.zookeeper.Watcher;
import org.apache.zookeeper.ZooKeeper;
import org.apache.zookeeper.Watcher.Event.EventType;

public class ZkManager {
	private static org.apache.commons.logging.Log _logger = org.apache.commons.logging.LogFactory
			.getLog(ZkManager.class);

	private ZooKeeper _zk;
	private String _zkAddress;
	private ZkWatcher _watcher = new ZkWatcher();
	private ReNotifyThread _reNotifyThread = new ReNotifyThread();

	public boolean initialize(String zkAddress) throws IOException {
		_logger.info("ZkManager.initialize zkAddress " + zkAddress);
		_zkAddress = zkAddress;
		try {
			synchronized (_watcher) {
				_zk = new ZooKeeper(_zkAddress, 2000, _watcher);
				_watcher.wait();
			}
		} catch (IllegalArgumentException e) {
			_logger.error("ZkManager.initialize " + e);
			throw e;
		} catch (IOException e) {
			_logger.error("ZkManager.initialize " + e);
			throw e;
		} catch (Throwable e) {
			e.printStackTrace();
			return false;
		}
		Thread thr = new Thread(_reNotifyThread);
		thr.start();
		return true;
	}

	public ZooKeeper getZk() {
		_logger.info("ZkManager.getZk zkAddress " + _zkAddress);
		synchronized (_watcher) {
			return _zk;
		}
	}

	private boolean notifyHandle() {
		_logger.info("ZkManager.notifyHandle zkAddress " + _zkAddress);
		try {
			if (ConfigManagerFactory.get(_zkAddress).updateConfig(getZk())) {
				_reNotifyThread.setRunningState(false);
				return true;
			}
		} catch (Throwable e) {
			_logger.error("ZkManager.notifyHandle Exception" + e
					+ " zkAddress " + _zkAddress);
		}
		_reNotifyThread.setRunningState(true);
		return false;
	}

	private class ZkWatcher implements Watcher {

		private boolean isAlive = true;

		public void process(WatchedEvent event) {
			_logger.info("ZkManager.ZkWatcher.process get notify : path = "
					+ event.getPath() + " type = " + event.getType()
					+ " state = " + event.getState());
			if (!isAlive) {
				_logger.error("ZkManager.ZkWatcher.process get notify : path = "
						+ event.getPath()
						+ " type = "
						+ event.getType()
						+ " state = "
						+ event.getState()
						+ " but watcher is already not alive!");
				return;
			}

			if (event.getType() == EventType.None) {
				switch (event.getState()) {
				case SyncConnected:
					synchronized (this) {
						this.notifyAll();
					}
					_reNotifyThread.setRunningState(true);
					notifyHandle();
					break;
				case Disconnected:
					_reNotifyThread.setRunningState(false);
					return;
				case Expired:
					while (true) {
						try {
							_reNotifyThread.setRunningState(false);
							try {
								_zk.close();
							} catch (InterruptedException e) {
								_logger.error("ZkManager.ZkWatcher.process close "
										+ e);
							}
							isAlive = false;
							_watcher = new ZkWatcher();
							_zk = new ZooKeeper(_zkAddress, 2000, _watcher);
							return;
						} catch (Throwable e) {
							_logger.error("ZkManager.ZkWatcher.process reCreate "
									+ e);
							e.printStackTrace();
							try {
								Thread.sleep(1000);
							} catch (Throwable te) {
								_logger.error("ZkManager.ZkWatcher.process Thread.sleep "
										+ e);
								continue;
							}
						}
					}
				}
			} else if (event.getPath().length() != 0) {
				notifyHandle();
			}
		}
	}

	private class ReNotifyThread implements Runnable {

		private boolean _isRunning = false;

		public void setRunningState(boolean flag) {
			_logger.info("ZkManager.ReNotifyThread.setRunningState flag "
					+ flag);
			synchronized (this) {
				_isRunning = flag;
				if (_isRunning) {
					this.notifyAll();
				}
			}
		}

		@Override
		public void run() {
			while (true) {
				_logger.info("ZkManager.ReNotifyThread.run start");
				try {
					Thread.sleep(1000);
				} catch (InterruptedException e1) {
					_logger.error("ReNotifyThread.run " + e1);
				}

				synchronized (this) {
					if (!_isRunning) {
						try {
							wait();
						} catch (InterruptedException e) {
							_logger.error("ReNotifyThread.run wait() " + e);
						}
					}
				}

				notifyHandle();
			}
		}

	}

}
