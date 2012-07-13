package loader;

import xce.socialgraph.util.Clogging;
import common.BasicFeatureLoader;
/**
 * 添加用户特征数据源类，用户对不用类型好友的喜好程度
 * @author zhangnan
 *
 */
public class CharacterLoader extends BasicFeatureLoader {
	public CharacterLoader(String path) {
		super(path);
	}

	/**
	 * 加载数据接口
	 */
	@Override
	public void load() {
		Clogging.Info("Begin to CharacterLoader!");
	}
}
