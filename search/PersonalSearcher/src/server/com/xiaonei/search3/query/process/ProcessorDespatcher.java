package com.xiaonei.search3.query.process;

import java.util.HashMap;
import java.util.Map;
import java.util.Map.Entry;

import org.apache.lucene.search.BooleanQuery;
import org.apache.lucene.search.Query;
import org.apache.lucene.search.BooleanClause.Occur;

import com.xiaonei.search3.query.process.processors.AgeRangeProcessor;
import com.xiaonei.search3.query.process.processors.AstrologyProcessor;
import com.xiaonei.search3.query.process.processors.BirthdayProcessor;
import com.xiaonei.search3.query.process.processors.CollegeProcessor;
import com.xiaonei.search3.query.process.processors.ElementaryProcessor;
import com.xiaonei.search3.query.process.processors.FavBookProcessor;
import com.xiaonei.search3.query.process.processors.FavCartoonProcessor;
import com.xiaonei.search3.query.process.processors.FavGameProcessor;
import com.xiaonei.search3.query.process.processors.FavMusicProcessor;
import com.xiaonei.search3.query.process.processors.FavSportsProcessor;
import com.xiaonei.search3.query.process.processors.GenderProcessor;
import com.xiaonei.search3.query.process.processors.HighSchoolProcessor;
import com.xiaonei.search3.query.process.processors.InterestsProcessor;
import com.xiaonei.search3.query.process.processors.JuniorSchoolProcessor;
import com.xiaonei.search3.query.process.processors.NameProcessor;
import com.xiaonei.search3.query.process.processors.RegionProcessor;
import com.xiaonei.search3.query.process.processors.UniversityProcessor;
import com.xiaonei.search3.query.process.processors.WorkPlaceProcessor;

/**
 * 
 * @author xuemin.luo xuemin.luo@opi-corp.com
 **/
public class ProcessorDespatcher {
	private static ProcessorDespatcher _instance = new ProcessorDespatcher();

	public ProcessorDespatcher getInstance() {
		if (_instance == null) {
			_instance = new ProcessorDespatcher();
		}
		return _instance;
	}

	public static BooleanQuery process(Map<String, Map<String, String>> map) {
		BooleanQuery queryAll = new BooleanQuery();
		BooleanQuery queryMust = new BooleanQuery();
		BooleanQuery queryShould = new BooleanQuery();
		for (Entry<String, Map<String, String>> outerEntry : map.entrySet()) {
			String type = outerEntry.getKey();
			// TODO
			Query query = null;
			// 过滤条件是MUST的
			if (type.equals("ageRange")) {
				query = AgeRangeProcessor.process(map, 30.0f).get(0);
				queryMust.add(query, Occur.MUST);
			} else if (type.equals("astrology")) {
				query = AstrologyProcessor.process(map, 30.0f);
				queryMust.add(query, Occur.MUST);
			} else if (type.equals("birthday")) {
				query = BirthdayProcessor.process(map, 30.0f).get(0);
				queryMust.add(query, Occur.MUST);
			} else if (type.startsWith("college")) {
				query = CollegeProcessor.process(map, 30.0f).get(0);
				queryMust.add(query, Occur.MUST);
			} else if (type.startsWith("elementary")) {
				query = ElementaryProcessor.process(map, 30.0f).get(0);
				queryMust.add(query, Occur.MUST);
			} else if (type.equals("gender")) {
				query = GenderProcessor.process(map, 30.0f).get(0);
				queryMust.add(query, Occur.MUST);
			} else if (type.startsWith("highScl")) {
				query = HighSchoolProcessor.process(map, 30.0f).get(0);
				queryMust.add(query, Occur.MUST);
			} else if (type.startsWith("juniorScl")) {
				query = JuniorSchoolProcessor.process(map, 30.0f).get(0);
				queryMust.add(query, Occur.MUST);
			} else if (type.equals("name")) {
				query = NameProcessor.process(map, 30.0f).get(0);
				queryMust.add(query, Occur.MUST);
			} else if (type.equals("region")) {
				query = RegionProcessor.process(map, 30.0f).get(0);
				queryMust.add(query, Occur.MUST);
			} else if (type.startsWith("university")) {
				query = UniversityProcessor.process(map, 30.0f).get(0);
				queryMust.add(query, Occur.MUST);
			} else if (type.equals("workplace")) {
				query = WorkPlaceProcessor.process(map, 30.0f).get(0);
				queryMust.add(query, Occur.MUST);
			}
		}
		for (Entry<String, Map<String, String>> outerEntry : map.entrySet()) {
			String type = outerEntry.getKey();
			// TODO
			Query query = null;
			// 这些条件是SHOULD的
			if (type.equals("favBook")) {
				query = FavBookProcessor.process(map, 30.0f).get(0);
				queryShould.add(query, Occur.SHOULD);
			} else if (type.equals("favCartoon")) {
				query = FavCartoonProcessor.process(map, 30.0f).get(0);
				queryShould.add(query, Occur.SHOULD);
			} else if (type.equals("favGame")) {
				query = FavGameProcessor.process(map, 30.0f).get(0);
				queryShould.add(query, Occur.SHOULD);
			} else if (type.equals("favMusic")) {
				query = FavMusicProcessor.process(map, 30.0f).get(0);
				queryShould.add(query, Occur.SHOULD);
			} else if (type.equals("favSports")) {
				query = FavSportsProcessor.process(map, 30.0f).get(0);
				queryShould.add(query, Occur.SHOULD);
			} else if (type.equals("interest")) {
				query = InterestsProcessor.process(map, 30.0f).get(0);
				queryShould.add(query, Occur.SHOULD);
			}
		}
		queryAll.add(queryMust, Occur.MUST);
		queryAll.add(queryShould, Occur.SHOULD);
		return queryAll;
	}

	public static String getType() {
		return "all";
	}

	public static void main(String args[]) {
		Map<String, Map<String, String>> outerMap = new HashMap<String, Map<String, String>>();
		Map<String, String> innerMap = new HashMap<String, String>();
		// 年龄
		innerMap.put("ageRange", "3");
		outerMap.put("ageRange", innerMap);
		// 星座
		innerMap = new HashMap<String, String>();
		innerMap.put("astrology", "处女座");
		outerMap.put("astrology", innerMap);
		// 生日
		innerMap = new HashMap<String, String>();
		innerMap.put("year", "1987");
		innerMap.put("month", "10");
		innerMap.put("day", "13");
		outerMap.put("birthday", innerMap);
		// 中专
		innerMap = new HashMap<String, String>();
		innerMap.put("id", "1234");
		innerMap.put("name", "蓝翔技校");
		innerMap.put("department", "计算机科学与技术");
		innerMap.put("year", "2006");
		outerMap.put("college|222", innerMap);
		// 小学
		innerMap = new HashMap<String, String>();
		innerMap.put("id", "1234");
		innerMap.put("name", "福建省沙县实验小学");
		innerMap.put("year", "2006");
		outerMap.put("elementary|222", innerMap);
		// 书籍
		innerMap = new HashMap<String, String>();
		innerMap.put("favBook", "深入理解操作系统");
		outerMap.put("favBook", innerMap);
		// 漫画
		innerMap = new HashMap<String, String>();
		innerMap.put("favCartoon", "海贼王");
		outerMap.put("favCartoon", innerMap);
		// 游戏
		innerMap = new HashMap<String, String>();
		innerMap.put("favGame", "使命召唤");
		outerMap.put("favGame", innerMap);
		// 电影
		innerMap = new HashMap<String, String>();
		innerMap.put("favMovie", "这个杀手不太冷");
		outerMap.put("favMovie", innerMap);
		// 音乐
		innerMap = new HashMap<String, String>();
		innerMap.put("favMusic", "You Raise Me Up");
		outerMap.put("favMusic", innerMap);
		// 运动
		innerMap = new HashMap<String, String>();
		innerMap.put("favSports", "篮球");
		outerMap.put("favSports", innerMap);
		// 性别
		innerMap = new HashMap<String, String>();
		innerMap.put("gender", "1");
		outerMap.put("gender", innerMap);
		// 高中
		innerMap = new HashMap<String, String>();
		innerMap.put("id", "1234");
		innerMap.put("name", "厦门英才学校");
		innerMap.put("clazz1", "3");
		innerMap.put("clazz2", "3");
		innerMap.put("clazz3", "7");
		innerMap.put("year", "1998");
		outerMap.put("highScl|222", innerMap);
		// 兴趣
		innerMap = new HashMap<String, String>();
		innerMap.put("interests", "钓鱼");
		outerMap.put("interests", innerMap);
		// 初中
		innerMap = new HashMap<String, String>();
		innerMap.put("id", "1234");
		innerMap.put("name", "福建省沙县第三中学");
		innerMap.put("year", "2006");
		outerMap.put("juniorScl|222", innerMap);
		// 姓名
		innerMap = new HashMap<String, String>();
		innerMap.put("name", "张三");
		outerMap.put("name", innerMap);
		innerMap = new HashMap<String, String>();
		// 家乡
		innerMap = new HashMap<String, String>();
		innerMap.put("province_id", "1234");
		innerMap.put("province_name", "福建省");
		innerMap.put("city_id", "1234");
		innerMap.put("city_name", "三明市");
		outerMap.put("region|222", innerMap);
		// 大学
		innerMap = new HashMap<String, String>();
		innerMap.put("id", "1234");
		innerMap.put("name", "哈工大");
		innerMap.put("department", "计算机科学与技术");
		innerMap.put("year", "2006");
		outerMap.put("university|222", innerMap);
		// 工作地
		innerMap = new HashMap<String, String>();
		innerMap.put("workplace", "千橡互动 人人网");
		outerMap.put("workplace", innerMap);
		Query query = ProcessorDespatcher.process(outerMap);
		System.out.println(query.toString());
	}

}
