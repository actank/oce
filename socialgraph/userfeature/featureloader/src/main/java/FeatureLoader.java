import java.io.BufferedReader;
import java.io.File;
import java.io.FileReader;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.Map.Entry;

import org.json.JSONArray;
import org.json.JSONException;

import xce.socialgraph.util.Clogging;

import common.BasicFeatureLoader;
import common.LoaderFactory;

/**
 * 用户特征数据加载入口
 * @author zhangnan
 *
 */
public class FeatureLoader {
	/**
	 * 从配置文件获取配置信息
	 * @param file_path	配置文件的路径
	 * @return
	 */
	static public Map<String, String> getTypeFileMap(String file_path) {
		Map<String, String> typeFileMap = new HashMap<String, String>();
		String content = getContentFromFile(file_path);
		if (!content.isEmpty()) {
			try {
				JSONArray jo = new JSONArray(content);
				for (int i = 0; i < jo.length(); ++i) {
					String type = jo.getJSONObject(i).getString("type");
					String path = jo.getJSONObject(i).getString("file_path");
					File tmp_file = new File(path);
					if (!tmp_file.exists()) {		//如果不存在，则退出
						Clogging.Error("getTypeFileMap load file:" + path + " doesn't exist!!");
						System.exit(0);
					}
					typeFileMap.put(type, path);
				}
			} catch (JSONException e) {
				e.printStackTrace();
				return typeFileMap;
			}
		}
		return typeFileMap;
	}
	
	/**
	 * 获取配置文件内容
	 * @param path 配置文件路径
	 * @return
	 */
	static public String getContentFromFile(String path) {
		String laststr = "";
		File file = new File(path);
		if (!file.exists()) {
			Clogging.Error(path + " doesn't exist!");
			return laststr;
		}
		
		try {
			BufferedReader reader = new BufferedReader(new FileReader(file));
			String tmp = "";
			while ((tmp = reader.readLine()) != null) {
				laststr = laststr + tmp;
			}
			reader.close();
		} catch (Exception e) {
			e.printStackTrace();
			return "";
		}
		return laststr;
	}
	
	/**
	 * 开始加载不同算法提供的数据源
	 * @param targetMap
	 */
	static public void startLoader(Map<String, String> targetMap) {
		List<BasicFeatureLoader> loaderList = new ArrayList<BasicFeatureLoader>();
		
		for (Entry<String, String> each : targetMap.entrySet()) {
			BasicFeatureLoader loader = LoaderFactory.createLoader(each.getKey(), each.getValue());
			if (null != loader) {
				loaderList.add(loader);
			}
		}
		for (BasicFeatureLoader each : loaderList) {
			each.load();
		}
	}
	
	public static void main(String[] args) {
		Clogging.InitLogger("UserFeatureLoader");
		if (args.length != 1) {
			Clogging.Fatal("UserFeatureLoader error configuration path!");
		}
		Map<String, String> fileTypeMap = FeatureLoader.getTypeFileMap(args[0]);
		FeatureLoader.startLoader(fileTypeMap);
		System.exit(0);
	}
}