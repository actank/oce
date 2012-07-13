package common;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.IOException;
/**
 * 用户特征数据加载抽象基类，新算法数据源的加载需要实现其load接口
 * @author zhangnan
 *
 */
public abstract class BasicFeatureLoader {
	private BufferedReader reader = null;
	/**
	 * 初始化生成数据文件加载的reader
	 * @param path 数据文件路径
	 */
	public BasicFeatureLoader(String path) {
		try {
			reader = new BufferedReader(new FileReader(new File(path)));
		} catch (FileNotFoundException e) {
			e.printStackTrace();
		}
	}
	
	/**
	 * 逐行获取数据文件
	 * @return
	 */
	public String getLine() {
		if (null != reader) {
			try {
				return reader.readLine();
			} catch (IOException e) {
				e.printStackTrace();
				return null;
			}
		}
		return null;
	}
	/**
	 * 数据加载抽象接口
	 */
	public abstract void load();
}
