package build;

import index.Document;
import index.Field;
import index.IndexWriter;

import java.net.InetSocketAddress;
import java.util.ArrayList;
import java.util.List;
import java.util.Map;

import tokyotyrant.RDB;
import tokyotyrant.transcoder.ByteArrayTranscoder;

import com.xiaonei.search.util.db.DatabaseI;
import com.xiaonei.search.util.db.DatabaseIteratorI;
import com.xiaonei.search2.model.info.CollegeInfo;
import com.xiaonei.search2.model.info.HighSchoolInfo;
import com.xiaonei.search2.model.info.JuniorHighSchoolInfo;
import com.xiaonei.search2.model.info.UniversityInfo;
import com.xiaonei.search2.model.info.UserInfo;
import com.xiaonei.search2.model.info.UserInfoUnserializer;
import com.xiaonei.search2.model.info.WorkplaceInfo;
import common.FieldConfig;

/** 建立用户索引 **/
public class RebuildUserIndex {
	private static DatabaseIteratorI iterator_;
	private static DatabaseI database_;

	private static int max_field = 16;

	private static Document getDocument(UserInfo info) {

		Document doc = new Document(max_field);
		doc.addField(new Field("name", info.name));
		doc.addField(new Field("astrology", info.astrology));
		doc.addField(new Field("gender", info.gender));
		doc.addField(new Field("homecity", info.homecity));
		doc.addField(new Field("homeprovince", info.homeprovince));
		doc.addField(new Field("signature", info.signature));
		String univName = "";
		String univYear = "";
		String univNameAndYear = "";
		for (UniversityInfo uinfo : info.universities) {
			univName += uinfo.name + " ";
			univYear += uinfo.year + " ";
			univNameAndYear += uinfo.name + "." + uinfo.year + " ";
		}
		for (CollegeInfo cinfo : info.colleges) {
			univName += cinfo.name + " ";
			univYear += cinfo.year + " ";
			univNameAndYear += cinfo.name + "." + cinfo.year + " ";
		}
		doc.addField(new Field("univ.name", univName));
		doc.addField(new Field("univ.year", univYear));
		doc.addField(new Field("univ.nameyear", univNameAndYear));

		String highName = "";
		String highYear = "";
		String highNameAndYear = "";
		for (HighSchoolInfo hinfo : info.highSchools) {
			highName += hinfo.name + " ";
			highYear += hinfo.year + " ";
			highNameAndYear += hinfo.name + "." + hinfo.year + " ";
		}
		doc.addField(new Field("high.name", highName));
		doc.addField(new Field("high.year", highYear));
		doc.addField(new Field("high.nameyear", highNameAndYear));

		String juniorName = "";
		String juniorYear = "";
		String juniorNameAndYear = "";
		for (JuniorHighSchoolInfo jinfo : info.juniorSchools) {
			juniorName += jinfo.name + " ";
			juniorYear += jinfo.year + " ";
			juniorNameAndYear += jinfo.name + "." + jinfo.year + " ";
		}
		doc.addField(new Field("junior.name", juniorName));
		doc.addField(new Field("junior.year", juniorYear));
		doc.addField(new Field("junior.nameyear", juniorNameAndYear));

		String workName = "";
		for (WorkplaceInfo winfo : info.workplaces) {
			workName += winfo.name + " ";
		}
		doc.addField(new Field("workplace", workName));
		return doc;
	}

	public static void main(String[] args) throws Exception {
		// database_ = DatabaseFactory.getInstance().createDatabase();
		// database_.open("10.22.206.32 1981");
		long start = System.currentTimeMillis();
		RDB rdb = new RDB();
		rdb.open(new InetSocketAddress("10.22.206.32", 1981));
		rdb.setValueTranscoder(new ByteArrayTranscoder());
		rdb.iterinit();
		int cnt = 0;
		IndexWriter writer = new IndexWriter(args[0], null, max_field);
		writer.addField("name", 1 << FieldConfig.omitTf
				| 1 << FieldConfig.multiField);
		writer.addField("astrology", 1 << FieldConfig.omitTf);
		writer.addField("gender", 1 << FieldConfig.omitTf);
		writer.addField("homecity", 1 << FieldConfig.omitTf
				| 1 << FieldConfig.multiField);
		writer.addField("homeprovince", 1 << FieldConfig.omitTf
				| 1 << FieldConfig.multiField);
		writer.addField("signature", 1 << FieldConfig.omitTf);

		writer.addField("univ.name", 1 << FieldConfig.multiField);
		writer.addField("univ.year", 0);
		writer.addField("univ.nameyear", 0);

		writer.addField("high.name", 1 << FieldConfig.multiField);
		writer.addField("high.year", 0);
		writer.addField("high.nameyear", 0);

		writer.addField("junior.name", 1 << FieldConfig.multiField);
		writer.addField("junior.year", 0);
		writer.addField("junior.nameyear", 0);

		writer.addField("workplace", 1 << FieldConfig.multiField);

		while (true) {
			int count = 1000;
			int num = 0;
			List<Object> userids = new ArrayList<Object>(1000);
			while (num++ < count) {
				try {
					Object key = rdb.iternext();
					if (key != null) {
						userids.add(key);
					} else {
						System.out.println("not find");
						break;
					}
				} catch (Exception ex) {
					ex.printStackTrace();
				}
			}
			Map<Object, Object> objs = rdb.mget(userids
					.toArray(new Object[] {}));

			for (Map.Entry<Object, Object> obj : objs.entrySet()) {

				try {
					byte[] bytes = (byte[]) obj.getValue();
					// System.out.println(bytes.length);
					// 反序列化
					UserInfo info = UserInfoUnserializer.getInstance()
							.unserialize(bytes, true);
					if (info == null) {
						continue;
					}
					int doc = writer.addDocument(getDocument(info));
				} catch (Throwable ex) {
					ex.printStackTrace();
				}
			}
			System.out.println(cnt);
			cnt++;
			break;
//			if (objs.size() != 1000) {
//				System.out.println(objs.size());
//				break;
//			}
		}
		writer.stop();
	}
}
