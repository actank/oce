package Test;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileReader;
import java.io.IOException;
import java.util.HashMap;
import java.util.Map;

import com.xiaonei.search2.SearchResult;
import com.xiaonei.search2.SearchViewFactory;
import com.xiaonei.search2.condition.OpenSearchCondition;
import com.xiaonei.search2.model.IndexType;
import com.xiaonei.search2.sample.AppPlainWebViewFactory;
import com.xiaonei.search2.sample.PagePlainWebViewFactory;
import com.xiaonei.search2.sample.UserPlainWebViewFactory;

import externalsearch.ExternalSearcherAdapter;

public class QuerySearchTest {
	public static void main(String[] args) throws IOException {

		BufferedReader reader = new BufferedReader(new FileReader(new File(
				"/home/dong/externalnew.log")));
		String line;
		while ((line = reader.readLine()) != null) {

			OpenSearchCondition condition = new OpenSearchCondition();

			condition.queryCondition.putQueryMap("query", line);
			condition.addOpUser(0);
			condition.setCaller("XXX");

			Map<IndexType, SearchViewFactory> factories = new HashMap<IndexType, SearchViewFactory>();
			factories
					.put(IndexType.User, UserPlainWebViewFactory.getInstance());
			factories.put(IndexType.App, AppPlainWebViewFactory.getInstance());
			factories
					.put(IndexType.Page, PagePlainWebViewFactory.getInstance());
			long start = System.currentTimeMillis();
			SearchResult result = ExternalSearcherAdapter.getInstance().search(
					condition, factories, 10, 10);
			long end = System.currentTimeMillis();
//
//			System.out
//					.println("query:" + line + "; result.size():"
//							+ result.getTotalCount() + " ;cost:"
//							+ (end - start) + "ms");
		}
		reader.close();
		System.exit(0);
	}
}
