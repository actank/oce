package xce.tools.objectcache;

import java.util.concurrent.BlockingQueue;
import java.util.concurrent.LinkedBlockingQueue;

public class ObjectCacheShell {
	public static void main(String[] args) {
		if (args.length < 1) {
			printUsage();
			System.exit(0);
		}
		System.out.println("arglength:"+args.length);
		try {
			BlockingQueue<Long[]> queue = new LinkedBlockingQueue<Long[]>();
			Consumer consumer = null;
			if ("ReloadFriendDist".equalsIgnoreCase(args[0])) {
				Producer producer = new UserIdAllAction(queue, 10000);
				consumer = new ReloadOceAction(producer, "FriendDist",
						new int[] { 0 }, 10);
			} else if ("RebuildAllFriendDist".equalsIgnoreCase(args[0])) {
				Producer producer = new UserIdAllAction(queue, 10000);
				consumer = new InvokeFriendDistAction(producer);
			} else if ("RemoveFeedNewsSwap".equalsIgnoreCase(args[0])) {
				Producer producer = new UserIdAllAction(queue, 10000);
				consumer = new RemoveFeedAction(producer, "XFeedNews",
						new int[] { 1 }, 100);
			}else if ("RemoveFeedNewsRecent".equalsIgnoreCase(args[0])) {
						Producer producer = new UserIdTopNAction(queue, 1000000);
						consumer = new RemoveFeedAction(producer, "XFeedNews",
								new int[] { 0 }, 100);
			} else if ("PreloadOneFriendDist".equalsIgnoreCase(args[0])) {
				Producer producer = new SpecificIdsAction(queue, args);
				consumer = new PreloadOceAction(producer, "FriendDist",
						new int[] { 0 }, 10);
			} else if ("ReloadAccount".equalsIgnoreCase(args[0])) {
				Producer producer = new SpecificIdsAction(queue, args);
				consumer = new RemoveOceAction(producer, "Account", new int[]{0},0);				
			} else if ("PreloadUserCount".equalsIgnoreCase(args[0])){
				Producer producer = new UserIdAllAction(queue,10000);
				consumer = new PreloadOceAction(producer,"UserCount",
						new int[]{0},10);
			} else if ("PreloadUserConfig".equalsIgnoreCase(args[0])){
				Producer producer=new UserIdAllAction(queue,10000);
				consumer = new PreloadOceAction(producer,"UserConfigN",
						new int[]{0},100);
			} else if ("PreloadUserState".equalsIgnoreCase(args[0])){
				Producer producer=new UserIdAllAction(queue,10000);
				consumer=new PreloadOceAction(producer,"UserState",new int[]{0},10);
			} else if ("PreloadUserNetwork".equalsIgnoreCase(args[0])){
				Producer producer=new UserIdAllAction(queue,10000);
				consumer=new PreloadOceAction(producer,"UserNetworkN",new int[]{0,1,2,3,4,5,6},10);
			} else if ("PreloadFootprint".equalsIgnoreCase(args[0])){
				Producer producer=new UserIdAllAction(queue,10000);
				consumer=new PreloadOceAction(producer,"Footprint",new int[]{0},10);
			} else if ("PreloadBlogFootprint".equalsIgnoreCase(args[0])){
				Producer producer=new UserIdAllAction(queue,10000);
				consumer=new PreloadOceAction(producer,"BlogFootprint",new int[]{0},1);
			} else if ("PreloadBuddyCore".equalsIgnoreCase(args[0])){
				Producer producer=new UserIdAllAction(queue,10000);
				consumer=new PreloadOceAction(producer,"BuddyCore",new int[]{0},10);
			} else if ("PreloadDbCacheRequestConfig".equalsIgnoreCase(args[0])){
				Producer producer=new UserIdAllAction(queue,10000);
				consumer=new PreloadOceAction(producer,"DbCacheRequestConfig",new int[]{0},1);
			} else if ("PreloadDbCacheNofityConfig".equalsIgnoreCase(args[0])){
				Producer producer=new UserIdAllAction(queue,10000);
				consumer=new PreloadOceAction(producer,"DbCacheNotifyConfig",new int[]{0},1);
			} else if ("PreloadNotifyRpl".equalsIgnoreCase(args[0])){
				Producer producer=new UserIdAllAction(queue,10000);
				consumer=new PreloadOceAction(producer,"NotifyRpl",new int[]{0},10);
			} else if ("PreloadUser".equalsIgnoreCase(args[0])){
				Producer producer=new UserIdAllAction(queue,10000);
				consumer=new PreloadOceAction(producer,"UserN",new int[]{0,1,2,3,5,6,7,8,9,10},100);
			} else if ("PreloadBuddyApplyCache".equalsIgnoreCase(args[0])){
				Producer producer=new UserIdAllAction(queue,10000);
				consumer=new PreloadOceAction(producer,"BuddyApplyCache",new int[]{0},10);
			} else if ("PreloadXFeedDispatcher".equalsIgnoreCase(args[0])){
				Producer producer=new XFeedIdAllAction(queue,10000);
				consumer=new PreloadXFeed(producer,10);
			} else if ("PreloadOfferFriends".equalsIgnoreCase(args[0])){
				Producer producer=new UserIdAllAction(queue,10000);
				consumer=new PreloadOceAction(producer,"OfferFriends",new int[]{0,1,2},10);
			} else if ("PreloadFriendFinder".equalsIgnoreCase(args[0])){
				Producer producer=new UserIdAllAction(queue,10000);
				consumer=new PreloadOceAction(producer,"FriendFinder",new int[]{0},10);
			} else if ("PreloadRecentlyOnlineBuddy".equalsIgnoreCase(args[0])){
				Producer producer=new UserIdAllAction(queue,10000);
				consumer=new PreloadOceAction(producer,"RecentlyOnlineBuddy",new int[]{0},10);
			} else if ("PreloadClassApplyCounter".equalsIgnoreCase(args[0])){
				Producer producer =new UserIdAllAction(queue,10000);
				consumer=new PreloadOceAction(producer,"ClassApplyCounter",new int[]{0,1},0);
			}
			else {
				System.out.println("Unknown type " + args[0]);
			}
			consumer.waitFinished();
		} catch (Exception e) {
			e.printStackTrace();
		}
		System.exit(0);
	}

	public static void printUsage() {
		System.out.println("Usage: Reload <SvcName>");

	}
}
