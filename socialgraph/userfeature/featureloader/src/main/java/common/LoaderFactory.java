package common;

import loader.CharacterLoader;
import loader.FavorLoader;

/**
 * 根据不同算法，创建具体的加载数据类，每次添加算法都要更新这个类
 * @author zhangnan
 *
 */
public class LoaderFactory {
	static public BasicFeatureLoader createLoader(String type, String path) {
		if (type.equals("FAVOR")) {
			return new FavorLoader(path);
		} else if (type.equals("CHARACTER")) {
			return new CharacterLoader(path);
		}
		return null;
	}
}
