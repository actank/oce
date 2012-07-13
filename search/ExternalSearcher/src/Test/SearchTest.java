package Test;

import java.io.IOException;
import java.util.HashMap;
import java.util.Map;

import com.xiaonei.search2.SearchResult;
import com.xiaonei.search2.SearchViewFactory;
import com.xiaonei.search2.condition.OpenSearchCondition;
import com.xiaonei.search2.model.IndexCondition;
import com.xiaonei.search2.model.IndexResult;
import com.xiaonei.search2.model.IndexType;
import com.xiaonei.search2.sample.AppPlainWebViewFactory;
import com.xiaonei.search2.sample.PagePlainWebViewFactory;
import com.xiaonei.search2.sample.UserPlainWebViewFactory;

import externalsearch.ExternalSearcherAdapter;
import externalsearch.ExternalSearcherI;

public class SearchTest {
	public static void main(String[] args) throws IOException {
		// if (args.length % 2 != 0) {
		// System.out.println("ERROR args");
		// System.out.println("usage: SearchTest field word [field word]");
		// }
		// for (int i = 0; i < 10; i++) {
		// Map<String, String> condition = new HashMap<String, String>();
		OpenSearchCondition condition = new OpenSearchCondition();
		// condition.put("gender", "女生");
		// condition.put("univ.name", "北京大学");
		// condition.put("homecity", "长春市");
		// condition.put("homeprovince", "吉林");
		// condition.put("DEFAULT", "北京");
		// condition.put("name", "姚雨馨");
		// condition.put("univ.name", "西北工业大学");
		// condition.queryCondition.putQueryMap("query", "东北大学 沈阳市");
		// condition.queryCondition.putQueryMap("query", "北京");
		condition.queryCondition.putQueryMap("GENERAL|home_province", "宁夏");
//		condition.queryCondition.putQueryMap("GENERAL|home_city", "石嘴山市");
		condition.addOpUser(0);
		condition.setCaller("XXX");

		// System.out.println("加载索引");
		// IndexReader reader = new IndexReader("/home/dong/index2");
		// reader.readField();
		// reader.readIndex();
		// reader.readFrequency();
		// reader.readIds();

		// ConditionToQuery termQuery = new ConditionToQuery();
		//
		// ArrayList<TermQuery> query = termQuery.getTermQuery(condition);
		// IndexSearcher searcher = new IndexSearcher(query);
		// List<DocTf> docTf = searcher.getUnion();
		// int[] ids = reader.mappingId(docTf);

		// System.out.println("result.size():" + docTf.size());
		Map<IndexType, SearchViewFactory> factories = new HashMap<IndexType, SearchViewFactory>();
		factories.put(IndexType.User, UserPlainWebViewFactory.getInstance());
		factories.put(IndexType.App, AppPlainWebViewFactory.getInstance());
		factories.put(IndexType.Page, PagePlainWebViewFactory.getInstance());
		long start = System.currentTimeMillis();
		// SearchResult result = ExternalSearcherAdapter.getInstance().search(
		// condition, factories, 10, 10);

		IndexCondition ic = condition.buildIndexCondition();

		Map<String, String> query = ic.queryCondition;

		ExternalSearcherI searcher = new ExternalSearcherI();
		IndexResult result = searcher.search(query, 0, 40);
		long end = System.currentTimeMillis();

		// for (DocTf dtf : docTf) {
		// System.out.println(dtf.toString());
		// }

		System.out.println("result.size():" + result.totalCount);
		System.out.println("cost:" + (end - start));
		for (int i = 0; i < result.totalCount; i++) {
			System.out.println(result.contents[i].toString());
		}
		// for (int id : ids) {
		// System.out.println(id);
		// }
	}
	// }
}
