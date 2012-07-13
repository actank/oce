package worker;

import blog.BlogRecommendLoaderByRule;
import blog.BlogResourceDataLoader;
import blog.LoadBlog;

/**
 * 依据用户行为进行日志推荐的数据加载类
 * @author zhangnan
 *
 */
public class BlogDataLoadByUserActive {
	private static BlogDataLoadByUserActive instance = null;
	
	public static BlogDataLoadByUserActive getInstance() {
		if (null == instance) {
			synchronized (BlogDataLoadByUserActive.class) {
				if (null == instance) {
					try {
						instance = new BlogDataLoadByUserActive();
					} catch (Exception e) {
						// TODO: handle exception
						e.printStackTrace();
						instance = null;
					}
				}
			}
		}
		return instance;
	}
	
	private BlogDataLoadByUserActive() {
	}

	/**
	 * 向distcache中加载数据
	 * @param blogFile 原始数据
	 * @param ruleFile 规则数据
	 */
	public void loadData(String blogFile, String ruleFile) {
		System.out.println("[BlogDataLoadByUserActive] loadData");
		System.out.println("blogFile : " + blogFile);
		System.out.println("ruleFile : " + ruleFile);
		
		//加载blog原始数据文件
		BlogResourceDataLoader.getInstance().load(blogFile);
		
		BlogResourceDataLoader.getInstance().PrintMap();
		
		//按照规则文件加载推荐数据
		LoadBlog loadRecommend = new BlogRecommendLoaderByRule();
		loadRecommend.load(ruleFile);
	}
	
	public static void main(String[] args) {
		System.out.println(args.length);
		if (2 != args.length) {
			System.err.println("error input args!!");
			return;
		}
		
		BlogDataLoadByUserActive.getInstance().loadData(args[0], args[1]);
		System.exit(0);
	}
}
