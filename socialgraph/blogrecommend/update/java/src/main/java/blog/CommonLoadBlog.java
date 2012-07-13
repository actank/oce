package blog;

import java.io.BufferedReader;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.io.InputStreamReader;

import com.xiaonei.xce.socialgraph.proto.GeneratedItems.Item;
import com.xiaonei.xce.socialgraph.proto.GeneratedItems.Items;
import common.MemcachedClient;

/**
 * 业务按blog相关度推荐日志，日志本身数据向distcache加载类
 * @author zhangnan
 *
 */
public class CommonLoadBlog implements LoadBlog {
	private static String BLOG = "BLOG";
	public void load(String file) {
		final int itemCount = 9;
		System.out.println("[CommonLoadBlog] load file(" + file + ")");
		String line = "";
		try {
			BufferedReader reader = new BufferedReader(new InputStreamReader(new FileInputStream(file)));
			line = reader.readLine();
			while (null != line) {
				String[] itemList = line.split("");
				if (itemCount != itemList.length) {
					System.out.println("itemList length error");
					continue;
				}
				
				long blogId = Long.valueOf(itemList[0]);
				String detail = itemList[8];
				
				Items.Builder builder = Items.newBuilder();
				
				String refineDetail = detail.trim();
				String[] typeList = refineDetail.split(";");
				for (String type : typeList) {
					String[] weightList = type.split(",");
					if (2 != weightList.length) {
						continue;
					}
					Item item = Item.newBuilder().setId(Integer.valueOf(weightList[0]))
							.setMessage(weightList[1]).build();
					builder.addItems(item);
				}
				byte[] binaryData = builder.build().toByteArray();
				if (0 != binaryData.length) {
					boolean suc = MemcachedClient.getInstance().set2ByDistCache(BLOG, blogId, binaryData);
					if (suc) {
						byte[] tmpData = MemcachedClient.getInstance().get2ByDistCache(BLOG, blogId);
						System.out.println("insert success tmpData size : " + tmpData.length + " shareId:" + blogId);
					} else {
						System.out.println("insert fail");
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
