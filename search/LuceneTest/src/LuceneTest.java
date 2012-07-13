import java.io.File;
import java.io.IOException;
import java.io.PrintStream;
import java.io.RandomAccessFile;
import java.io.Reader;
import java.nio.MappedByteBuffer;
import java.nio.channels.FileChannel.MapMode;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.Map;
import java.util.Scanner;

import javax.swing.text.html.HTMLDocument.HTMLReader.IsindexAction;

import net.paoding.analysis.analyzer.PaodingAnalyzer;

import org.apache.lucene.analysis.TokenStream;
import org.apache.lucene.analysis.standard.StandardAnalyzer;
import org.apache.lucene.document.Document;
import org.apache.lucene.document.Field;
import org.apache.lucene.document.Field.Index;
import org.apache.lucene.document.Field.Store;
import org.apache.lucene.document.Fieldable;
import org.apache.lucene.index.CorruptIndexException;
import org.apache.lucene.index.IndexReader;
import org.apache.lucene.index.IndexWriter;
import org.apache.lucene.index.ParallelReader;
import org.apache.lucene.index.IndexWriter.MaxFieldLength;
import org.apache.lucene.index.Term;
import org.apache.lucene.queryParser.MultiFieldQueryParser;
import org.apache.lucene.queryParser.ParseException;
import org.apache.lucene.queryParser.QueryParser;
import org.apache.lucene.search.BooleanClause;
import org.apache.lucene.search.BooleanQuery;
import org.apache.lucene.search.IndexSearcher;
import org.apache.lucene.search.PhraseQuery;
import org.apache.lucene.search.Query;
import org.apache.lucene.search.ScoreDoc;
import org.apache.lucene.search.TermQuery;
import org.apache.lucene.search.TopDocs;
import org.apache.lucene.search.BooleanClause.Occur;
import org.apache.lucene.store.Directory;
import org.apache.lucene.store.FSDirectory;
import org.apache.lucene.store.LockObtainFailedException;
import org.apache.lucene.store.MMapDirectory;
import org.apache.lucene.store.RAMDirectory;
import org.apache.lucene.util.Version;

import com.xiaonei.xce.log.TimeCost;
import com.xiaonei.xce.username.DefaultUserNameFactory;
import com.xiaonei.xce.username.DefaultUserNameInfo;
import com.xiaonei.xce.username.UserNameAdapter;
import com.xiaonei.xce.username.UserNameFactory;
import com.xiaonei.xce.username.UserNameInfo;

public class LuceneTest {
	static void printResult(TopDocs docs) {
		try {
			System.out.println("get result totalHits : " + docs.totalHits);
			UserNameAdapter<DefaultUserNameInfo> adapter = new UserNameAdapter<DefaultUserNameInfo>(
					new DefaultUserNameFactory());
			for (ScoreDoc doc : docs.scoreDocs) {
				System.out.println("id: " + doc.doc + " score: " + doc.score);
			}
		} catch (Exception ex) {
			ex.printStackTrace();
		}
	}

	public static void main(String[] args) throws Exception {

		int Cluster = 8;
		if (args.length != 1) {
			System.out.println("please input index directory");
			System.exit(0);
		}
		File file = new File(args[0]);
		boolean useRAM_ = false;

		String[] arr = file.getName().split("_");
		int suffix = Integer.valueOf(arr[1]);
		ParallelReader reader = new ParallelReader();
		if (useRAM_) {
			reader.add(IndexReader.open(
					new RAMDirectory(FSDirectory.open(new File(args[0]
							+ "/general"))), true));
			reader.add(IndexReader.open(
					new RAMDirectory(FSDirectory.open(new File(args[0]
							+ "/network"))), true));
			reader.add(IndexReader.open(
					new RAMDirectory(FSDirectory.open(new File(args[0]
							+ "/info"))), true));
		} else {
			reader.add(IndexReader.open(
					MMapDirectory.open(new File(args[0] + "/general")), true));
			reader.add(IndexReader.open(
					MMapDirectory.open(new File(args[0] + "/network")), true));
			reader.add(IndexReader.open(
					MMapDirectory.open(new File(args[0] + "/info")), true));
		}
		// System.out.println("reader load done");
		UserSearcher searcher = new UserSearcher(reader, args[0] + "/docid");
		System.out.println("load index done ");
		System.out.println("input you userid : ");
		Scanner scanner = new Scanner(System.in);
		int userid = 220840375;
		if (scanner.hasNext())
			userid = Integer.valueOf(scanner.nextLine());
		String mode = "relation";
		// if(scanner.hasNext())
		// mode = scanner.nextLine();
		// System.out.println(userid + " " + mode);
		System.out.print("input query with " + mode + " search: ");
		while (scanner.hasNext()) {
			String query = scanner.nextLine();
			// System.out.println(query);
			if (query.equals("relation") || query.equals("norelation")) {
				mode = query;
				System.out.print("input query with " + mode + " search: ");
				continue;
			}
			ArrayList<Integer> fids = new ArrayList<Integer>();
			ArrayList<Integer> commons = new ArrayList<Integer>();
			QueryParser parser = new MultiFieldQueryParser(
					Version.LUCENE_CURRENT, new String[] { "name",
							"university.university_name",
							"high_school.high_school_name" },
					new PaodingAnalyzer());
			Query q = parser.parse(query);
			TopDocs docs;
			//TimeCost tc = TimeCost.begin("search");
			if (mode.equals("relation")) {
				FriendUtil.getFriendOfFriend(userid, fids, commons);
				//tc.endStep("get relation");
				System.out.println("get relation " + userid + " total: "
						+ fids.size() + " " + commons.size());
				ArrayList<ArrayList<Integer>> suffixFids = new ArrayList<ArrayList<Integer>>();
				ArrayList<ArrayList<Integer>> suffixCommons = new ArrayList<ArrayList<Integer>>();
				for (int i = 0; i < Cluster; ++i) {
					suffixFids.add(new ArrayList<Integer>());
					suffixCommons.add(new ArrayList<Integer>());
				}
				for (int i = 0; i < fids.size(); ++i) {
					int n = fids.get(i) % Cluster;
					suffixFids.get(n).add(fids.get(i));
					suffixCommons.get(n).add(commons.get(i));
				}
				//tc.endStep("group by suffix");
				System.out.println("start relation search : " + suffix + " "
						+ suffixFids.get(suffix).size());
				docs = searcher.search(q.weight(searcher), null, 10,
						suffixFids.get(suffix), suffixCommons.get(suffix));
				//tc.endStep("search and merge");
			} else {
				docs = searcher.search(q.weight(searcher), null, 10);
				//tc.endStep("seach done");
			}
			printResult(docs);
			System.out.print("input query with " + mode + " search: ");
		}
		// FSDirectory dir = FSDirectory.open(new File(
		// "/home/tanbokan/index/A"));
		// RAMDirectory dir2 = new RAMDirectory(dir);
		// System.out.println(dir2.toString());
		// IndexReader reader = IndexReader.open(dir);
		// reader.clone();
		// Field field ;
		// IndexWriter write = null;
		// String[] files = dir.listAll();
		// for (String file : files) {
		// File f = new File("/home/tanbokan/index/User_0/general", file);
		// RandomAccessFile raf = new RandomAccessFile(f, "r");
		// MappedByteBuffer buf = raf.getChannel().map(MapMode.READ_ONLY, 0,
		// f.length());
		// System.out.println(buf.load());
		// }
		// String qq =
		// "+(+(+((name:孟繁玲^604.5 splitname:\"孟 繁 玲\"~15^70.0 signature:孟繁玲^3.0 splitsignature:\"孟 繁 玲\"~15^2.0) (university.university_name:\"孟繁 繁玲\"^32.0) (high_school.high_school_name:\"孟繁 繁玲\"^41.3 junior_high_school_info.junior_high_school_name:\"孟繁 繁玲\"^20.1 college_info.college_name:\"孟繁 繁玲\"^3.1 elementary_school_info.elementary_school_name:\"孟繁 繁玲\"^9.8 workplace_name:\"孟繁 繁玲\"^11.4 network_name:\"孟繁 繁玲\"^140.0 interest.interest:\"孟繁 繁玲\"^3.0 book.book:\"孟繁 繁玲\"^3.0 comic.comic:\"孟繁 繁玲\"^2.0 game.game:\"孟繁 繁玲\"^2.0 movie.movie:\"孟繁 繁玲\"^2.0"
		// +
		// " music.music:\"孟繁 繁玲\"^3.0 society.society:\"孟繁 繁玲\"^5.8 sport.sport:\"孟繁 繁玲\"^2.0)))) ((((+user_basic.id:282999695 +(name:magic blue splitname:\"magic blue\"))^100.0) ((+user_basic.id:282999695 +(name:magicblue splitname:magicblue))^100.0))^1000.0) (home_province:天津^275.0 home_city:\"塘沽 塘沽区\"^143.0 ((elementary_school_info.elementary_school_name:\"塘沽 一中 中心 小 小学\"^8.4 elementary_school_info.elementary_school_year:1998^26.0 ((+elementary_school_info.elementary_school_name:\"塘沽 一中 中心 小 小学\" +elementary_school_info.elementary_school_year:1998)^26.0))) ((college_info.college_id:80000799^2.0)) ((university.university_id:6116^2.0 university.enroll_year:2010^534.0 ((+university.university_id:6116 +university.enroll_year:2010)^534.0) ((+university.university_id:6116 +university.department:\"工学 学院\")^150.6)))"
		// +
		// " ((junior_high_school_info.junior_high_school_id:40015025^2.5 junior_high_school_info.junior_high_school_year:2004^59.3 ((+junior_high_school_info.junior_high_school_id:40015025 +junior_high_school_info.junior_high_school_year:2004)^59.3))));";
		// QueryParser parser = new QueryParser(Version.LUCENE_CURRENT, "id",
		// new StandardAnalyzer(Version.LUCENE_CURRENT));
		// Query query = parser.parse("ooxx:\"fuck all\"" );
		// BooleanQuery q = new BooleanQuery();
		// TermQuery term = new TermQuery(new Term("ooxx","fuck"));
		// //term.setBoost(100f);
		// q.add(term,Occur.SHOULD);
		// q.add(new TermQuery(new Term("ooxx","fuck")),Occur.SHOULD);
		// System.out.println(query instanceof PhraseQuery);
		// System.out.println(query.toString());
		// System.out.println(q.toString());
		// System.out.println(query.toString());
		// IndexReader reader = IndexReader.open(dir, true);
		// IndexSearcher searcher = new IndexSearcher(reader);
		// TermQuery term = new TermQuery(new Term("name", "王伟"));
		// TopDocs docs = searcher.search(term, 100);
	}
}
