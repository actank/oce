package mop.hi.oce.adapter;

import mop.hi.oce.adapter.impl.AccountAdapterImpl;
import mop.hi.oce.adapter.impl.Antispam2AdapterImpl;
import mop.hi.oce.adapter.impl.AntispamAdapterImpl;
import mop.hi.oce.adapter.impl.BatchUpdaterLeavewordAdapterImpl;
import mop.hi.oce.adapter.impl.BatchUpdaterMasterAdapterImpl;
import mop.hi.oce.adapter.impl.BatchUpdaterSelfAdapterImpl;
import mop.hi.oce.adapter.impl.BatchUpdaterValueAdapterImpl;
import mop.hi.oce.adapter.impl.BuddyCoreAdapterImpl;
import mop.hi.oce.adapter.impl.ClassApplyCounterAdapterImpl;
//import mop.hi.oce.adapter.impl.ClassmateEventAdapterImpl;
import mop.hi.oce.adapter.impl.FeedAdapterImpl;
import mop.hi.oce.adapter.impl.FootprintAdapterImpl;
import mop.hi.oce.adapter.impl.ImporterNAdapterImpl;

import com.xiaonei.xce.cardfacade.CardFacadeAdapter;
import com.xiaonei.xce.footprint.AppFootprintAdapterImpl;
import com.xiaonei.xce.footprint.FootprintNewAdapter;
import com.xiaonei.xce.footprint.FootprintAdapterReplicaImpl;
import com.xiaonei.xce.footprintunite.FootprintUniteAdapter;
import com.xiaonei.xce.footprintunite.VisitFootprintAdapter;

import mop.hi.oce.adapter.impl.GeneralFilterAdapterImpl;
import mop.hi.oce.adapter.impl.ImporterAdapterImpl;
import mop.hi.oce.adapter.impl.NetworkAdapterMCImpl;
import mop.hi.oce.adapter.impl.OnlineBuddyAdapterImpl;
import mop.hi.oce.adapter.impl.PassportAdapterImpl;
import mop.hi.oce.adapter.impl.PassportAdapterMCImpl;
import mop.hi.oce.adapter.impl.PassportWapAdapterImpl;
//import mop.hi.oce.adapter.impl.ReplyEventAdapterImpl;
//import mop.hi.oce.adapter.impl.TribeEventAdapterImpl;
import mop.hi.oce.adapter.impl.WebOnlineAdapterImpl;

public class AdapterFactory {
	private static BuddyCoreAdapter buddyCoreAdapter;
	// private static EventAdapter replyEventAdapter;
	// private static EventAdapter tribeEventAdapter;
	private static FootprintNewAdapter userFootprintNewAdapter;
	private static FootprintNewAdapter appFootprintAdapter;
	private static FootprintAdapter tribeFootprintAdapter;
	private static FootprintAdapter blogFootprintAdapter;

	private static CardFacadeAdapter cardFacadeAdapter;
	private static FootprintUniteAdapter homeFootprintAdapter;
	private static VisitFootprintAdapter visitFootprintAdapter;
	// private static ClassmateEventAdapterImpl classmateEventAdapter;
	private static MasterBatchUpdaterAdapter masterBatchUpdaterAdapter;
	private static BatchUpdaterSelfAdapter selfBatchUpdaterAdapter;
	private static BatchUpdaterLeavewordAdapter leavewordBatchUpdaterAdapter;
	private static OnlineBuddyAdapter onlineBuddyAdapter;
	private static WebOnlineAdapter webOnlineAdapter;
	/*
	 * generalFilterAdapter has two interfaces, but the flterControl is not very
	 * proper, suggest not to use.mop.hi.oce.adapter
	 */
	private static GeneralFilterAdapterImpl generalFilterAdapter;
	private static PassportAdapter passportAdapter;
	private static PassportAdapter passportWapAdapter;

	private static AntispamAdapter antispamAdapter;
	private static Antispam2Adapter antispam2Adapter;
	private static NetworkAdapter networkAdapter;
	private static FeedAdapter feedAdapter;
	private static AccountAdapter accountAdapter;
	private static ImporterAdapter importerAdapter;
	private static ImporterNAdapter importernAdapter;

	private static ClassApplyCounterAdapter classApplyCounterAdapter;

	public static ClassApplyCounterAdapter getClassApplyCounterAdapter() {
		if (classApplyCounterAdapter == null) {
			synchronized (AdapterFactory.class) {
				if (classApplyCounterAdapter == null) {
					classApplyCounterAdapter = new ClassApplyCounterAdapterImpl();
				}
			}
		}
		return classApplyCounterAdapter;
	}

	public static OnlineBuddyAdapter getOnlineBuddyAdapter() {
		if (onlineBuddyAdapter == null) {
			synchronized (AdapterFactory.class) {
				if (onlineBuddyAdapter == null) {
					onlineBuddyAdapter = new OnlineBuddyAdapterImpl();
				}
			}
		}
		return onlineBuddyAdapter;
	}

	public static ImporterAdapter getImporterAdapter() {
		if (importerAdapter == null) {
			synchronized (AdapterFactory.class) {
				if (importerAdapter == null) {
					importerAdapter = new ImporterAdapterImpl();
				}
			}
		}
		return importerAdapter;
	}

	public static ImporterNAdapter getImporterNAdapter() {
		if (importernAdapter == null) {
			synchronized (AdapterFactory.class) {
				if (importernAdapter == null) {
					importernAdapter = new ImporterNAdapterImpl();
				}
			}
		}
		return importernAdapter;
	}

	public static AccountAdapter getAccountAdapter() {
		if (accountAdapter == null) {
			synchronized (AdapterFactory.class) {
				if (accountAdapter == null) {
					accountAdapter = new AccountAdapterImpl();
				}
			}
		}
		return accountAdapter;
	}

	public static FeedAdapter getFeedAdapter() {
		if (feedAdapter == null) {
			synchronized (AdapterFactory.class) {
				if (feedAdapter == null) {
					feedAdapter = new FeedAdapterImpl();
				}
			}
		}
		return feedAdapter;
	}

	public static NetworkAdapter getNetworkAdapter() {
		if (networkAdapter == null) {
			synchronized (AdapterFactory.class) {
				if (networkAdapter == null) {
					networkAdapter = new NetworkAdapterMCImpl();
				}
			}
		}
		return networkAdapter;
	}

	public static AntispamAdapter getAntispamAdapter() {
		if (antispamAdapter == null) {
			synchronized (AntispamAdapterImpl.class) {
				if (antispamAdapter == null) {
					antispamAdapter = new AntispamAdapterImpl();
				}
			}
		}
		return antispamAdapter;
	}

	public static Antispam2Adapter getAntispam2Adapter() {
		if (antispam2Adapter == null) {
			synchronized (Antispam2AdapterImpl.class) {
				if (antispam2Adapter == null) {
					antispam2Adapter = new Antispam2AdapterImpl();
				}
			}
		}
		return antispam2Adapter;
	}

	// public static ClassmateEventAdapterImpl getClassmateEventAdapter() {
	// if (classmateEventAdapter == null) {
	// synchronized (ClassmateEventAdapterImpl.class) {
	// if (classmateEventAdapter == null) {
	// classmateEventAdapter = new ClassmateEventAdapterImpl();
	// }
	// }
	// }
	// return classmateEventAdapter;
	// }

	/*
	 * @Deprecated public static FootprintAdapter getClassFootprintAdapter() {
	 * if (classFootprintAdapter == null) { synchronized
	 * (FootprintAdapter.class) { if (classFootprintAdapter == null) {
	 * classFootprintAdapter = new FootprintAdapterImpl( "@ClassFootprint", 1,
	 * false); } } } return classFootprintAdapter; }
	 */

	public static FootprintAdapter getTribeFootprintAdapter() {
		if (tribeFootprintAdapter == null) {
			synchronized (FootprintAdapter.class) {
				if (tribeFootprintAdapter == null) {
					tribeFootprintAdapter = new FootprintAdapterImpl(
							"@TribeFootprintL", 1, false);
				}
			}
		}
		return tribeFootprintAdapter;
	}

	public static FootprintAdapter getBlogFootprintAdapter() {
		if (blogFootprintAdapter == null) {
			synchronized (FootprintAdapter.class) {
				if (blogFootprintAdapter == null) {
					blogFootprintAdapter = new FootprintAdapterImpl(
							"@BlogFootprintL", 1, false);
				}
			}
		}
		return blogFootprintAdapter;
	}

	public static CardFacadeAdapter getCardFacadeAdapter() {
		if (cardFacadeAdapter == null) {
			synchronized (CardFacadeAdapter.class) {
				if (cardFacadeAdapter == null) {
					cardFacadeAdapter = new CardFacadeAdapter();
				}
			}
		}
		return cardFacadeAdapter;
	}

	/*
	 * @Deprecated public static FootprintAdapter getPhotoFootprintAdapter() {
	 * if (photoFootprintAdapter == null) { synchronized
	 * (FootprintAdapter.class) { if (photoFootprintAdapter == null) {
	 * photoFootprintAdapter = new FootprintAdapterImpl( "@PhotoFootprint", 1,
	 * false); } } } return photoFootprintAdapter; }
	 */

	public static BuddyCoreAdapter getBuddyCoreAdapter() {
		if (buddyCoreAdapter == null) {
			synchronized (AdapterFactory.class) {
				if (buddyCoreAdapter == null) {
					buddyCoreAdapter = new BuddyCoreAdapterImpl();
				}
			}
		}
		return buddyCoreAdapter;
	}

	// public static EventAdapter getReplyEventAdapter() {
	// if (replyEventAdapter == null) {
	// synchronized (AdapterFactory.class) {
	// if (replyEventAdapter == null) {
	// replyEventAdapter = new ReplyEventAdapterImpl();
	// }
	// }
	// }
	// return replyEventAdapter;
	// }

	// public static EventAdapter getTribeEventAdapter() {
	// if (tribeEventAdapter == null) {
	// synchronized (AdapterFactory.class) {
	// if (tribeEventAdapter == null) {
	// tribeEventAdapter = new TribeEventAdapterImpl();
	// }
	// }
	// }
	// return tribeEventAdapter;
	// }

	/*
	 * @Deprecated public static FootprintAdapter getFootprintAdapter() { if
	 * (userFootprintAdapter == null) { synchronized (AdapterFactory.class) { if
	 * (userFootprintAdapter == null) { userFootprintAdapter = new
	 * FootprintAdapterImpl( "@Footprint", 10, true); } } } return
	 * userFootprintAdapter;FootprintNewView fptView=new FootprintNewView();
	 * xce.fptn.Footprint fpn=fptView.getFootprint();
	 * fpn.tinyinfo.guest=fpt.getGuest();
	 * 
	 * }
	 */

	/*
	 * @Deprecated public static FootprintNewAdapter getFootprintNewAdapter() {
	 * if (userFootprintNewAdapter == null) { synchronized
	 * (AdapterFactory.class) { if (userFootprintNewAdapter == null) {
	 * userFootprintNewAdapter = new FootprintAdapterReplicaImpl(
	 * "@FootprintNew", 10, true); } } } return userFootprintNewAdapter; }
	 */
	public static FootprintUniteAdapter getHomeFootprintAdapter() {
		if (homeFootprintAdapter == null) {
			synchronized (AdapterFactory.class) {
				if (homeFootprintAdapter == null) {
					homeFootprintAdapter = new FootprintUniteAdapter();
				}
			}
		}
		return homeFootprintAdapter;
	}

	public static VisitFootprintAdapter getVisitFootprintAdapter() {
		if (visitFootprintAdapter == null) {
			synchronized (AdapterFactory.class) {
				if (visitFootprintAdapter == null) {
					visitFootprintAdapter = new VisitFootprintAdapter();
				}
			}
		}
		return visitFootprintAdapter;
	}

	public static FootprintNewAdapter getAppFootprintAdapter() {
		if (appFootprintAdapter == null) {
			synchronized (AdapterFactory.class) {
				if (appFootprintAdapter == null) {
					appFootprintAdapter = new AppFootprintAdapterImpl(
							"@AppFootprint", 10);
				}
			}
		}
		return appFootprintAdapter;
	}

	@Deprecated
	public static WebOnlineAdapter getWebOnlineAdapter() {
		if (webOnlineAdapter == null) {
			synchronized (AdapterFactory.class) {
				if (webOnlineAdapter == null) {
					webOnlineAdapter = new WebOnlineAdapterImpl();
				}
			}
		}
		return webOnlineAdapter;
	}

	public static MasterBatchUpdaterAdapter getMasterBatchUpdaterAdapter() {
		if (masterBatchUpdaterAdapter == null) {
			synchronized (AdapterFactory.class) {
				if (masterBatchUpdaterAdapter == null) {
					masterBatchUpdaterAdapter = new BatchUpdaterMasterAdapterImpl();
				}
			}
		}
		return masterBatchUpdaterAdapter;
	}

	public static BatchUpdaterSelfAdapter getSelfBatchUpdaterAdapter() {
		if (selfBatchUpdaterAdapter == null) {
			synchronized (AdapterFactory.class) {
				if (selfBatchUpdaterAdapter == null) {
					selfBatchUpdaterAdapter = new BatchUpdaterSelfAdapterImpl();
				}
			}
		}
		return selfBatchUpdaterAdapter;
	}

	public static BatchUpdaterLeavewordAdapter getLeavewordBatchUpdaterAdapter() {
		if (leavewordBatchUpdaterAdapter == null) {
			synchronized (AdapterFactory.class) {
				if (leavewordBatchUpdaterAdapter == null) {
					leavewordBatchUpdaterAdapter = new BatchUpdaterLeavewordAdapterImpl();
				}
			}
		}
		return leavewordBatchUpdaterAdapter;
	}

	@Deprecated
	public static PassportAdapter getPassportAdapter() {
		if (passportAdapter == null) {
			synchronized (AdapterFactory.class) {
				if (passportAdapter == null) {
					passportAdapter = new PassportAdapterMCImpl();
				}
			}
		}
		return passportAdapter;
	}

	@Deprecated
	public static PassportAdapter getPassportWapAdapter() {
		if (passportWapAdapter == null) {
			synchronized (AdapterFactory.class) {
				if (passportWapAdapter == null) {
					passportWapAdapter = new PassportWapAdapterImpl();
				}
			}
		}
		return passportWapAdapter;
	}

	public static GeneralFilterAdapterImpl getGeneralFilterAdapter() {
		if (generalFilterAdapter == null) {
			synchronized (AdapterFactory.class) {
				if (generalFilterAdapter == null) {
					generalFilterAdapter = new GeneralFilterAdapterImpl();
				}
			}
		}
		return generalFilterAdapter;
	}

	private static BatchUpdaterValueAdapter batchUpdaterValueAdapter;

	public static BatchUpdaterValueAdapter getBatchUpdaterValueAdapter() {
		if (batchUpdaterValueAdapter == null) {
			synchronized (AdapterFactory.class) {
				if (batchUpdaterValueAdapter == null) {
					batchUpdaterValueAdapter = new BatchUpdaterValueAdapterImpl();
				}
			}
		}
		return batchUpdaterValueAdapter;
	}

}
