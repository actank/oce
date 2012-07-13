package blog;

import java.io.BufferedReader;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.io.InputStreamReader;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

import com.xiaonei.xce.socialgraph.proto.GeneratedItems.Item;
import com.xiaonei.xce.socialgraph.proto.GeneratedItems.Items;

import common.Blog;
import common.MemcachedClient;

/**
 * 按照用户行为推荐日志类
 * @author zhangnan
 *
 */
public class BlogRecommendLoaderByRule implements LoadBlog {
	private static final int LIMIT = 10;		//推荐日志的数量限制为10个
	private static String BLOGACTION = "BLOGACTION";		//type
	
	/**
	 * 重载接口用来处理根据用户行为推荐的数据文件
	 */
	public void load(String file) {
		System.out.println("[BlogRecommendLoaderByRule] load file:" + file);
		
		String line = "";
		Pattern pattern = Pattern.compile("b(\\d+)u(\\d+)");
		try {
			BufferedReader reader = new BufferedReader(new InputStreamReader(new FileInputStream(file)));
			line = reader.readLine();
			while (null != line) {
				String[] blockList = line.split("	");
				if (2 != blockList.length) {
					line = reader.readLine();
					continue;
				}
				
				int userId = Integer.valueOf(blockList[0].trim());		//获取用户id
				
				Items.Builder result = Items.newBuilder();
				
				String[] blogItemList = blockList[1].split(";");		//日志之间的分隔符为“;”，切分出每个推荐日志
				for (String it : blogItemList) {				//遍历该用户的每个推荐日志
					String[] itWeightList = it.split(",");		//格式为b725065032u221163026,2500.693325，区分“，”
					if (2 != itWeightList.length) {		
						continue;
					}
					
					Matcher matcher = pattern.matcher(itWeightList[0]);			//b725065032u221163026，blogId为725065032 - group 1，ownerId为221163026 - group 2
					if (matcher.find()) {
						Long blogId = Long.valueOf(matcher.group(1)); 
						
						Blog blogData = BlogResourceDataLoader.getInstance().getBlogData(blogId);
						
						if (null != blogData 							// blogData应该存在于内存，也就是被分享过，有资格被加入cache中
								&& result.getItemsCount() <= LIMIT) { 					// 日志推荐的数量小于LIMIT
							String message = "";
							message = String.valueOf(blogData.shareId) + ":"
									+ String.valueOf(blogData.blogId); // 多个属性拼接成的字符串 share_id:blog_id
							
							Item item = Item.newBuilder().setId(blogData.hostId)					//该blog的作者id
									.setMessage(message).addFields(blogData.firstShareId)			//fields 存储分享人的id, count
									.addFields(blogData.count).addMessages(blogData.time)			//messages 存储 time, title, body
									.addMessages(blogData.title).addMessages(blogData.content)
									.build();
//							System.out.println(message + " - " + blogData.hostId + " - "
//									+ blogData.firstShareId + " - " + blogData.time + " - "
//									+ blogData.title + " - " + blogData.content);
							result.addItems(item);
						}
					}
				}
				
				byte[] binaryData = result.build().toByteArray();
				
				if (0 != binaryData.length) {
					boolean suc = MemcachedClient.getInstance().setByDistCache(BLOGACTION, userId, binaryData);
					if (suc) {
						byte[] data = MemcachedClient.getInstance().getByDistCache(BLOGACTION, userId);
						System.out.println("BLOGACTION insert success id:" + userId + " data size:" + data.length);
					} else {
						System.out.println("BLOGACTION insert fail id:" + userId);
					}
				}
				
				line = reader.readLine();
			}
		} catch (FileNotFoundException e) {
			e.printStackTrace();
		} catch (IOException e) {
			e.printStackTrace();
		}
	}
}
