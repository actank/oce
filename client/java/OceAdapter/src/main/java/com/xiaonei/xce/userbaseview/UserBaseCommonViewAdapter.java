package com.xiaonei.xce.userbaseview;

import xce.userbase.UserBaseViewManagerPrx;
import xce.userbase.UserBaseViewManagerPrxHelper;
import xce.userbase.UserCommonViewData;
import Ice.ConnectTimeoutException;
import Ice.TimeoutException;

import com.xiaonei.xce.idcache.IdInfoAdapter;
import com.xiaonei.xce.log.Oce;
import com.xiaonei.xce.log.TimeCost;
import com.xiaonei.xce.userdanger.UserDangerInfo;
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

public class UserBaseCommonViewAdapter<T extends UserBaseCommonView<P, R, N, S, U>, P extends UserPassportInfo, R extends UserRightInfo, N extends UserNameInfo, S extends UserStateInfo, U extends UserUrlInfo>
		extends ReplicatedClusterAdapter {

	private static final String ctr_endpoints = "ControllerUserBase";
 
	private static final String service_name = "UBVM";

	private static int _interval = 120;

	protected final UserBaseCommonViewFactory<T> _viewFactory;

	protected final UserPassportFactory<P> _passportFactory;
	protected final UserRightFactory<R> _rightFactory;
	protected final UserNameFactory<N> _nameFactory;
	protected final UserStateFactory<S> _stateFactory;
	protected final UserUrlFactory<U> _urlFactory;
	
	private IdInfoAdapter _idInfoAdapter = null;

	public UserBaseCommonViewAdapter(UserBaseCommonViewFactory<T> viewFactory,
			UserPassportFactory<P> passportFactory,
			UserRightFactory<R> rightFactory, UserNameFactory<N> nameFactory,
			UserStateFactory<S> stateFactory, UserUrlFactory<U> urlFactory) {
		super(ctr_endpoints, _interval);
		_viewFactory = viewFactory;
		_passportFactory = passportFactory;
		_rightFactory = rightFactory;
		_nameFactory = nameFactory;
		_stateFactory = stateFactory;
		_urlFactory = urlFactory;
		_idInfoAdapter = new IdInfoAdapter();
	}

	public T getUserBaseView(int userId) {
		TimeCost cost = TimeCost.begin("UserBaseCommonViewAdapter.getUserBaseView id: "+userId);

		T res = _viewFactory.create();
		UserBaseViewManagerPrx prx = null;
    int realUserId = userId;
		try {
			if (userId <= 0) {
				return res;
			}
			
			if(_idInfoAdapter.isVirtualId(userId)){
				int relatedTransId = _idInfoAdapter.getRelatedTransId(userId);
				if(relatedTransId > 0){
          int relatedUserId = _idInfoAdapter.getRelatedUserId(userId);
          if(relatedUserId > 0) {
            realUserId = relatedUserId;
          }
					userId = relatedTransId;
				}

			}
			
			prx = getUserBaseViewManager(userId);
			UserCommonViewData ubvd = prx.getUserCommonView(userId);
      res.setRealUserId(realUserId);
			P passport = _passportFactory.create();
			passport.parse(ubvd.userpassport);
			res.setUserPassportInfo(passport);

			R right = _rightFactory.create();
			right.parse(ubvd.userright);
			res.setUserRightInfo(right);

			N name = _nameFactory.create();
			name.parse(ubvd.username);
			res.setUserNameInfo(name);

			S state = _stateFactory.create();
			state.parse(ubvd.userstate);
			res.setUserStateInfo(state);

			res.setUserLoginCountInfo(ubvd.userlogincount.logincount);

			U url = _urlFactory.create();
			url.parse(ubvd.userurl);
			res.setUserUrl(url);

			res.setStage(ubvd.stage);
			res.setUniv(ubvd.univ);
			res.setGender(ubvd.gender);
			res.setUserDangerInfo(new UserDangerInfo(userId));
			return res;
		} catch(ConnectTimeoutException e) {
			Oce.getLogger().error(
					this.getClass().getName() + ".getUserBaseView [" + prx
					+ "] userId=" + userId + " Ice.ConnectTimeoutException");
			throw e;
	    } catch (TimeoutException e) {
			Oce.getLogger().error(this.getClass().getName() + ".getUserBaseView [" + prx 
					+ "] userId=" + userId + " Ice.TimeoutException");
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
