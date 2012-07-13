package com.xiaonei.xce.buddyadapter.buddybyapplicantcache;

import com.xiaonei.xce.log.Oce;

import xce.buddy.BuddyByApplicantCacheManagerPrx;
import xce.buddy.BuddyByApplicantCacheManagerPrxHelper;
import xce.buddy.BuddyByApplicantLoaderPrx;
import xce.buddy.BuddyByApplicantLoaderPrxHelper;
import xce.clusterstate.ReplicatedClusterAdapter;

/**
 * BuddyByApplicantCacheAdapter
 * 
 * @author yuyang(yang.yu@opi-corp.com)
 * 
 */

public class BuddyByApplicantCacheAdapter<T extends BuddyByApplicantCacheAccepterInfo, W extends BuddyByApplicantCacheData<T>>
		extends ReplicatedClusterAdapter {
	private static final String ctr_endpoints = "ControllerBuddyByApplicantCache";
	private final BuddyByApplicantCacheFactory<T, W> _factory;
	private static int _interval = 120;

	private static final BuddyByApplicantLoaderAdapter _loader = new BuddyByApplicantLoaderAdapter();

	/**
	 * @param factory
	 */
	public BuddyByApplicantCacheAdapter(BuddyByApplicantCacheFactory<T, W> factory) {
		super(ctr_endpoints, _interval);
		_factory = factory;
		// TODO Auto-generated constructor stub
	}

	/**
	 * @param id
	 * @return
	 */
	protected BuddyByApplicantCacheManagerPrx getBuddyByApplicantCacheManagerTwoway(
			int id) {
		Ice.ObjectPrx prx0 = getValidProxy(id, 300);
		if (prx0 != null) {
			BuddyByApplicantCacheManagerPrx prx = BuddyByApplicantCacheManagerPrxHelper
					.uncheckedCast(prx0.ice_timeout(300).ice_twoway());
			return prx;
		} else {
			return null;
		}
	}

	/**
	 * @param id
	 * @return
	 */
	protected BuddyByApplicantCacheManagerPrx getBuddyByApplicantCacheManagerOneway(
			int id) {
		Ice.ObjectPrx prx0 = getValidProxy(id, 300);
		if (prx0 != null) {
			BuddyByApplicantCacheManagerPrx prx = BuddyByApplicantCacheManagerPrxHelper
					.uncheckedCast(prx0.ice_timeout(300).ice_oneway());
			return prx;
		} else {
			return null;
		}
	}

	/**
	 * @param id
	 * @param begin
	 * @param limit
	 * @return
	 */
	public W getBuddyByApplicantCacheData(int id, int begin, int limit) {
		W result = _factory.createBuddyByApplicantCacheData();
		BuddyByApplicantCacheManagerPrx prx = null;
		try {
			prx = getBuddyByApplicantCacheManagerTwoway(id);
			xce.buddy.BuddyByApplicantData res = prx.getAccepters(id, begin, limit);
			if (res == null)
				return result;
			result.setBase(res, _factory);
		} catch (Ice.ConnectTimeoutException e) {
			Oce.getLogger().error(
					this.getClass().getName() + ".getBuddyByApplicantCacheData  [ " + prx
							+ " ] id=" + id + " " + e);
		} catch (Ice.TimeoutException e) {
			Oce.getLogger().error(
					this.getClass().getName() + ".getBuddyByApplicantCacheData  [ " + prx
							+ " ] id=" + id + " " + e);
		} catch (java.lang.Throwable e) {
			Oce.getLogger().error(
					this.getClass().getName() + ".getBuddyByApplicantCacheData " + id
							+ " ", e);
		}

		return result;
	}
	
	/**
	 * @param id
	 * @param begin
	 * @param limit
	 * @return
	 */
	public W getBuddyByApplicantCacheDataSync(int id, int begin, int limit) {
		W result = _factory.createBuddyByApplicantCacheData();
		BuddyByApplicantCacheManagerPrx prx = null;
		try {
			prx = getBuddyByApplicantCacheManagerTwoway(id);
			xce.buddy.BuddyByApplicantData res = prx.getAcceptersSync(id, begin, limit);
			if (res == null)
				return result;
			result.setBase(res, _factory);
		} catch (Ice.ConnectTimeoutException e) {
			Oce.getLogger().error(
					this.getClass().getName() + ".getBuddyByApplicantCacheDataSync  [ " + prx
							+ " ] id=" + id + " " + e);
		} catch (Ice.TimeoutException e) {
			Oce.getLogger().error(
					this.getClass().getName() + ".getBuddyByApplicantCacheDataSync  [ " + prx
							+ " ] id=" + id + " " + e);
		} catch (java.lang.Throwable e) {
			Oce.getLogger().error(
					this.getClass().getName() + ".getBuddyByApplicantCacheDataSync " + id
							+ " ", e);
		}

		return result;
	}

	/**
	 * @param id
	 * @param accepter
	 */
	public void addAccepter(int id, T accepter) {
		_loader.addAccepter(id, accepter.getBase());
	}

	/**
	 * @param id
	 * @param accepterId
	 */
	public void removeAccepter(int id, int accepterId) {
		_loader.removeAccepter(id, accepterId);
	}

	/**
	 * @param id
	 */
	public void reload(int id) {
		_loader.reload(id);
	}

	/**
	 * @param id
	 */
	public void clear(int id) {
		_loader.clear(id);
	}

	/**
	 * @author yuyang
	 * 
	 */
	private static class BuddyByApplicantLoaderAdapter extends
			ReplicatedClusterAdapter {

		private static final String _loader_endpoints = "ControllerBuddyByApplicantLoader";
		private static int _loader_interval = 120;

		/**
		 * 
		 */
		public BuddyByApplicantLoaderAdapter() {
			super(_loader_endpoints, _loader_interval);
			// TODO Auto-generated constructor stub
		}

		/**
		 * @param id
		 * @param accepter
		 */
		public void addAccepter(int id, xce.buddy.AccepterInfo accepter) {
			BuddyByApplicantLoaderPrx prx = null;
			try {
				prx = getBuddyByApplicantLoaderTwoway(id);
				prx.addAccepter(id, accepter);
			} catch (Ice.ConnectTimeoutException e) {
				Oce.getLogger().error(
						this.getClass().getName() + ".addAccepter  [ " + prx + " ] id="
								+ id + " " + e);
			} catch (Ice.TimeoutException e) {
				Oce.getLogger().error(
						this.getClass().getName() + ".addAccepter  [ " + prx + " ] id="
								+ id + " " + e);
			} catch (java.lang.Throwable e) {
				Oce.getLogger().error(
						this.getClass().getName() + ".addAccepter " + id + " ", e);
			}
		}

		/**
		 * @param id
		 * @param accepterId
		 */
		public void removeAccepter(int id, int accepterId) {
			BuddyByApplicantLoaderPrx prx = null;
			try {
				prx = getBuddyByApplicantLoaderTwoway(id);
				prx.removeAccepter(id, accepterId);
			} catch (Ice.ConnectTimeoutException e) {
				Oce.getLogger().error(
						this.getClass().getName() + ".removeAccepter  [ " + prx + " ] id="
								+ id + " " + e);
			} catch (Ice.TimeoutException e) {
				Oce.getLogger().error(
						this.getClass().getName() + ".removeAccepter  [ " + prx + " ] id="
								+ id + " " + e);
			} catch (java.lang.Throwable e) {
				Oce.getLogger().error(
						this.getClass().getName() + ".removeAccepter " + id + " ", e);
			}
		}

		/**
		 * @param id
		 */
		public void reload(int id) {
			BuddyByApplicantLoaderPrx prx = null;
			try {
				prx = getBuddyByApplicantLoaderTwoway(id);
				prx.reload(id);
			} catch (Ice.ConnectTimeoutException e) {
				Oce.getLogger().error(
						this.getClass().getName() + ".reload  [ " + prx + " ] id=" + id
								+ " " + e);
			} catch (Ice.TimeoutException e) {
				Oce.getLogger().error(
						this.getClass().getName() + ".reload  [ " + prx + " ] id=" + id
								+ " " + e);
			} catch (java.lang.Throwable e) {
				Oce.getLogger().error(
						this.getClass().getName() + ".reload " + id + " ", e);
			}
		}

		/**
		 * @param id
		 */
		public void clear(int id) {
			BuddyByApplicantLoaderPrx prx = null;
			try {
				prx = getBuddyByApplicantLoaderTwoway(id);
				prx.clear(id);
			} catch (Ice.ConnectTimeoutException e) {
				Oce.getLogger().error(
						this.getClass().getName() + ".clear  [ " + prx + " ] id=" + id
								+ " " + e);
			} catch (Ice.TimeoutException e) {
				Oce.getLogger().error(
						this.getClass().getName() + ".clear  [ " + prx + " ] id=" + id
								+ " " + e);
			} catch (java.lang.Throwable e) {
				Oce.getLogger().error(this.getClass().getName() + ".clear " + id + " ",
						e);
			}
		}

		/**
		 * @param id
		 * @return
		 */
		protected BuddyByApplicantLoaderPrx getBuddyByApplicantLoaderTwoway(int id) {
			Ice.ObjectPrx prx0 = getValidProxy(id, 300);
			if (prx0 != null) {
				BuddyByApplicantLoaderPrx prx = BuddyByApplicantLoaderPrxHelper
						.uncheckedCast(prx0.ice_timeout(300).ice_twoway());
				return prx;
			} else {
				return null;
			}
		}

		/**
		 * @param id
		 * @return
		 */
		protected BuddyByApplicantLoaderPrx getBuddyByApplicantLoaderOneway(int id) {
			Ice.ObjectPrx prx0 = getValidProxy(id, 300);
			if (prx0 != null) {
				BuddyByApplicantLoaderPrx prx = BuddyByApplicantLoaderPrxHelper
						.uncheckedCast(prx0.ice_timeout(300).ice_oneway());
				return prx;
			} else {
				return null;
			}
		}

	}

}
