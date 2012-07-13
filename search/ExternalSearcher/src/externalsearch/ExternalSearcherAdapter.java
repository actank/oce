package externalsearch;

import java.util.HashMap;
import java.util.Map;
import java.util.Vector;

import xce.util.channel.Channel;
import xce.util.service.ServiceAdapter;

import com.xiaonei.search2.SearchResult;
import com.xiaonei.search2.SearchViewFactory;
import com.xiaonei.search2.condition.AbstractCondition;
import com.xiaonei.search2.model.IndexCondition;
import com.xiaonei.search2.model.IndexResult;
import com.xiaonei.search2.model.IndexResultItem;
import com.xiaonei.search2.model.IndexType;

public class ExternalSearcherAdapter extends ServiceAdapter {

	private static ExternalSearcherAdapter _instance = new ExternalSearcherAdapter();

	public static ExternalSearcherAdapter getInstance() {
		return _instance;
	}

	private ExternalSearcherAdapter() {
		super("@ExternalSearcher", 1, Channel
				.newSearchChannel("ExternalSearcher"));
	}

	/*-- 以下同时列出了各同的代理调用方式, 可根据实际需要保留和删除 --*/
	private Vector<ExternalSearcherPrx> managers = new Vector<ExternalSearcherPrx>();

	private ExternalSearcherPrx getExternalSearcher(int userId) {
		return locateProxy(managers, "M", userId, Channel.Invoke.Twoway,
				ExternalSearcherPrxHelper.class, 5000);
	}

	public IndexResult search(Map<String, String> condition, int begin,
			int limit) {

		IndexResult resultIds = new IndexResult();
		resultIds.currentPos = 0;
		resultIds.timeCost = 0;
		resultIds.totalCount = 0;
		resultIds.contents = new IndexResultItem[0];
		resultIds.typedCount = new HashMap<IndexType, Integer>();
		resultIds = getExternalSearcher(0).search(condition, begin, limit);

		return resultIds;
	}

	public SearchResult search(AbstractCondition condition,
			Map<IndexType, SearchViewFactory> factories, int begin, int limit) {

		IndexCondition ic = condition.buildIndexCondition();

		Map<String, String> query = ic.queryCondition;

		IndexResult resultIds = new IndexResult();
		resultIds.currentPos = 0;
		resultIds.timeCost = 0;
		resultIds.totalCount = 0;
		resultIds.contents = new IndexResultItem[0];
		resultIds.typedCount = new HashMap<IndexType, Integer>();
		resultIds = getExternalSearcher(0).search(query, begin, limit);

		SearchResult result = null;
		result = new SearchResult(resultIds, factories);
		return result;
	}

	public static void main(String[] args) {
		Map<String, String> q = new HashMap<String, String>();
		q.put("caller", "yao.hu.0");
		q.put("query", "谭博");
		q.put("GENERAL|gender", "男生");
		System.out.println(ExternalSearcherAdapter.getInstance().search(q, 0,
				10).totalCount);
	}

}
