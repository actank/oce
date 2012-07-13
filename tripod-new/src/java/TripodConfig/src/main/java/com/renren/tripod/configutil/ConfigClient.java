package com.renren.tripod.configutil;

import java.io.IOException;

import org.apache.zookeeper.KeeperException;

public abstract class ConfigClient {
	private static org.apache.commons.logging.Log _logger = org.apache.commons.logging.LogFactory
			.getLog(ConfigClient.class);

	private String _zkAddress;
	private String _identifier;
	private ReUpdateThread _reUpdateThread = new ReUpdateThread();

	private class ReUpdateThread implements Runnable {

		private boolean _isRunning = false;

		public void setRunningState(boolean flag) {
			_logger.info("ConfigClient.ReUpdateThread.setRunningState flag" + flag);
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
				_logger.debug("ReUpdateThread.run start");
				try {
					Thread.sleep(1000);
				} catch (InterruptedException e1) {
					_logger.error("ReUpdateThread.run " + e1);
				}

				synchronized (this) {
					if (!_isRunning) {
						try {
							wait();
						} catch (InterruptedException e) {
							_logger.error("ReUpdateThread.run wait() " + e);
						}
					}
				}
				notifyHandle();
			}
		}
	}

	public ConfigClient(String zkAddress, String identifier) {
		_logger.info("ConfigClient.CongClient zkAddress " + zkAddress
				+ " identifier " + identifier);
		_zkAddress = zkAddress;
		_identifier = identifier;
	}

	public boolean initialize() {
		ConfigManager cm;
		try {
			cm = ConfigManagerFactory.get(_zkAddress);
			if (cm == null) {
				_logger.error("ConfigClient.initialize ConfigManager is null for zkaddress "
						+ _zkAddress);
				return false;
			}
			cm.registerClient(this);

			ConfigData newConfig = cm.getConfigData();
			if (update(newConfig) == false) {
				_logger.error("ConfigClient.initialize update failed");
				return false;
			}
			Thread thr = new Thread(_reUpdateThread);
			thr.start();
			return true;
		} catch (IOException e) {
			e.printStackTrace();
			_logger.error("ConfigClient.initialize IOException " + e);
			return false;
		} catch (KeeperException e) {
			e.printStackTrace();
			_logger.error("ConfigClient.initialize KeeperException " + e);
			return false;
		} catch (InterruptedException e) {
			_logger.error("ConfigClient.initialize InterruptedException " + e);
			e.printStackTrace();
			return false;
		}
	}

	public void notifyHandle() {
		_logger.info("ConfigClient.notifyHandle");
		try {
			ConfigData newConfig = ConfigManagerFactory.get(_zkAddress)
					.getConfigData();
			if (update(newConfig) == false) {
				_logger.error("ConfigClient.notifyHandle update failed");
				_reUpdateThread.setRunningState(true);
				return;
			}
		} catch (IOException e) {
			_logger.error("ConfigClient.notifyHandle IOException " + e);
			e.printStackTrace();
		} catch (KeeperException e) {
			_logger.error("ConfigClient.notifyHandle KeeperException " + e);
			e.printStackTrace();
		} catch (InterruptedException e) {
			_logger.error("ConfigClient.notifyHandle InterruptedException " + e);
			e.printStackTrace();
		}
		_reUpdateThread.setRunningState(false);
	}

	protected abstract boolean update(ConfigData newConfig);
}
