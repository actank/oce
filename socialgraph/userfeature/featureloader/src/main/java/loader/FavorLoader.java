package loader;

import xce.socialgraph.util.Clogging;

import com.google.protobuf.InvalidProtocolBufferException;
import com.xiaonei.xce.socialgraph.BusinessCacheAdapter;
import com.xiaonei.xce.socialgraph.proto.GeneratedItems.Item;
import com.xiaonei.xce.socialgraph.proto.GeneratedItems.Items;

import common.BasicFeatureLoader;
/**
 * 加载用户对右侧推荐位的偏好数据文件
 * @author zhangnan
 *
 */
public class FavorLoader extends BasicFeatureLoader {
	static public String prefix = "USERFEATURE";		//distcache中的前缀
	static public String type = "FAVOR";		//数据类型
	static public int length = 6;			//数据文件解析后的数组长度
	
	public FavorLoader(String path) {
		super(path);
	}

	/**
	 * 实现抽象接口，加载用户对右侧推荐位的偏好属性
	 */
	@Override
	public void load() {
		Clogging.Info("Begin to FavorLoader!");
		String line = null;
		while ((line = getLine()) != null) {
			String[] strList = line.split("	");					//数据格式为： 726410003		1	 女生	30	0	 0 
			if (length != strList.length) {
				continue;
			}
			int userId = Integer.valueOf(strList[0]);		//取出用户id
			int category = Integer.valueOf(strList[5]);		//取出用户分类数据
			
			byte[] data = BusinessCacheAdapter.getInstance().get(prefix, userId);
			
			if (null == data) {		//cache中没有数据，直接插入
				insert(userId, category);
				
				byte[] insert_result = BusinessCacheAdapter.getInstance().get(prefix, userId);
				Clogging.Debug("insert " + userId + " success insert_result size:"
						+ insert_result.length);
			} else {		 //cache中有数据，需要更新老数据
				update(userId, category, data);
				
				byte[] update_result = BusinessCacheAdapter.getInstance().get(prefix, userId);
				Clogging.Debug("update " + userId + " success update_result size:"
						+ update_result.length);
			}
		}
	}
	
	/**
	 * 插入操作
	 * @param userId
	 * @param category
	 */
	private void insert(int userId, int category) {
		Item.Builder favorData = Item.newBuilder();
		favorData.setId(userId);
		favorData.setField(category);
		favorData.setMessage(type);
		
		Items.Builder featureData = Items.newBuilder();
		featureData.addItems(favorData.build());
		BusinessCacheAdapter.getInstance().set(prefix, userId, featureData.build().toByteArray());
	}
	
	/**
	 * 更新操作
	 * @param userId
	 * @param category
	 * @param data
	 */
	private void update(int userId, int category, byte[] data) {
		try {
			Items oldFavorData = Items.parseFrom(data);
			Items.Builder newFavorData = Items.newBuilder();
			for (Item each : oldFavorData.getItemsList()) {
				if (each.getMessage().equals(type)) {
					Item.Builder item = Item.newBuilder();
					item.setId(userId);
					item.setField(category);
					item.setMessage(type);
					newFavorData.addItems(item);
				} else {
					newFavorData.addItems(each);
				}
			}
			BusinessCacheAdapter.getInstance().set(prefix, userId, newFavorData.build().toByteArray());
		} catch (InvalidProtocolBufferException e) {
			e.printStackTrace();
		}
	}
	
	@SuppressWarnings("unused")
	private void PrintItems(byte[] data) {
		if (null == data) {
			Clogging.Error("PrintItems data is null!");
			return;
		}
		Clogging.Info("PrintItems data length:" + data.length);
		try {
			Items result = Items.parseFrom(data);
			for (Item each : result.getItemsList()) {
				Clogging.Debug(String.valueOf(each.getId()) + ":" + each.getMessage()
						+ ":" + String.valueOf(each.getField()));
			}
		} catch (InvalidProtocolBufferException e) {
			e.printStackTrace();
		}
	}
}
