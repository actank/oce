package com.xiaonei.search3.model;

import java.sql.ResultSet;
import java.sql.SQLException;
import java.util.Calendar;
import java.util.List;
import java.util.Vector;

import org.apache.lucene.document.DateTools;
import org.apache.lucene.document.Document;
import org.apache.lucene.document.Field;

import com.xiaonei.search2.model.Constraints;
import com.xiaonei.search2.model.UserFieldNames;
import com.xiaonei.search2.model.info.CollegeInfo;
import com.xiaonei.search2.model.info.ElementarySchoolInfo;
import com.xiaonei.search2.model.info.HighSchoolInfo;
import com.xiaonei.search2.model.info.JuniorHighSchoolInfo;
import com.xiaonei.search2.model.info.NetworkInfo;
import com.xiaonei.search2.model.info.RegionInfo;
import com.xiaonei.search2.model.info.UniversityInfo;
import com.xiaonei.search2.model.info.UserInfo;
import com.xiaonei.search2.model.info.WorkplaceInfo;
import com.xiaonei.search2.util.ExceptionUtil;
import com.xiaonei.search2.util.IPToCity;
import com.xiaonei.search2.util.PinyinMap;
import com.xiaonei.xce.friendfinderbyip.FriendFinderByIPAdapter;
import com.xiaonei.xce.friendfinderbyip.IPData;

/**
 * 搜索结果的单个对象类。
 * 
 * @author bochun.bai@dudu-inc.com
 * 
 */
public class UserIndexView implements Indexable {

	private boolean isEmpty(String name) {
		return ((name == null) || (name.trim().isEmpty()));
	}

	/** Field是索引中对应一段数据,可能被查询或者被检索,这里完成其建索引的相关配置 **/
	private List<Field> toNetworkFields(NetworkInfo info) {
		List<Field> result = new Vector<Field>();

		if (info.id > 0) {
			result.add(new Field(UserFieldNames.NETWORK_ID, String
					.valueOf(info.id), Field.Store.NO, Field.Index.NOT_ANALYZED));
		}

		if (info.stage != 0) {
			result.add(new Field(UserFieldNames.NETWORK_STAGE, String
					.valueOf(info.stage), Field.Store.NO,
					Field.Index.NOT_ANALYZED));
		}
		if (!isEmpty(info.name)) {
			result.add(new Field(UserFieldNames.NETWORK_NAME, info.name,
					Field.Store.NO, Field.Index.ANALYZED));
		}
		return result;
	}

	/** Field是索引中对应一段数据,可能被查询或者被检索,这里完成其建索引的相关配置 **/
	private List<Field> toUniversityFields(UniversityInfo info) {
		List<Field> result = new Vector<Field>();

		if (info.id > 0) {
			result.add(new Field(UserFieldNames.UNIVERSITY + "."
					+ UserFieldNames.UNIVERSITY_INFO_ID, String
					.valueOf(info.id), Field.Store.NO, Field.Index.NOT_ANALYZED));
		}
		if (info.year > 0) {
			result.add(new Field(UserFieldNames.UNIVERSITY + "."
					+ UserFieldNames.UNIVERSITY_INFO_ENROLL_YEAR, String
					.valueOf(info.year), Field.Store.NO,
					Field.Index.NOT_ANALYZED));
		}
		if (!isEmpty(info.name)) {
			result.add(new Field(UserFieldNames.UNIVERSITY + "."
					+ UserFieldNames.UNIVERSITY_INFO_NAME, info.name,
					Field.Store.NO, Field.Index.ANALYZED));
		}
		if (!isEmpty(info.dorm)) {
			result.add(new Field(UserFieldNames.UNIVERSITY + "."
					+ UserFieldNames.UNIVERSITY_INFO_DORM, info.dorm,
					Field.Store.NO, Field.Index.ANALYZED));
		}
		if (!isEmpty(info.department)) {
			result.add(new Field(UserFieldNames.UNIVERSITY + "."
					+ UserFieldNames.UNIVERSITY_INFO_DEPARTMENT,
					info.department, Field.Store.NO, Field.Index.ANALYZED));
		}

		return result;
	}

	/** Field是索引中对应一段数据,可能被查询或者被检索,这里完成其建索引的相关配置 **/
	private List<Field> toHighSchoolFields(HighSchoolInfo info) {
		List<Field> result = new Vector<Field>();

		if (!isEmpty(info.name)) {
			result.add(new Field(UserFieldNames.HIGH_SCHOOL + "."
					+ UserFieldNames.HIGH_SCHOOL_INFO_NAME, info.name,
					Field.Store.NO, Field.Index.ANALYZED));
		}
		if (info.id > 0) {
			result.add(new Field(UserFieldNames.HIGH_SCHOOL + "."
					+ UserFieldNames.HIGH_SCHOOL_INFO_ID, String
					.valueOf(info.id), Field.Store.NO, Field.Index.NOT_ANALYZED));
		}
		if (info.year > 0) {
			result.add(new Field(UserFieldNames.HIGH_SCHOOL + "."
					+ UserFieldNames.HIGH_SCHOOL_INFO_ENROLL_YEAR, String
					.valueOf(info.year), Field.Store.NO,
					Field.Index.NOT_ANALYZED));
		}
		if (!isEmpty(info.clazz1)) {
			result.add(new Field(UserFieldNames.HIGH_SCHOOL + "."
					+ UserFieldNames.HIGH_SCHOOL_INFO_CLASS1, info.clazz1,
					Field.Store.NO, Field.Index.NOT_ANALYZED));
		}
		if (!isEmpty(info.clazz2)) {
			result.add(new Field(UserFieldNames.HIGH_SCHOOL + "."
					+ UserFieldNames.HIGH_SCHOOL_INFO_CLASS2, info.clazz2,
					Field.Store.NO, Field.Index.NOT_ANALYZED));
		}
		if (!isEmpty(info.clazz3)) {
			result.add(new Field(UserFieldNames.HIGH_SCHOOL + "."
					+ UserFieldNames.HIGH_SCHOOL_INFO_CLASS3, info.clazz3,
					Field.Store.NO, Field.Index.NOT_ANALYZED));
		}

		return result;
	}

	/** Field是索引中对应一段数据,可能被查询或者被检索,这里完成其建索引的相关配置 **/
	private List<Field> toWorkplaceFields(WorkplaceInfo info) {
		List<Field> result = new Vector<Field>();

		if (info.id > 0) {
			result.add(new Field(UserFieldNames.WORKPLACE_INFO_ID, String
					.valueOf(info.id), Field.Store.NO, Field.Index.NOT_ANALYZED));
		}
		if (!isEmpty(info.name)) {
			result.add(new Field(UserFieldNames.WORKPLACE_INFO_NAME, info.name,
					Field.Store.NO, Field.Index.ANALYZED));
		}
		return result;
	}

	/** Field是索引中对应一段数据,可能被查询或者被检索,这里完成其建索引的相关配置 **/
	private List<Field> toRegionFields(RegionInfo info) {
		List<Field> result = new Vector<Field>();

		if (info.id > 0) {
			result.add(new Field(UserFieldNames.REGION_INFO_ID, String
					.valueOf(info.id), Field.Store.NO, Field.Index.NOT_ANALYZED));
		}
		if (!isEmpty(info.province)) {
			result.add(new Field(UserFieldNames.REGION_INFO_PROVINCE_NAME,
					info.province, Field.Store.NO, Field.Index.ANALYZED));
		}
		if (!isEmpty(info.city)) {
			result.add(new Field(UserFieldNames.REGION_INFO_CITY_NAME,
					info.city, Field.Store.NO, Field.Index.ANALYZED));
		}
		return result;
	}

	/** Field是索引中对应一段数据,可能被查询或者被检索,这里完成其建索引的相关配置 **/
	private List<Field> toCollegeFields(CollegeInfo info) {
		List<Field> result = new Vector<Field>();

		if (info.id > 0) {
			result.add(new Field(UserFieldNames.COLLEGE_INFO_COLLEGE_ID, String
					.valueOf(info.id), Field.Store.NO, Field.Index.NOT_ANALYZED));
		}
		if (info.year > 0) {
			result.add(new Field(UserFieldNames.COLLEGE_INFO_ENROLL_YEAR,
					String.valueOf(info.year), Field.Store.NO,
					Field.Index.NOT_ANALYZED));
		}
		if (!isEmpty(info.name)) {
			result.add(new Field(UserFieldNames.COLLEGE_INFO_COLLEGE_NAME,
					info.name, Field.Store.NO, Field.Index.ANALYZED));
		}
		if (!isEmpty(info.department)) {
			result.add(new Field(UserFieldNames.COLLEGE_INFO_DEPARTMENT,
					info.department, Field.Store.NO, Field.Index.ANALYZED));
		}
		return result;
	}

	/** Field是索引中对应一段数据,可能被查询或者被检索,这里完成其建索引的相关配置 **/
	private List<Field> toElementarySchoolFields(ElementarySchoolInfo info) {
		List<Field> result = new Vector<Field>();

		if (info.id > 0) {
			result.add(new Field(
					UserFieldNames.ELEMENTARY_SCHOOL_INFO_SCHOOL_ID, String
							.valueOf(info.id), Field.Store.NO,
					Field.Index.NOT_ANALYZED));
		}
		if (info.year > 0) {
			result.add(new Field(
					UserFieldNames.ELEMENTARY_SCHOOL_INFO_SCHOOL_YEAR, String
							.valueOf(info.year), Field.Store.NO,
					Field.Index.NOT_ANALYZED));
		}
		if (!isEmpty(info.name))
			result.add(new Field(
					UserFieldNames.ELEMENTARY_SCHOOL_INFO_SCHOOL_NAME,
					info.name, Field.Store.NO, Field.Index.ANALYZED));
		return result;
	}

	/** Field是索引中对应一段数据,可能被查询或者被检索,这里完成其建索引的相关配置 **/
	private List<Field> toJuniorSchoolFields(JuniorHighSchoolInfo info) {
		List<Field> result = new Vector<Field>();

		if (info.id > 0) {
			result.add(new Field(
					UserFieldNames.JUNIOR_HIGH_SCHOOL_INFO_SCHOOL_ID, String
							.valueOf(info.id), Field.Store.NO,
					Field.Index.NOT_ANALYZED));
		}
		if (info.year > 0) {
			result.add(new Field(
					UserFieldNames.JUNIOR_HIGH_SCHOOL_INFO_SCHOOL_YEAR, String
							.valueOf(info.year), Field.Store.NO,
					Field.Index.NOT_ANALYZED));
		}
		if (!isEmpty(info.name)) {
			result.add(new Field(
					UserFieldNames.JUNIOR_HIGH_SCHOOL_INFO_SCHOOL_NAME,
					info.name, Field.Store.NO, Field.Index.ANALYZED));
		}
		return result;
	}

	// 用户的基本信息
	UserInfo info_;

	public UserIndexView() {
		info_ = new UserInfo();
	}

	public UserIndexView(UserInfo info) {
		info_ = info;
	}

	@Override
	public String toString() {
		return toStringBuffer().toString();
	}

	public int getId() {
		return info_.id;
	}

	public StringBuffer toStringBuffer() {
		StringBuffer result = new StringBuffer();
		result.append(info_.id).append(" ");
		result.append(info_.name).append(" ");
		result.append(info_.gender).append(" ");
		result.append(info_.birth_year).append(" ");
		result.append(info_.birth_month).append(" ");
		result.append(info_.birth_day).append(" ");
		result.append(info_.astrology).append(" ");
		result.append(info_.homeprovince).append(" ");
		result.append(info_.homecity).append(" ");
		// result.append(info_.selected).append(" ");
		result.append(info_.state).append(" ");
		result.append(info_.star).append(" ");
		result.append(info_.browse_config).append(" ");
		result.append(info_.status_config).append(" ");
		result.append(info_.fav_interest).append(" ");
		result.append(info_.fav_music).append(" ");
		result.append(info_.fav_movie).append(" ");
		result.append(info_.fav_game).append(" ");
		result.append(info_.fav_comic).append(" ");
		result.append(info_.fav_sport).append(" ");
		result.append(info_.fav_book).append(" ");
		result.append(info_.fav_society).append(" ");
		return result;
	}

	/**
	 * Field是索引中对应一段数据,可能被查询或者被检索,这里完成其建索引的相关配置,针对的是用户的基本资料.
	 * <p>
	 * 这里完成的是用户的基本资料的toFields逻辑
	 * </p>
	 **/
	public List<Field> toFields() {
		List<Field> result = new Vector<Field>();

		if (info_.id == 0 || isEmpty(info_.name)) {
			return null;
		}
		String pinyin = PinyinMap.getInstance().getPinyinString(info_.name);

		result.add(new Field(UserFieldNames.ID, String.valueOf(info_.id),
				Field.Store.YES, Field.Index.NOT_ANALYZED));
		result.add(new Field(UserFieldNames.NAME, info_.name, Field.Store.NO,
				Field.Index.NOT_ANALYZED));
		result.add(new Field(Constraints.SPLITNAME, info_.name + " " + pinyin,
				Field.Store.NO, Field.Index.ANALYZED));

		if (!isEmpty(pinyin)) {
			result.add(new Field(UserFieldNames.NAME_PINYIN, pinyin,
					Field.Store.NO, Field.Index.ANALYZED));
		}
		if (!isEmpty(info_.signature)) {
			result.add(new Field(UserFieldNames.SIGNATURE, info_.signature,
					Field.Store.NO, Field.Index.NOT_ANALYZED));
			result.add(new Field(Constraints.SPLITSIGNATURE, info_.signature,
					Field.Store.NO, Field.Index.ANALYZED));
		}
		result.add(new Field(UserFieldNames.GENDER, info_.gender,
				Field.Store.NO, Field.Index.NOT_ANALYZED));
		if (info_.birth_year > 0) {
			result.add(new Field(UserFieldNames.BIRTH_YEAR, String
					.valueOf(info_.birth_year), Field.Store.NO,
					Field.Index.NOT_ANALYZED));
		}
		if (info_.birth_month > 0) {
			result.add(new Field(UserFieldNames.BIRTH_MONTH, String
					.valueOf(info_.birth_month), Field.Store.NO,
					Field.Index.NOT_ANALYZED));
		}
		if (info_.birth_day > 0) {
			result.add(new Field(UserFieldNames.BIRTH_DAY, String
					.valueOf(info_.birth_day), Field.Store.NO,
					Field.Index.NOT_ANALYZED));
		}
		if (!isEmpty(info_.astrology)) {
			result.add(new Field(UserFieldNames.ASTROLOGY, info_.astrology,
					Field.Store.NO, Field.Index.NOT_ANALYZED));
		}
		if (!isEmpty(info_.homeprovince)) {
			result.add(new Field(UserFieldNames.HOME_PROVINCE,
					info_.homeprovince, Field.Store.NO, Field.Index.ANALYZED));
		}
		if (!isEmpty(info_.homecity)) {
			result.add(new Field(UserFieldNames.HOME_CITY, info_.homecity,
					Field.Store.NO, Field.Index.ANALYZED));
		}

		// result.add(new Field(UserFieldNames.SELECTED,
		// String.valueOf(info_.selected),
		result.add(new Field(UserFieldNames.SELECTED, String
				.valueOf((info_.state & 1)), Field.Store.NO,
				Field.Index.NOT_ANALYZED));

		if (!isEmpty(info_.fav_interest)) {
			result.add(new Field(UserFieldNames.FAV_INTEREST,
					info_.fav_interest, Field.Store.NO, Field.Index.ANALYZED));
		}
		if (!isEmpty(info_.fav_music)) {
			result.add(new Field(UserFieldNames.FAV_MUSIC, info_.fav_music,
					Field.Store.NO, Field.Index.ANALYZED));
		}
		if (!isEmpty(info_.fav_movie)) {
			result.add(new Field(UserFieldNames.FAV_MOVIE, info_.fav_movie,
					Field.Store.NO, Field.Index.ANALYZED));
		}
		if (!isEmpty(info_.fav_game)) {
			result.add(new Field(UserFieldNames.FAV_GAME, info_.fav_game,
					Field.Store.NO, Field.Index.ANALYZED));
		}
		if (!isEmpty(info_.fav_comic)) {
			result.add(new Field(UserFieldNames.FAV_COMIC, info_.fav_comic,
					Field.Store.NO, Field.Index.ANALYZED));
		}
		if (!isEmpty(info_.fav_sport)) {
			result.add(new Field(UserFieldNames.FAV_SPORT, info_.fav_sport,
					Field.Store.NO, Field.Index.ANALYZED));
		}
		if (!isEmpty(info_.fav_book)) {
			result.add(new Field(UserFieldNames.FAV_BOOK, info_.fav_book,
					Field.Store.NO, Field.Index.ANALYZED));
		}
		if (!isEmpty(info_.fav_society)) {
			result.add(new Field(UserFieldNames.FAV_SOCIETY, info_.fav_society,
					Field.Store.NO, Field.Index.ANALYZED));
		}
		result.add(new Field(UserFieldNames.LASTLOGINTIME,
				DateTools.timeToString(info_.lastlogintime,
						DateTools.Resolution.MINUTE), Field.Store.NO,
				Field.Index.NOT_ANALYZED));
		result.add(new Field(UserFieldNames.CURRENT_STAGE, String
				.valueOf(info_.current_stage), Field.Store.NO,
				Field.Index.NOT_ANALYZED));

		if (info_.basic_config > 0) {
			result.add(new Field(UserFieldNames.BASIC_CONFIG, String
					.valueOf(info_.basic_config), Field.Store.NO,
					Field.Index.NOT_ANALYZED));
		}
		if (info_.browse_config > 0) {
			result.add(new Field(UserFieldNames.BROWSE_CONFIG, String
					.valueOf(info_.browse_config), Field.Store.NO,
					Field.Index.NOT_ANALYZED));
		}
		if (info_.status_config > 0) {
			result.add(new Field(UserFieldNames.STATUS_CONFIG, String
					.valueOf(info_.status_config), Field.Store.NO,
					Field.Index.NOT_ANALYZED));
		}

		int age = Calendar.getInstance().get(Calendar.YEAR) - info_.birth_year;
		if (age >= 0 && age <= 100) {
			result.add(new Field(UserFieldNames.AGE, String.valueOf(age),
					Field.Store.NO, Field.Index.NOT_ANALYZED));
		}

		result.add(new Field(UserFieldNames.RANGE_AGE, String
				.valueOf(UserFieldNames.getAgeRange(age)), Field.Store.NO,
				Field.Index.NOT_ANALYZED));

		result.add(new Field(UserFieldNames.RANGE_VIEWCOUNT, String
				.valueOf(UserFieldNames.getViewCountRange(info_.viewcount)),
				Field.Store.NO, Field.Index.NOT_ANALYZED));

		long now = System.currentTimeMillis();
		result.add(new Field(UserFieldNames.LASTLOGIN15, String.valueOf(((now
				- info_.lastlogintime < 15L * Constraints.ANHOUR) ? 1 : 0)),
				Field.Store.NO, Field.Index.NOT_ANALYZED));
		result.add(new Field(UserFieldNames.LASTLOGIN07, String.valueOf(((now
				- info_.lastlogintime < 7L * Constraints.ANHOUR) ? 1 : 0)),
				Field.Store.NO, Field.Index.NOT_ANALYZED));

		try {
			List<IPData> ip_datas = FriendFinderByIPAdapter.getInstance()
					.getFreqIP(info_.id, 1);
			for (IPData ip_data : ip_datas) {
				IPToCity.Region region = IPToCity.getInstance().getCityByIP(
						ip_data.getIP());
				if (region != null) {
					String province = region.getProvince();
					String city = region.getCity();

					if (!isEmpty(province)) {
						result.add(new Field(
								UserFieldNames.REGION_LOGIN_PROVINCE, province,
								Field.Store.NO, Field.Index.ANALYZED));
					}
					if (!isEmpty(city)) {
						if (!city.endsWith("市")) {
							city += "市";
						}
						result.add(new Field(UserFieldNames.REGION_LOGIN_CITY,
								city, Field.Store.NO, Field.Index.ANALYZED));
					}
				}
			}
		} catch (Exception e) {
			ExceptionUtil.logException(e);
		}

		return result;
	}

	public Document toDocument() {
		Document doc = new Document();
		List<Field> generalFields = toFields();
		for (Field generalField : generalFields) {
			doc.add(generalField);
		}
		
		for (NetworkInfo info : info_.networks) {
			if (info == null) {
				continue;
			}
			for (Field networkField : toNetworkFields(info)) {
				doc.add(networkField);
			}
		}

		for (RegionInfo info : info_.regions) {
			if (info == null) {
				continue;
			}
			for (Field infoField : toRegionFields(info)) {
				doc.add(infoField);
			}
		}
		for (WorkplaceInfo info : info_.workplaces) {
			if (info == null) {
				continue;
			}
			for (Field infoField : toWorkplaceFields(info)) {
				doc.add(infoField);
			}
		}
		for (UniversityInfo info : info_.universities) {
			if (info == null) {
				continue;
			}
			for (Field infoField : toUniversityFields(info)) {
				doc.add(infoField);
			}
		}
		for (CollegeInfo info : info_.colleges) {
			if (info == null) {
				continue;
			}
			for (Field infoField : toCollegeFields(info)) {
				doc.add(infoField);
			}
		}
		for (HighSchoolInfo info : info_.highSchools) {
			if (info == null) {
				continue;
			}
			for (Field infoField : toHighSchoolFields(info)) {
				doc.add(infoField);
			}
		}
		for (JuniorHighSchoolInfo info : info_.juniorSchools) {
			if (info == null) {
				continue;
			}
			for (Field infoField : toJuniorSchoolFields(info)) {
				doc.add(infoField);
			}
		}
		for (ElementarySchoolInfo info : info_.elementarySchools) {
			if (info == null) {
				continue;
			}
			for (Field infoField : toElementarySchoolFields(info)) {
				doc.add(infoField);
			}
		}
		return doc;

	}
 	/** 一个Document是一些Field的集合,代表一个被索引的单元 **/
	public List<Document> toDocuments() {
		List<Field> generalFields = toFields();
		if (generalFields == null) {
			return null;
		}

		List<Document> docs = new Vector<Document>();
		Document docGeneral = new Document();
		/** 生成general信息的document **/
		for (Field generalField : generalFields) {
			docGeneral.add(generalField);
		}
		docs.add(docGeneral);
		/** 生成network信息的document **/
		Document docNetwork = new Document();
		docNetwork.add(generalFields.get(0));

		for (NetworkInfo info : info_.networks) {
			if (info == null) {
				continue;
			}
			for (Field networkField : toNetworkFields(info)) {
				docNetwork.add(networkField);
			}
		}
		docs.add(docNetwork);

		Document docInfo = new Document();
		docInfo.add(generalFields.get(0));
		/** 生成info信息的document **/
		for (RegionInfo info : info_.regions) {
			if (info == null) {
				continue;
			}
			for (Field infoField : toRegionFields(info)) {
				docInfo.add(infoField);
			}
		}
		for (WorkplaceInfo info : info_.workplaces) {
			if (info == null) {
				continue;
			}
			for (Field infoField : toWorkplaceFields(info)) {
				docInfo.add(infoField);
			}
		}
		for (UniversityInfo info : info_.universities) {
			if (info == null) {
				continue;
			}
			for (Field infoField : toUniversityFields(info)) {
				docInfo.add(infoField);
			}
		}
		for (CollegeInfo info : info_.colleges) {
			if (info == null) {
				continue;
			}
			for (Field infoField : toCollegeFields(info)) {
				docInfo.add(infoField);
			}
		}
		for (HighSchoolInfo info : info_.highSchools) {
			if (info == null) {
				continue;
			}
			for (Field infoField : toHighSchoolFields(info)) {
				docInfo.add(infoField);
			}
		}
		for (JuniorHighSchoolInfo info : info_.juniorSchools) {
			if (info == null) {
				continue;
			}
			for (Field infoField : toJuniorSchoolFields(info)) {
				docInfo.add(infoField);
			}
		}
		for (ElementarySchoolInfo info : info_.elementarySchools) {
			if (info == null) {
				continue;
			}
			for (Field infoField : toElementarySchoolFields(info)) {
				docInfo.add(infoField);
			}
		}
		docs.add(docInfo);

		float boost = 0.0f;

		// if (info_.selected == 1) {
		if ((info_.state & 1) == 1) {
			boost += 40;
		}

		switch (info_.star & 3) {
		case 1:
		case 2:
		case 3:
			boost += 40;
			break;
		default:
			boost += 0;
		}

		if ((!info_.headurl.equals("0/0/head.jpg"))
				&& (!info_.headurl.equals("0/0/men_head.gif"))
				&& (!info_.headurl.equals("women_head.gif"))) {
			boost += 10;
		}

		if (!info_.homecity.equals("")) {
			boost += 8;
		}

		long now = System.currentTimeMillis();
		if (now - info_.lastlogintime < 24L * Constraints.ANHOUR) {
			boost += 100;
		} else if (now - info_.lastlogintime < 3L * 24L * Constraints.ANHOUR) {
			boost += 70;
		} else if (now - info_.lastlogintime < 7L * 24L * Constraints.ANHOUR) {
			boost += 50;
		} else if (now - info_.lastlogintime < 30L * 24L * Constraints.ANHOUR) {
			boost += 30;
		}

		// float docBoost = boost + (float)Math.random() / 4;
		float docBoost = 1.0f + ((float) Math.pow(boost, 0.78) + (float) Math
				.random()) / 100.0f;

		for (Document doc : docs) {
			doc.setBoost(docBoost);
		}

		return docs;
	}

	// 从ResultSet中取得基本数据
	public void parseGeneralBasic(ResultSet rs) throws SQLException {
		info_.id = rs.getInt(UserFieldNames.ID);
		info_.name = rs.getString(UserFieldNames.NAME);
		info_.gender = rs.getString(UserFieldNames.GENDER);

		if (info_.gender == null
				|| !(info_.gender.equals("男生") || info_.gender.equals("女生"))) {
			info_.gender = "未知";
		}
		try {
			info_.birth_year = rs.getInt(UserFieldNames.BIRTH_YEAR);
		} catch (Exception e) {
			info_.birth_year = 0;
		}
		try {
			info_.birth_month = rs.getInt(UserFieldNames.BIRTH_MONTH);
		} catch (Exception e) {
			info_.birth_month = 0;
		}
		try {
			info_.birth_day = rs.getInt(UserFieldNames.BIRTH_DAY);
		} catch (Exception e) {
			info_.birth_day = 0;
		}
		info_.astrology = rs.getString(UserFieldNames.ASTROLOGY);
		info_.homeprovince = rs.getString(UserFieldNames.HOME_PROVINCE);
		info_.homecity = rs.getString(UserFieldNames.HOME_CITY);
		try {
			info_.star = rs.getInt(UserFieldNames.STAR);
		} catch (Exception e) {
			info_.star = 1;
		}
		try {
			info_.current_stage = rs.getInt(UserFieldNames.CURRENT_STAGE);
		} catch (Exception e) {
			info_.current_stage = 90;
		}
		info_.status = rs.getInt(UserFieldNames.STATUS);
	}

	// 从ResultSet中取得最后登录时间
	public void parseGeneralTime(ResultSet rs) throws SQLException {
		try {
			info_.lastlogintime = rs.getDate(UserFieldNames.LASTLOGINTIME)
					.getTime();
		} catch (SQLException e) {
			info_.lastlogintime = 0;
			e.printStackTrace();
			System.out.println("Lastlogintime is zero for id:" + info_.id);
		}
	}

	// 从ResultSet中取得兴趣信息
	public void parseFavInterest(ResultSet rs) throws SQLException {
		info_.fav_interest += rs.getString(UserFieldNames.FAV_INTEREST) + " ";
	}

	public void parseFavMusic(ResultSet rs) throws SQLException {
		info_.fav_music += rs.getString(UserFieldNames.FAV_MUSIC) + " ";
	}

	public void parseFavMovie(ResultSet rs) throws SQLException {
		info_.fav_movie += rs.getString(UserFieldNames.FAV_MOVIE) + " ";
	}

	public void parseFavGame(ResultSet rs) throws SQLException {
		info_.fav_game += rs.getString(UserFieldNames.FAV_GAME) + " ";
	}

	public void parseFavComic(ResultSet rs) throws SQLException {
		info_.fav_comic += rs.getString(UserFieldNames.FAV_COMIC) + " ";
	}

	public void parseFavSport(ResultSet rs) throws SQLException {
		info_.fav_sport += rs.getString(UserFieldNames.FAV_SPORT) + " ";
	}

	public void parseFavBook(ResultSet rs) throws SQLException {
		info_.fav_book += rs.getString(UserFieldNames.FAV_BOOK) + " ";
	}

	public void parseFavSociety(ResultSet rs) throws SQLException {
		info_.fav_society += rs.getString(UserFieldNames.FAV_SOCIETY) + " ";
	}

	// 从ResultSet中哦功能取得用户隐私设置
	public void parseUserConfig(ResultSet rs) throws SQLException {
		info_.basic_config = rs.getInt(UserFieldNames.BASIC_CONFIG);
		info_.browse_config = rs.getInt(UserFieldNames.BROWSE_CONFIG);
		info_.status_config = rs.getInt(UserFieldNames.STATUS_CONFIG);
	}

	public void parseUserState(ResultSet rs) throws SQLException {
		info_.state = rs.getInt(UserFieldNames.STATE);
	}

	public void parseUserViewCount(ResultSet rs) throws SQLException {
		info_.viewcount = rs.getInt(UserFieldNames.VIEWCOUNT);
	}

	public void parseNetwork(ResultSet rs) throws SQLException {
		int id = rs.getInt(UserFieldNames.NETWORK_ID);
		String name = rs.getString(UserFieldNames.NETWORK_NAME);
		int stage = rs.getInt(UserFieldNames.NETWORK_STAGE);

		NetworkInfo ni = new NetworkInfo(id, name, stage);

		info_.networks.add(ni);
	}

	public void parseUniversityInfo(ResultSet rs) throws SQLException {
		int id = rs.getInt(UserFieldNames.UNIVERSITY_INFO_ID);
		String name = rs.getString(UserFieldNames.UNIVERSITY_INFO_NAME);
		String dorm = rs.getString(UserFieldNames.UNIVERSITY_INFO_DORM);
		String department = rs
				.getString(UserFieldNames.UNIVERSITY_INFO_DEPARTMENT);
		int year = rs.getInt(UserFieldNames.UNIVERSITY_INFO_ENROLL_YEAR);

		info_.universities.add(new UniversityInfo(id, name, department, dorm,
				year));
	}

	public void parseHighSchoolInfo(ResultSet rs) throws SQLException {
		int id = rs.getInt(UserFieldNames.HIGH_SCHOOL_INFO_ID);
		String name = rs.getString(UserFieldNames.HIGH_SCHOOL_INFO_NAME);
		String clazz1 = rs.getString(UserFieldNames.HIGH_SCHOOL_INFO_CLASS1);
		String clazz2 = rs.getString(UserFieldNames.HIGH_SCHOOL_INFO_CLASS2);
		String clazz3 = rs.getString(UserFieldNames.HIGH_SCHOOL_INFO_CLASS3);
		int year = rs.getInt(UserFieldNames.HIGH_SCHOOL_INFO_ENROLL_YEAR);

		info_.highSchools.add(new HighSchoolInfo(id, name, clazz1, clazz2,
				clazz3, year));
	}

	public void parseWorkplaceInfo(ResultSet rs) throws SQLException {
		int id = rs.getInt(UserFieldNames.WORKPLACE_INFO_ID);
		String name = rs.getString(UserFieldNames.WORKPLACE_INFO_NAME);

		info_.workplaces.add(new WorkplaceInfo(id, name));
	}

	public void parseUsers(ResultSet rs) throws SQLException {
		info_.signature = rs.getString(UserFieldNames.SIGNATURE);
	}

	public void parseUserNames(ResultSet rs) throws SQLException {
		info_.signature = rs.getString("nickname");
	}

	public void parseUserUrl(ResultSet rs) throws SQLException {
		info_.tinyurl = rs.getString(UserFieldNames.TINYURL);
		if (info_.tinyurl == null) {
			info_.tinyurl = ".";
		}
		info_.headurl = rs.getString(UserFieldNames.HEADURL);
		if (info_.headurl == null) {
			info_.headurl = ".";
		}
	}

	public void parseRegionInfo(ResultSet rs) throws SQLException {
		int id = rs.getInt(UserFieldNames.REGION_INFO_ID);
		String province = rs
				.getString(UserFieldNames.REGION_INFO_PROVINCE_NAME);
		String city = rs.getString(UserFieldNames.REGION_INFO_CITY_NAME);

		info_.regions.add(new RegionInfo(id, province, city));
	}

	public void parseCollegeInfo(ResultSet rs) throws SQLException {
		int id = rs.getInt(UserFieldNames.COLLEGE_INFO_COLLEGE_ID);
		String name = rs.getString(UserFieldNames.COLLEGE_INFO_COLLEGE_NAME);
		String department = rs
				.getString(UserFieldNames.COLLEGE_INFO_DEPARTMENT);
		int year = rs.getInt(UserFieldNames.COLLEGE_INFO_ENROLL_YEAR);

		info_.colleges.add(new CollegeInfo(id, name, department, year));
	}

	public void parseElementarySchoolInfo(ResultSet rs) throws SQLException {
		int id = rs.getInt(UserFieldNames.ELEMENTARY_SCHOOL_INFO_SCHOOL_ID);
		String name = rs
				.getString(UserFieldNames.ELEMENTARY_SCHOOL_INFO_SCHOOL_NAME);
		int year = rs.getInt(UserFieldNames.ELEMENTARY_SCHOOL_INFO_SCHOOL_YEAR);

		info_.elementarySchools.add(new ElementarySchoolInfo(id, name, year));
	}

	public void parseJuniorHighSchoolInfo(ResultSet rs) throws SQLException {
		int id = rs.getInt(UserFieldNames.JUNIOR_HIGH_SCHOOL_INFO_SCHOOL_ID);
		String name = rs
				.getString(UserFieldNames.JUNIOR_HIGH_SCHOOL_INFO_SCHOOL_NAME);
		int year = rs
				.getInt(UserFieldNames.JUNIOR_HIGH_SCHOOL_INFO_SCHOOL_YEAR);
		info_.juniorSchools.add(new JuniorHighSchoolInfo(id, name, year));
	}
}
