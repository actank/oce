package com.xiaonei.xce.userbaseview;

import java.util.Vector;

import xce.userbase.UserBaseViewData;
import xce.userbase.UserBaseViewManagerPrx;
import xce.userbase.UserBaseViewManagerPrxHelper;
import xce.util.channel.Channel;
import xce.util.service.ServiceAdapter;
import Ice.TimeoutException;

import com.xiaonei.xce.log.Oce;
import com.xiaonei.xce.log.TimeCost;
import com.xiaonei.xce.userbasic.UserBasicFactory;
import com.xiaonei.xce.userbasic.UserBasicInfo;
import com.xiaonei.xce.username.UserNameFactory;
import com.xiaonei.xce.username.UserNameInfo;
import com.xiaonei.xce.userpassport.UserPassportFactory;
import com.xiaonei.xce.userpassport.UserPassportInfo;
import com.xiaonei.xce.userright.UserRightFactory;
import com.xiaonei.xce.userright.UserRightInfo;
import com.xiaonei.xce.userstate.UserStateFactory;
import com.xiaonei.xce.userstate.UserStateInfo;
import com.xiaonei.xce.userurl.UserUrlFactory;
import com.xiaonei.xce.userurl.UserUrlInfo;

import xce.clusterstate.ReplicatedClusterAdapter;

public class UserBaseFullViewAdapter<T extends UserBaseFullView<P, R, N, S, B, U>, P extends UserPassportInfo, R extends UserRightInfo, N extends UserNameInfo, S extends UserStateInfo, B extends UserBasicInfo, U extends UserUrlInfo>
    extends ReplicatedClusterAdapter {
	private static final String ctr_endpoints = "ControllerUserBase";
 
	private static final String service_name = "UBVM";

	private static int _interval = 120;


  protected final UserBaseFullViewFactory<T> _viewFactory;

  protected final UserPassportFactory<P> _passportFactory;
  protected final UserRightFactory<R> _rightFactory;
  protected final UserNameFactory<N> _nameFactory;
  protected final UserStateFactory<S> _stateFactory;
  protected final UserBasicFactory<B> _basicFactory;
  protected final UserUrlFactory<U> _urlFactory;

  public UserBaseFullViewAdapter(UserBaseFullViewFactory<T> viewFactory,
      UserPassportFactory<P> passportFactory, UserRightFactory<R> rightFactory,
      UserNameFactory<N> nameFactory, UserStateFactory<S> stateFactory,
      UserBasicFactory<B> basicFactory, UserUrlFactory<U> urlFactory) {
	  super(ctr_endpoints, _interval);
    _viewFactory = viewFactory;
    _passportFactory = passportFactory;
    _rightFactory = rightFactory;
    _nameFactory = nameFactory;
    _stateFactory = stateFactory;
    _basicFactory = basicFactory;
    _urlFactory = urlFactory;
  }

  public T getUserBaseView(int userId) throws Exception {
    TimeCost cost = TimeCost
        .begin("UserBaseFullViewAdapter.getUserBaseView id: " + userId);
    UserBaseViewManagerPrx prx = null;
    T res = _viewFactory.create();

    try {
	if (userId <= 0) {
		return res;
	}
	 prx = getUserBaseViewManager(userId);
      UserBaseViewData ubvd = prx.getUserBaseView(
          userId);
      P passport = _passportFactory.create();
      passport.parse(ubvd.upd);
      res.setUserPassportInfo(passport);

      R right = _rightFactory.create();
      right.parse(ubvd.urd);
      res.setUserRightInfo(right);

      N name = _nameFactory.create();
      name.parse(ubvd.und);
      res.setUserNameInfo(name);

      S state = _stateFactory.create();
      state.parse(ubvd.usd);
      res.setUserStateInfo(state);

      res.setUserLoginCountInfo(ubvd.ulcd.logincount);

      B basic = _basicFactory.create();
      basic.parse(ubvd.userbasic);
      res.setUserBasic(basic);

      U url = _urlFactory.create();
      url.parse(ubvd.userurl);
      res.setUserUrl(url);

      return res;
    } catch (TimeoutException e) {
      Oce.getLogger().error(
          this.getClass().getName() + ".getUserBaseView " + " prx = " + prx + " id=" + userId
              + " Ice.TimeoutException");
      throw e;
    } finally {
      cost.endFinally();
    }
  }

	protected UserBaseViewManagerPrx getUserBaseViewManager(int userId) {
		Ice.ObjectPrx prx0 = getValidProxy(userId,service_name, 300);
		if (prx0 != null) {
			UserBaseViewManagerPrx prx = UserBaseViewManagerPrxHelper
					.uncheckedCast(prx0.ice_timeout(300).ice_twoway());
			return prx;
		} else {
			return null;
		}
	}
}
