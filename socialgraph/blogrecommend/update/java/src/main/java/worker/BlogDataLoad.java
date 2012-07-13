package worker;

import blog.CommonLoadBlog;
import blog.LoadBlog;
import blog.UpdateTypeBlog;

public class BlogDataLoad {
	private static BlogDataLoad instance = null;
	
	public static BlogDataLoad getInstance() {
		if (null == instance) {
			synchronized (BlogDataLoad.class) {
				if (null == instance) {
					try {
						instance = new BlogDataLoad();
					} catch (Exception e) {
						e.printStackTrace();
						instance = null;
					}
				}
			}
		}
		return instance;
	}
	
	private BlogDataLoad() {
	}
	
	/**
	 * 向distcache中加载数据
	 * @param dest 源文件
	 */
	public void loadData(String dest) {
		System.out.println("[BlogDataLoad] loadData dest: " + dest);
		//加载blog数据信息
		LoadBlog loadBlog = new CommonLoadBlog();
		loadBlog.load(dest);
		
		//增量更新加载blog tag信息
		LoadBlog loadType = new UpdateTypeBlog();
		loadType.load(dest);
	}
	
	public static void main(String[] args) {
		System.out.println(args.length);
		if (1 != args.length) {
			System.err.println("error input args!!");
			return;
		}
		BlogDataLoad.getInstance().loadData(args[0]);
		System.exit(0);
	}
}
