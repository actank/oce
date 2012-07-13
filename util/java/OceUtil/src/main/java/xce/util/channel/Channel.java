/*
 * Created on 2005-10-26
 *
 * TODO To change the template for this generated file go to
 * Window - Preferences - Java - Code Style - Code Templates
 */
package xce.util.channel;

import java.util.HashMap;
import java.util.Map;

import xce.util.tools.IPAddress;
import Ice.Communicator;
import Ice.Properties;

public abstract class Channel {
	public static enum Invoke {
		Oneway, Twoway, Udp
	};

	private static Map<String, Channel> channels = new HashMap<String, Channel>();

	protected Communicator communicator;

	protected Properties properties = Ice.Util.createProperties();

	public static Channel newOceChannel(String endpoints) {
		Channel channel = channels.get(endpoints);
		if (channel == null) {
			synchronized (Channel.class) {
				channel = channels.get(endpoints);
				if (channel == null) {
					channel = new OceChannel();
					channels.put(endpoints, channel);
				}
			}
		}
		return channel;
	}

	public static Channel[] newUserChannel(String endpoints, int channelCount) {
		System.out.println("mod value: " + channelCount);
		Channel[] multiChannel = new Channel[channelCount];
		for(int i = 0 ; i < channelCount ; i++) {
			String channelString = endpoints + i;
			Channel channel = channels.get(channelString);
			if (channel == null) {
				synchronized (Channel.class) {
					channel = channels.get(channelString);
					if (channel == null) {
						channel = new UserChannel();
						channels.put(channelString, channel);
					}
				}
			}
			multiChannel[i] = channel;
		}
		return multiChannel;
	}
	
	public static Channel newUserChannel(String endpoints) {
		Channel channel = channels.get(endpoints);
		if (channel == null) {
			synchronized (Channel.class) {
				channel = channels.get(endpoints);
				if (channel == null) {
					channel = new UserChannel();
					channels.put(endpoints, channel);
				}
			}
		}
		return channel;
	}

	public static Channel newPassportChannel(String endpoints) {
		Channel channel = channels.get(endpoints);
		if (channel == null) {
			synchronized (Channel.class) {
				channel = channels.get(endpoints);
				if (channel == null) {
					channel = new PassportChannel();
					channels.put(endpoints, channel);
				}
			}
		}
		return channel;
	}

	public static Channel newCoreChannel(String endpoints) {
		Channel channel = channels.get(endpoints);
		if (channel == null) {
			synchronized (Channel.class) {
				channel = channels.get(endpoints);
				if (channel == null) {
					channel = new CoreChannel();
					channels.put(endpoints, channel);
				}
			}
		}
		return channel;
	}

	public static Channel newSearchChannel(String endpoints) {
		Channel channel = channels.get(endpoints);
		if (channel == null) {
			synchronized (Channel.class) {
				channel = channels.get(endpoints);
				if (channel == null) {
					channel = new SearchChannel();
					channels.put(endpoints, channel);
				}
			}
		}
		return channel;
	}
	
	public static Channel newSearchFoFChannel(String endpoints) {
		Channel channel = channels.get(endpoints);
		if (channel == null) {
			synchronized (Channel.class) {
				channel = channels.get(endpoints);
				if (channel == null) {
					channel = new SearchFoFChannel();
					channels.put(endpoints, channel);
				}
			}
		}
		return channel;
	}
	public static Channel newAddressBookChannel(String endpoints) {
		Channel channel = channels.get(endpoints);
		if (channel == null) {
			synchronized (Channel.class) {
				channel = channels.get(endpoints);
				if (channel == null) {
					channel = new AddressBookChannel();
					channels.put(endpoints, channel);
				}
			}
		}
		return channel;
	}
	public static Channel newSearchCacheChannel(String endpoints) {
		Channel channel = channels.get(endpoints);
		if (channel == null) {
			synchronized (Channel.class) {
				channel = channels.get(endpoints);
				if (channel == null) {
					channel = new SearchCacheChannel();
					channels.put(endpoints, channel);
				}
			}
		}
		return channel;
	}
	public static Channel newBlogSearchChannel(String endpoints) {
		Channel channel = channels.get(endpoints);
		if (channel == null) {
			synchronized (Channel.class) {
				channel = channels.get(endpoints);
				if (channel == null) {
					channel = new BlogSearchChannel();
					channels.put(endpoints, channel);
				}
			}
		}
		return channel;
	}
	
	public static Channel newSocialGraphChannel(String endpoints){
		Channel channel = channels.get(endpoints);
		if (channel == null){
			synchronized(Channel.class){
				channel = channels.get(endpoints);
				if(channel==null){
					channel=new SocialGraphChannel();
					channels.put(endpoints, channel);
				}
			}
		}
		return channel;
	}

	public static Channel newFeedChannel(String endpoints) {
		Channel channel = channels.get(endpoints);
		if (channel == null) {
			synchronized (Channel.class) {
				channel = channels.get(endpoints);
				if (channel == null) {
					channel = new FeedChannel();
					channels.put(endpoints, channel);
				}
			}
		}
		return channel;
	}

	public static Channel newTalkChannel(String endpoints) {
		Channel channel = channels.get(endpoints);
		if (channel == null) {
			synchronized (Channel.class) {
				channel = channels.get(endpoints);
				if (channel == null) {
					channel = new TalkChannel();
					channels.put(endpoints, channel);
				}
			}
		}
		return channel;
	}
	public static Channel newFollowChannel(String endpoints) {
		Channel channel = channels.get(endpoints);
		if (channel == null) {
			synchronized (Channel.class) {
				channel = channels.get(endpoints);
				if (channel == null) {
					channel = new FollowChannel();
					channels.put(endpoints, channel);
				}
			}
		}
		return channel;
	}
	public static Channel newWServiceChannel(String endpoints) {
		Channel channel = channels.get(endpoints);
		if (channel == null) {
			synchronized (Channel.class) {
				channel = channels.get(endpoints);
				if (channel == null) {
					channel = new WServiceChannel();
					channels.put(endpoints, channel);
				}
			}
		}
		return channel;
	}

	public static Channel newMenuChannel(String endpoints) {
		Channel channel = channels.get(endpoints);
		if (channel == null) {
			synchronized (Channel.class) {
				channel = channels.get(endpoints);
				if (channel == null) {
					channel = new MenuChannel();
					channels.put(endpoints, channel);
				}
			}
		}
		return channel;
	}

	public static Channel newFootprintChannel(String endpoints) {
		Channel channel = channels.get(endpoints);
		if (channel == null) {
			synchronized (Channel.class) {
				channel = channels.get(endpoints);
				if (channel == null) {
					channel = new FootprintChannel();
					channels.put(endpoints, channel);
				}
			}
		}
		return channel;
	}

	
	public static Channel newImageMagickdChannel(String endpoints) {
		Channel channel = channels.get(endpoints);
		if (channel == null) {
			synchronized (Channel.class) {
				channel = channels.get(endpoints);
				if (channel == null) {
					channel = new ImageMagickdChannel();
					channels.put(endpoints, channel);
				}
			}
		}
		return channel;
	}

	public static Channel newLogChannel(String endpoints) {
		Channel channel = channels.get(endpoints);
		if (channel == null) {
			synchronized (Channel.class) {
				channel = channels.get(endpoints);
				if (channel == null) {
					channel = new LogChannel();
					channels.put(endpoints, channel);
				}
			}
		}
		return channel;
	}

	public static Channel newActiveTrackChannel(String endpoints) {
		Channel channel = channels.get(endpoints);
		if (channel == null) {
			synchronized (Channel.class) {
				channel = channels.get(endpoints);
				if (channel == null) {
					channel = new ActiveTrackChannel();
					channels.put(endpoints, channel);
				}
			}
		}
		return channel;
	}
	
	public static Channel newScoreChannel(String endpoints) {
		Channel channel = channels.get(endpoints);
		if (channel == null) {
			synchronized (Channel.class) {
				channel = channels.get(endpoints);
				if (channel == null) {
					channel = new ScoreChannel();
					channels.put(endpoints, channel);
				}
			}
		}
		return channel;
	}

    public static Channel newLikeChannel(String endpoints) {
		Channel channel = channels.get(endpoints);
		if (channel == null) {
			synchronized (Channel.class) {
				channel = channels.get(endpoints);
				if (channel == null) {
					channel = new LikeChannel();
					channels.put(endpoints, channel);
				}
			}
		}
		return channel;
	}

	public static Channel newBuddyChannel(String endpoints) {
                Channel channel = channels.get(endpoints);
                if (channel == null) {
                        synchronized (Channel.class) {
                                channel = channels.get(endpoints);
                                if (channel == null) {
                                        channel = new BuddyChannel();
                                        channels.put(endpoints, channel);
                                }
                        }
                }
                return channel;
        }	
	
    	public static Channel newGadgetsChannel(String endpoints) {
		Channel channel = channels.get(endpoints);
		if (channel == null) {
			synchronized (Channel.class) {
				channel = channels.get(endpoints);
				if (channel == null) {
					channel = new GadgetsChannel();
					channels.put(endpoints, channel);
				}
			}
		}
		return channel;
	}
    
    public static Channel newImporterChannel(String endpoints) {
		Channel channel = channels.get(endpoints);
		if (channel == null) {
			synchronized (Channel.class) {
				channel = channels.get(endpoints);
				if (channel == null) {
					channel = new ImporterChannel();
					channels.put(endpoints, channel);
				}
			}
		}
		return channel;
	}
    
    public static Channel newXwServiceSNSChannel(String endpoints) {
		Channel channel = channels.get(endpoints);
		if (channel == null) {
			synchronized (Channel.class) {
				channel = channels.get(endpoints);
				if (channel == null) {
					channel = new XwServiceSNSChannel();
					channels.put(endpoints, channel);
				}
			}
		}
		return channel;
	}
    
    public static Channel newXwServiceUGCChannel(String endpoints) {
		Channel channel = channels.get(endpoints);
		if (channel == null) {
			synchronized (Channel.class) {
				channel = channels.get(endpoints);
				if (channel == null) {
					channel = new XwServiceUGCChannel();
					channels.put(endpoints, channel);
				}
			}
		}
		return channel;
	}
    
    public static Channel newXwServiceAPIChannel(String endpoints) {
		Channel channel = channels.get(endpoints);
		if (channel == null) {
			synchronized (Channel.class) {
				channel = channels.get(endpoints);
				if (channel == null) {
					channel = new XwServiceAPIChannel();
					channels.put(endpoints, channel);
				}
			}
		}
		return channel;
	}

    public static Channel newXwServiceInviteChannel(String endpoints) {
		Channel channel = channels.get(endpoints);
		if (channel == null) {
			synchronized (Channel.class) {
				channel = channels.get(endpoints);
				if (channel == null) {
					channel = new XwServiceInviteChannel();
					channels.put(endpoints, channel);
				}
			}
		}
		return channel;
	}
    
    public static Channel newDataPlatformChannel(String endpoints) {
               Channel channel = channels.get(endpoints);
               if (channel == null) {
                       synchronized (Channel.class) {
                               channel = channels.get(endpoints);
                               if (channel == null) {
                                       channel = new DataPlatformChannel();
                                       channels.put(endpoints, channel);
                               }
                       }
               }
               return channel;
       }
    public static Channel newShareSearchChannel(String endpoints) {
        Channel channel = channels.get(endpoints);
        if (channel == null) {
                synchronized (Channel.class) {
                        channel = channels.get(endpoints);
                        if (channel == null) {
                                channel = new ShareSearchChannel();
                                channels.put(endpoints, channel);
                        }
                }
        }
        return channel;
}



	/** name 参数是为了提示必须把构造函数实现为protected */
	protected Channel(String name) {
		initDefault();
		initLocator();
		initOverride();
		Ice.InitializationData init = new Ice.InitializationData();
		init.properties = this.properties;
		this.communicator = Ice.Util.initialize(init);
		initPrintName(name);
	}

	/** name 参数是为了提示必须把构造函数实现为protected */
	protected Channel(String name, Properties properties) {
		Ice.InitializationData init = new Ice.InitializationData();
		init.properties = properties;
		this.communicator = Ice.Util.initialize(init);
		initPrintName(name);
	}

	protected void initDefault() {
		properties.setProperty("Ice.Override.ConnectTimeout", "70");
		properties.setProperty("Ice.ThreadPool.Client.SizeMax", "1000");
		properties.setProperty("Ice.ThreadPool.Client.StackSize", "131072");
		properties.setProperty("Ice.ThreadPool.Server.SizeMax", "1000");
		properties.setProperty("Ice.ThreadPool.Server.StackSize", "131072");
		properties.setProperty("Ice.MessageSizeMax", "102400");
		// properties.setProperty("Ice.Trace.Protocol", "3");
		// properties.setProperty("Ice.Trace.Network", "3");
	}

	private void initPrintName(String instanceName) {
		try {
			String hostname = IPAddress.getLocalHostName();
			System.out.println(instanceName + " found hostname -> " + hostname);
		} catch (Throwable e) {
		  System.err.println(this.getClass().getName() + " " + e);
		}
	}

	protected abstract void initLocator();

	protected void initOverride() {
		if (System.getProperty("__NO_TIMEOUT__") != null) {
			properties.setProperty("Ice.Override.Timeout", "-1");
			properties.setProperty("Ice.Override.ConnectTimeout", "-1");
		} else if (System.getProperty("__LONG_TIMEOUT__") != null) {
			properties.setProperty("Ice.Override.Timeout", "30000");
			properties.setProperty("Ice.Override.ConnectTimeout", "7000");
		}
	}

	@Override
	public void finalize() {
		communicator.destroy();
	}

	public Properties getProperties() {
		return communicator.getProperties();
	}

	@SuppressWarnings("unchecked")
	public <T> T locate(String name, String endpoints, Invoke iv, Class helper,
			int timeout) {
		switch (iv) {
		case Oneway:
			return (T) uncheckedCast(communicator.stringToProxy(
					name + endpoints).ice_timeout(timeout).ice_oneway(), helper);
		case Twoway:
			return (T) uncheckedCast(communicator.stringToProxy(
					name + endpoints).ice_timeout(timeout), helper);
		case Udp:
			return (T) uncheckedCast(communicator.stringToProxy(
					name + endpoints).ice_timeout(timeout).ice_datagram(),
					helper);
		default:
			return null;
		}
	}

	@SuppressWarnings("unchecked")
	public <T> T uncheckedCast(Ice.ObjectPrx __obj, Class helper) {
		T __d = null;
		if (__obj != null) {
			try {
				Ice.ObjectPrxHelperBase __h = (Ice.ObjectPrxHelperBase) helper
						.newInstance();
				__h.__copyFrom(__obj);
				__d = (T) __h;
			} catch (Exception e) {

			}
		}
		return __d;
	}

	public Communicator getCommunicator() {
		return communicator;
	}
}
