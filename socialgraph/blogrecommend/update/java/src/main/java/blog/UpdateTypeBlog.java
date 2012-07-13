package blog;

import java.io.BufferedReader;
import java.io.FileInputStream;
import java.io.InputStreamReader;
import java.util.ArrayList;
import java.util.Collections;
import java.util.HashMap;
import java.util.List;
import java.util.Map.Entry;

import com.google.protobuf.InvalidProtocolBufferException;
import com.xiaonei.xce.socialgraph.proto.GeneratedItems.Item;
import com.xiaonei.xce.socialgraph.proto.GeneratedItems.Items;

import common.Blog;
import common.BlogCompare;
import common.MemcachedClient;

/**
 * 增量更新代码，更新cache数据时既考虑已有文件数据，又考虑distcahe中已有数据
 * @author zhangnan
 *
 */
public class UpdateTypeBlog implements LoadBlog {
	private HashMap<Integer, BlogData> tagBlogMap = new HashMap<Integer, BlogData>(); 
	private final int itemCount = 9;
	private final int saveSize = 10;	//推荐数量
	private static String BLOGTAG = "BLOGTAG";
	
	public void load(String file) {
		System.out.println("[UpdateTypeBlog] load file: " + file);
		
		//从文件中加载
		loadFromFile(file);
		
		//加载原始memcache中的数据
		loadFromOriginalCache();
		
		//处理tagMap中的数据，应用一些处理策略
		processTagMap();
			
		//向distcache中插入数据
		setCache();		
	}
	
	/**
	 * 从文件中加载数据进入本地map
	 * @param file
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
//					System.out.println(blog.shareId + "-" + blog.blogId + "-" + blog.hostId
//							+ "-" + blog.time + "-" + blog.count + "-" + blog.firstShareId
//							+ "-" + blog.title + "-" + blog.content + "-" + blog.weightStr);
					
					int category = Integer.valueOf(weightList[0]);
					float weight = Float.valueOf(weightList[1]);
					blog.weight = weight;
					
					BlogData data = tagBlogMap.get(category);		//向map中插入数据
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
	 * 从distcache中加载原始数据，以完成增量更新
	 */
	private void loadFromOriginalCache() {
		System.out.println("tagBlogMap size : " + tagBlogMap.size());
		for (Entry<Integer, BlogData> eachEntry : tagBlogMap.entrySet()) {
			byte[] data = MemcachedClient.getInstance().getByDistCache(BLOGTAG, eachEntry.getKey());
			if (null == data) {		//如果distcache中没有该类型数据，则继续;如果有数据，就将数据添加到本地map中
				continue;
			}
			try {
				Items originalItems = Items.parseFrom(data);
				for (Item eachItem : originalItems.getItemsList()) {
					Blog blog = new Blog();
					blog.hostId = eachItem.getId();
					String[] messageSlice = eachItem.getMessage().split(":"); 
					if (messageSlice.length != 3) {		//如果计数错误，继续循环
						continue;
					} else {
						blog.shareId = Long.valueOf(messageSlice[0]);			//message第一个段存shareId
						blog.blogId = Long.valueOf(messageSlice[1]);			//message第二个段存blogId
						blog.weight = Float.valueOf(messageSlice[2]); 			//message第三个段存weight
					}
					
					if (eachItem.getFieldsList().size() != 2) {		//如果计数错误，继续循环
						continue;
					} else {
						blog.firstShareId = eachItem.getFieldsList().get(0);		//序列第一个存储firstShareId
						blog.count = eachItem.getFieldsList().get(1);			//序列第二个存储count
					}
					
					if (eachItem.getMessagesList().size() != 4) {			//如果计数错误，继续循环
						continue;
					} else {
						blog.time = eachItem.getMessagesList().get(0);		//序列第一个存储 time
						blog.title = eachItem.getMessagesList().get(1); 		//序列第二个存储 title
						blog.content = eachItem.getMessagesList().get(2);		//序列第三个存储 content
						blog.weightStr = eachItem.getMessagesList().get(3);		//序列第三个存储 weightStr
					}
					
					BlogData tagData = tagBlogMap.get(eachEntry.getKey());		//向map中插入数据
					if (null == tagData) {
						tagData = new BlogData();
						tagData.addBlog(blog);
						tagBlogMap.put(eachEntry.getKey(), tagData);
					} else {
						tagData.addBlog(blog);
					}
				}
			} catch (InvalidProtocolBufferException e) {
				e.printStackTrace();
			}
		}
	}
	
	/**
	 * 处理tagBlogMap中的数据，包括一些处理策略
	 */
	private void processTagMap() {
		System.out.println("[UpdateTypeBlog] processTagMap tagBlogMap size:" + tagBlogMap.size());
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
	 * 用tagBlogMap中的数据加载distcache
	 */
	private void setCache() {
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
