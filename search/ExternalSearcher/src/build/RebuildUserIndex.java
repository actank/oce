package build;

import java.io.File;
import java.io.IOException;
import java.net.InetSocketAddress;
import java.sql.Connection;
import java.sql.ResultSet;
import java.sql.Statement;
import java.util.ArrayList;
import java.util.Map;
import java.util.concurrent.ArrayBlockingQueue;
import java.util.concurrent.LinkedBlockingQueue;

import tokyotyrant.RDB;
import tokyotyrant.transcoder.ByteArrayTranscoder;

import com.xiaonei.search2.model.info.HighSchoolInfo;
import com.xiaonei.search2.model.info.JuniorHighSchoolInfo;
import com.xiaonei.search2.model.info.UniversityInfo;
import com.xiaonei.search2.model.info.UserInfo;
import com.xiaonei.search2.model.info.UserInfoUnserializer;
import com.xiaonei.search2.model.info.WorkplaceInfo;

import engine.common.FieldConfig;
import engine.index.Document;
import engine.index.Field;
import engine.index.IndexWriter;
import engine.index.SimpleFSIndexOutput;

/** 建立用户索引 **/
public class RebuildUserIndex {
	// private static DatabaseIteratorI iterator_;
	// private static DatabaseI database_;

	private static int max_field = 18;
	private static String white = "@###@";

	static ArrayList<Integer> ids = new ArrayList<Integer>(20000000);

	private static Document getDocument(UserInfo info) {

		Document doc = new Document(max_field);
		doc.addField(new Field("name", info.name));
		doc.addField(new Field("astrology", info.astrology));
		doc.addField(new Field("gender", info.gender));
		doc.addField(new Field("homecity", info.homecity));
		doc.addField(new Field("homeprovince", info.homeprovince));
		doc.addField(new Field("signature", info.signature));
		String univId = "";
		String univName = "";
		String univYear = "";
		String univNameAndYear = "";
		for (UniversityInfo uinfo : info.universities) {
			univId += uinfo.id + white;
			univName += uinfo.name + white;
			univYear += uinfo.year + white;
			univNameAndYear += uinfo.id + "." + uinfo.year + white;
		}
		// for (CollegeInfo cinfo : info.colleges) {
		// univId += cinfo.id + white;
		// univName += cinfo.id + white;
		// univYear += cinfo.year + white;
		// univNameAndYear += cinfo.id + "." + cinfo.year + white;
		// }
		doc.addField(new Field("univ.name", univId));
		doc.addField(new Field("univ.info", univName));
		doc.addField(new Field("univ.year", univYear));
		doc.addField(new Field("univ.nameyear", univNameAndYear));

		String highName = "";
		String highYear = "";
		String highNameAndYear = "";
		for (HighSchoolInfo hinfo : info.highSchools) {
			highName += hinfo.name + white;
			highYear += hinfo.year + white;
			highNameAndYear += hinfo.name + "." + hinfo.year + white;
		}
		doc.addField(new Field("high.name", highName));
		doc.addField(new Field("high.year", highYear));
		doc.addField(new Field("high.nameyear", highNameAndYear));

		String juniorName = "";
		String juniorYear = "";
		String juniorNameAndYear = "";
		for (JuniorHighSchoolInfo jinfo : info.juniorSchools) {
			juniorName += jinfo.name + white;
			juniorYear += jinfo.year + white;
			juniorNameAndYear += jinfo.name + "." + jinfo.year + white;
		}
		doc.addField(new Field("junior.name", juniorName));
		doc.addField(new Field("junior.year", juniorYear));
		doc.addField(new Field("junior.nameyear", juniorNameAndYear));

		String workName = "";
		for (WorkplaceInfo winfo : info.workplaces) {
			workName += winfo.name + white;
		}
		doc.addField(new Field("workplace", workName));
		int age = 2010 - info.birth_year;
		int num;
		if (age < 16) {
			num = 0;
		} else if (age >= 16 && age <= 22) {
			num = 1;
		} else if (age >= 23 && age <= 30) {
			num = 2;
		} else if (age >= 31 && age <= 40) {
			num = 3;
		} else {
			num = 4;
		}
		doc.addField(new Field("age", String.valueOf(num)));
		return doc;
	}

	public static IndexWriter init(String[] args) throws IOException {
		IndexWriter writer = new IndexWriter(args[0], null, max_field);
		writer.addField("name", FieldConfig.multiField | FieldConfig.noIndex);
		writer.addField("astrology", FieldConfig.noAnalyzer
				| FieldConfig.omitTf);
		writer.addField("gender", FieldConfig.noAnalyzer | FieldConfig.omitTf);
		writer.addField("homecity", FieldConfig.noAnalyzer | FieldConfig.omitTf);
		writer.addField("homeprovince", FieldConfig.noAnalyzer
				| FieldConfig.omitTf);
		writer.addField("signature", FieldConfig.multiField
				| FieldConfig.noIndex);

		// name == id
		writer.addField("univ.name", FieldConfig.whiteSpace
				| FieldConfig.omitTf);
		// info == name
		writer.addField("univ.info", FieldConfig.noIndex
				| FieldConfig.multiField | FieldConfig.omitTf);
		writer.addField("univ.year", FieldConfig.whiteSpace
				| FieldConfig.omitTf);
		writer.addField("univ.nameyear", FieldConfig.whiteSpace
				| FieldConfig.omitTf);

		writer.addField("high.name", FieldConfig.multiField
				| FieldConfig.whiteSpace | FieldConfig.omitTf);
		writer.addField("high.year", FieldConfig.whiteSpace
				| FieldConfig.omitTf);
		writer.addField("high.nameyear", FieldConfig.whiteSpace
				| FieldConfig.omitTf);

		writer.addField("junior.name", FieldConfig.multiField
				| FieldConfig.whiteSpace | FieldConfig.omitTf);
		writer.addField("junior.year", FieldConfig.whiteSpace
				| FieldConfig.omitTf);
		writer.addField("junior.nameyear", FieldConfig.whiteSpace
				| FieldConfig.omitTf);

		writer.addField("workplace", FieldConfig.multiField
				| FieldConfig.whiteSpace | FieldConfig.omitTf);
		writer.addField("age", FieldConfig.noAnalyzer | FieldConfig.omitTf);
		return writer;
	}

	static private ArrayBlockingQueue<ArrayList<UserInfo>> q = new ArrayBlockingQueue<ArrayList<UserInfo>>(
			200);

	static class TTThread extends Thread {
		private RDB rdb;
		private LinkedBlockingQueue<ArrayList<Object>> userQueue = new LinkedBlockingQueue<ArrayList<Object>>(
				10);
		private int num;

		public TTThread(String[] args, int i) throws IOException {
			RDB rdb = new RDB();
			rdb.open(new InetSocketAddress(args[1], 1981));
			rdb.setValueTranscoder(new ByteArrayTranscoder());
			rdb.iterinit();
			this.rdb = rdb;
			num = i;
		}

		public void put(ArrayList<Object> ids) throws InterruptedException {
			userQueue.put(ids);
		}

		public void run() {
			while (true) {
				TimeCost tc = TimeCost.begin("TTThread");
				ArrayList<Object> ids = null;
				try {
					ids = userQueue.take();
				} catch (InterruptedException e) {
					// TODO Auto-generated catch block
					e.printStackTrace();
				}
				if (ids.size() == 0) {
					System.out.println("TTRead Dead " + num);
					break;
				}
				Map<Object, Object> objs = null;
				try {
					objs = rdb.mget(ids.toArray(new Object[] {}));
				} catch (IOException e) {
					// TODO Auto-generated catch block
					e.printStackTrace();
				}
				if (objs == null) {
					continue;
				}
				ArrayList<UserInfo> infos = new ArrayList<UserInfo>(objs.size());
				for (Map.Entry<Object, Object> obj : objs.entrySet()) {
					try {
						byte[] bytes = (byte[]) obj.getValue();
						UserInfo info = UserInfoUnserializer.getInstance()
								.unserialize(bytes, true);
						if (info == null || info.browse_config != 100) {
							continue;
						}
						infos.add(info);

					} catch (Throwable ex) {
						ex.printStackTrace();
					}
				}
				tc.endStep("readTT");
				tc.endFinally();
				try {
					if (infos.size() > 0)
						q.put(infos);
				} catch (InterruptedException e) {
					// TODO Auto-generated catch block
					e.printStackTrace();
				}

			}
		}
	}

	static class AddThread extends Thread {
		private IndexWriter writer;
		private String[] args;

		public AddThread(IndexWriter writer, String[] args) {
			this.writer = writer;
			this.args = args;
		}

		public void run() {
			while (true) {
				try {
					TimeCost tc = TimeCost.begin("AddThread");
					ArrayList<UserInfo> infos = q.take();
					if (infos.size() == 0) {
						writer.stop();
						break;
					}
					for (UserInfo info : infos) {
						writer.addDocument(getDocument(info));
						ids.add(info.id);
					}
					tc.endStep("addDocument");
					tc.endFinally();
				} catch (Exception e) {
					// TODO Auto-generated catch block
					e.printStackTrace();
				}

			}
			SimpleFSIndexOutput output = null;
			try {
				output = new SimpleFSIndexOutput(new File(args[0] + "/docids"));
			} catch (IOException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
			try {
				output.writeInt(ids.size());
			} catch (IOException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
			for (Integer id : ids) {
				try {
					output.writeInt(id);
				} catch (IOException e) {
					// TODO Auto-generated catch block
					e.printStackTrace();
				}
			}
			try {
				output.close();
			} catch (IOException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
		}
	}

	public static void main(String[] args) throws Exception {
		// database_ = DatabaseFactory.getInstance().createDatabase();
		// database_.open("10.22.206.32 1981");
		// RDB rdb = new RDB();
		// rdb.open(new InetSocketAddress("s27.xce.d.xiaonei.com", 1981));
		// rdb.setValueTranscoder(new ByteArrayTranscoder());
		// rdb.iterinit();
		// Object value = rdb.get("user_" + 280281578);
		// byte[] bytes = (byte[]) value;
		// UserInfo info = UserInfoUnserializer.getInstance().unserialize(bytes,
		// true);
		// for (int i = 0; i < info.universities.size(); ++i)
		// System.out.println(info.universities.get(i));
		// System.out.println(StringEscapeUtils
		// .unescapeHtml("Queen&amp;&#35;39;s"));
		int suffix = Integer.valueOf(args[2]);
		IndexWriter writer = init(args);
		AddThread addThread = new AddThread(writer, args);
		addThread.start();
		int tsize = 6;
		TTThread[] readThread = new TTThread[tsize];
		for (int i = 0; i < tsize; ++i) {
			readThread[i] = new TTThread(args, i);
			readThread[i].start();
		}
		TimeCost total = TimeCost.begin("TOTAL");
		Class.forName("com.mysql.jdbc.Driver").newInstance();
		Connection con = java.sql.DriverManager.getConnection(
				"jdbc:mysql://user65.db.d.xiaonei.com:3306/fb", "work@RR",
				"Geeker4ZolZ");
		Statement stmt = con.createStatement();
		// Connection con = XceAdapter.getInstance().getReadConnection(
		// "user_passport_preloader");
		// Statement stmt = con.createStatement();
		int min = 0;
		int limit = 4000;
		int cnt = 0;
		while (true) {
			TimeCost tc = TimeCost.begin("produce");
			String sql = "select id from user_passport where id > " + min
					+ " and status = 0 limit 4000";
			ResultSet rs = stmt.executeQuery(sql);
			ArrayList<Object> ids = new ArrayList<Object>();
			int count = 0;
			int userid = 0;
			while (rs.next()) {
				userid = rs.getInt("id");
				if (userid % 4 == suffix) {
					ids.add(new String("user_" + userid));
				}
				count++;
			}
			System.out.println(sql);
			// pool.execute(new Task(ids));

			readThread[cnt % tsize].put(ids);
			min = userid;
			System.out.println(cnt);
			cnt++;
			if (args.length == 4 && cnt == Integer.valueOf(args[3])) {
				break;
			}
			if (count < limit) {
				break;
			}
			tc.endFinally();
			System.out.println("BlockingQueue : " + q.size());
		}
		total.endStep("READ DONE");
		for (int i = 0; i < tsize; ++i) {
			readThread[i].put(new ArrayList<Object>());
			System.out.println("ReadThread Join... " + i);
			readThread[i].join();
			System.out.println("ReadThread Done... " + i);
		}
		total.endStep("READ TTDone");
		q.put(new ArrayList<UserInfo>());
		addThread.join();
		total.endStep("AddDocument Done");
	}
}
