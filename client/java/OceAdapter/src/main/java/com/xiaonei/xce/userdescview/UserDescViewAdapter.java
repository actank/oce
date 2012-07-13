package com.xiaonei.xce.userdescview;

import xce.userdesc.UserDescDataN;
import xce.userdesc.UserDescReaderPrx;
import xce.userdesc.UserDescReaderPrxHelper;

import com.xiaonei.xce.usercontact.UserContactFactory;
import com.xiaonei.xce.usercontact.UserContactInfo;
import com.xiaonei.xce.userfav.UserFavFactory;
import com.xiaonei.xce.userfav.UserFavInfo;
import com.xiaonei.xce.usertime.UserTimeFactory;
import com.xiaonei.xce.usertime.UserTimeInfo;
import com.xiaonei.xce.log.TimeCost;

import xce.clusterstate.ReplicatedClusterAdapter;
import com.xiaonei.xce.log.Oce;
import Ice.TimeoutException;

public final class UserDescViewAdapter<V extends UserDescView<T, C, F>, T extends UserTimeInfo, C extends UserContactInfo, F extends UserFavInfo>
		extends ReplicatedClusterAdapter{

	private static final String ctr_endpoints = "ControllerUserDescReader";

	private static final String service_name = "M";
	
	private static int _interval = 120;

	public UserDescViewAdapter(UserDescViewFactory<V> factory,
			UserTimeFactory<T> timeFactory,
			UserContactFactory<C> contactFactory, UserFavFactory<F> favFactory) {
		super(ctr_endpoints, _interval);
		_factory = factory;
		_timeFactory = timeFactory;
		_contactFactory = contactFactory;
		_favFactory = favFactory;
	}


	protected final UserDescViewFactory<V> _factory;

	protected final UserTimeFactory<T> _timeFactory;

	protected final UserContactFactory<C> _contactFactory;

	protected final UserFavFactory<F> _favFactory;

	protected UserDescReaderPrx getUserDescReaderPrx(int userId) {
		Ice.ObjectPrx prx0 = getValidProxy(userId,service_name, 300);
		if (prx0 != null) {
			UserDescReaderPrx prx = UserDescReaderPrxHelper
					.uncheckedCast(prx0.ice_timeout(300).ice_twoway());
			return prx;
		} else {
			return null;
		}
	}
	
	public V getUserDescView(int id) {
		TimeCost cost = TimeCost.begin("UserDescViewAdapter.getUserDescView id: "+id);
		UserDescReaderPrx prx = null ;
		try {
			V result = _factory.create();
			if (id <= 0) {
				return result;
			}
			prx = getUserDescReaderPrx(id);
			UserDescDataN data = prx.getUserDescN(id);

			T time = _timeFactory.create();
			time.parse(data.userTime);
			result.setUserTimeView(time);

			C contact = _contactFactory.create();
			contact.parse(data.userContact);
			result.setUserContactView(contact);

			F fav = _favFactory.create();
			fav.parse(data.userFav);
			result.setUserFavView(fav);

			return result;
		} catch (TimeoutException e) 
		{
			Oce.getLogger().error(this.getClass().getName() + ".getUserDescView [" + prx + "] id=" + id + " Ice.TimeoutException");
			throw e;
		} finally {
			cost.endFinally();
		}
	}

	public static void main(String[] args) {

		// int id = 233940269;
		/*
		 * getUserDesc用法
		 */
		// UserDescViewInfo data = new
		// UserDescViewAdapter.getInstance().getUserDescView(id);
		// System.out.println("UserDescResult.id: " + data.userTimeView.id +
		// System.out.println("UserDescResult.activateTime: " +
		// data.userTimeView.activateTime + " UserDescResult.qq: " +
		// data.userContactView.qq + "...");
		// System.out.println("UserDescResult.favFlag: " +
		// data.userFavView.favFlag + " UserDescResult.msn: " +
		// data.userContactView.msn + "...");
	}

}
