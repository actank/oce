package blog;

import java.io.BufferedReader;
import java.io.FileInputStream;
import java.io.InputStreamReader;
import java.util.ArrayList;
import java.util.Collections;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.Map.Entry;

import com.xiaonei.xce.socialgraph.proto.GeneratedItems.Item;
import com.xiaonei.xce.socialgraph.proto.GeneratedItems.Items;

import common.Blog;
import common.BlogCompare;
import common.MemcachedClient;

/**
 * 旧的加载数据代码，用于以新文件中的数据全部替换distcache中的数据
 * @author zhangnan
 *
 */
public class TagsLoadBlog implements LoadBlog {
	private Map<Integer, BlogData> tagBlogMap = new HashMap<Integer, TagsLoadBlog.BlogData>(); 
	private final int itemCount = 9;
	private final int saveSize = 10;	//推荐数量
	private static String BLOGTAG = "BLOGTAG";
	
	public void load(String file) {
		System.out.println("[TagsLoadBlog] load file(" + file + ")");
		
		//从文件中加载数据
		loadFromFile(file);
		
		//排序map中的blog序列
		processTagMap();
		
		//向distcache中插入数据
		setCache();		
	}
	
	/**
	 * 从文件中加载数据
	 * @param file 文件地址
	 */
	private void loadFromFile(String file) {
		String line = "";
		try {
			BufferedReader reader = new BufferedReader(new InputStreamReader(new FileInputStream(file)));
			line = reader.readLine();
			while (null != line) {
				String[] itemList = line.split("");
				if (itemCount != itemList.length) {
					System.err.println("itemList length error");
				}

				String[] typeList = itemList[8].trim().split(";");
				for (String type : typeList) {
					String[] weightList = type.split(",");
					if (2 != weightList.length) {
						continue;
					}
					Blog blog = new Blog();
					blog.blogId = Long.valueOf(itemList[0].trim());
					blog.shareId = Long.valueOf(itemList[1].trim());
					blog.firstShareId = Integer.valueOf(itemList[2].trim());
					blog.time = itemList[3];
					blog.count = Integer.valueOf(itemList[4].trim());
					blog.hostId = Integer.valueOf(itemList[5].trim());
					blog.title = itemList[6].trim();
					blog.content = itemList[7].trim();
					blog.weightStr = itemList[8].trim();
					
					int category = Integer.valueOf(weightList[0]);
					float weight = Float.valueOf(weightList[1]);
					blog.weight = weight;
					
					BlogData data = tagBlogMap.get(category);
					if (null == data) {
						data = new BlogData();
						data.addBlog(blog);
						tagBlogMap.put(category, data);
					} else {
						data.addBlog(blog);
					}
				}
				line = reader.readLine();
			}
		} catch (Exception e) {
			e.printStackTrace();
		}
	}
	
	/**
	 * 处理tagBlogMap中的数据，包括一些处理策略
	 */
	private void processTagMap() {
		System.out.println("[TagsLoadBlog] processTagMap tagBlogMap size:" + tagBlogMap.size());
		for (Entry<Integer, BlogData> entry : tagBlogMap.entrySet()) {
			Collections.sort(entry.getValue().getBlog(), new BlogCompare());		//blog推荐按照type相关度排序
			
			float check = 0;
			List<Blog> subList = new ArrayList<Blog>();
			for (Blog bb : entry.getValue().getBlog()) {
				if (check == bb.weight) {			//连续两个相关度相同的日志，认为是相同的日志，只推荐其中一个，避免重复
					continue;
				}
				if (subList.size() > saveSize) {		//限制推荐个数
					break;
				} else {
					check = bb.weight;
					subList.add(bb);
				}
			}
			entry.getValue().changeBlogList(subList);
		}
	}
	
	/**
	 * 向distcache中插入数据
	 */
	private void setCache() {
		System.out.println("tagBlogMap size : " + tagBlogMap.size());
		for (Entry<Integer, BlogData> eachEntry : tagBlogMap.entrySet()) {
			Items.Builder result = Items.newBuilder();
			for (Blog blog : eachEntry.getValue().getBlog()) {
				String message = "";
				message = String.valueOf(blog.shareId) + ":"
						+ String.valueOf(blog.blogId) + ":" + String.valueOf(blog.weight);
				Item item = Item.newBuilder().setId(blog.hostId).setMessage(message)
						.addFields(blog.firstShareId).addFields(blog.count)
						.addMessages(blog.time).addMessages(blog.title)
						.addMessages(blog.content).addMessages(blog.weightStr).build();
				result.addItems(item);
				System.out.println(blog.hostId + " " + message + " "
						+ blog.firstShareId + " " + blog.count + " " + blog.title + " "
						+ blog.content + " " + blog.weightStr);  		//输出check信息
			}
			
			byte[] binaryData = result.build().toByteArray();
			if (0 != binaryData.length) {
				boolean suc = MemcachedClient.getInstance().setByDistCache(BLOGTAG, eachEntry.getKey(), binaryData);
				if (suc) {
					byte[] data = MemcachedClient.getInstance().getByDistCache(BLOGTAG, eachEntry.getKey());
					System.out.println("BLOGTAG insert success id:" + eachEntry.getKey() + " data size:" + data.length);
				} else {
					System.out.println("BLOGTAG insert fail id:" + eachEntry.getKey());
				}
			}
		}
	}
	
	/**
	 * 打印tagBlogMap
	 */
	private void printTagBlogMap() {
		for (Entry<Integer, BlogData> entry : tagBlogMap.entrySet()) {
			System.out.println(entry.getKey() + " : ");
			for (Blog blog : entry.getValue().getBlog()) {
				System.out.println(blog.shareId + "-" + blog.blogId + "-" + blog.hostId
						+ "-" + blog.time + "-" + blog.count + "-" + blog.firstShareId
						+ "-" + blog.title + "-" + blog.content + "-" + blog.weightStr
						+ "-" + blog.weight);
			}
		}
	}
	
	/**
	 * 内部类，用于存储加载的blog列表 
	 */
	private class BlogData {
		private List<Blog> blogList = new ArrayList<Blog>();
		
		public BlogData() {
			// TODO Auto-generated constructor stub
		}
		
		public void addBlog(Blog blog) {
			blogList.add(blog);
		}
		
		public List<Blog> getBlog() {
			return blogList;
		}
		
		public void changeBlogList(List<Blog> newList) {
			blogList = newList;
		}
	}
}
