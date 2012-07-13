package blog;

import java.io.BufferedReader;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.io.InputStreamReader;
import java.util.HashMap;
import java.util.Map;

import worker.BlogDataLoadByUserActive;
import common.Blog;

/**
 * 业务按照用户行为推荐日志，该类主要负责将日志的原始文件加载到内存中，以为规则文件的加载提供原始数据
 * 文件格式 blog_id share_id share_user_id share_time view_count owner_id title body
 * @author zhangnan
 *
 */
public class BlogResourceDataLoader implements LoadBlog {
	private static BlogResourceDataLoader instance = null;
	private Map<Long, Blog> blogDataMap = null;
	
	//设计成单体是为了方便其他程序使用
	public static BlogResourceDataLoader getInstance() {
		if (null == instance) {
			synchronized (BlogDataLoadByUserActive.class) {
				if (null == instance) {
					try {
						instance = new BlogResourceDataLoader();
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
	
	public BlogResourceDataLoader() {
		blogDataMap = new HashMap<Long, Blog>();
	}
	
	public void load(String file) {
		System.out.println("[BlogResourceDataLoader] load file:" + file);
		final int itemCount = 8; 	//原始文件的列数
		String line = "";
		try {
			BufferedReader reader = new BufferedReader(new InputStreamReader(new FileInputStream(file)));
			line = reader.readLine();
			while (null != line) {
				String[] itemList = line.split("");
				if (itemCount != itemList.length) {
					System.out.println("itemList length error");
					line = reader.readLine();		//有异常的行就忽略不计
					continue;
				}
				
				long blogId = Long.valueOf(itemList[0]);
				
				Blog item = new Blog();
				item.shareId = Long.valueOf(itemList[1].trim());		//分享id
				item.blogId = Long.valueOf(itemList[0].trim());		//blogId
				item.hostId = Integer.valueOf(itemList[5].trim());		//该blog的拥有者id
				item.time = itemList[3].trim();											//blog的被分享时间
				item.count = Integer.valueOf(itemList[4].trim());		//blog被分享的次数
				item.firstShareId = Integer.valueOf(itemList[2].trim());		//第一次分享的用户id
				item.title = itemList[6].trim();													//blog的title
				item.content = itemList[7].trim();											//blog的内容
				
				blogDataMap.put(blogId, item);		//插入本地map中
				
				line = reader.readLine();
			}
		} catch (FileNotFoundException e) {
			e.printStackTrace();
		} catch (IOException e) {
			e.printStackTrace();
		}
	}

	/**
	 * 依据blogid获取对应的blog信息
	 * @param blogid 日志id
	 * @return 返回对应的blog信息
	 */
	public Blog getBlogData(Long blogid) {
		return blogDataMap.get(blogid);
	}
	
	/**
	 * 测试用，看map是否加载成功
	 */
	public void PrintMap() {
		System.out.println("map size : " + blogDataMap.size());
//		for (Entry<Long, Blog> item : blogDataMap.entrySet()) {
//			System.out.println(item.getKey() + " : ");
//			System.out.println(item.getValue().blogId + ":" + item.getValue().count
//					+ ":" + item.getValue().firstShareId + ":" + item.getValue().title
//					+ ":" + item.getValue().content + ":" + item.getValue().shareId + ":"
//					+ item.getValue().hostId);
//		}
	}
}
