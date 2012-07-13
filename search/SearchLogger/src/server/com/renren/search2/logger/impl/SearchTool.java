package com.renren.search2.logger.impl;

import java.io.IOException;
import java.util.Map;

import org.apache.lucene.index.Term;
import org.apache.lucene.search.BooleanClause.Occur;
import org.apache.lucene.search.BooleanQuery;
import org.apache.lucene.search.Explanation;
import org.apache.lucene.search.ScoreDoc;
import org.apache.lucene.search.Sort;
import org.apache.lucene.search.TermQuery;
import org.apache.lucene.search.TopDocs;

import com.renren.search2.logger.searcher.SearcherPrivatePool;
import com.xiaonei.search2.model.GroupType;
import com.xiaonei.search2.model.IndexCondition;
import com.xiaonei.search2.model.UserFieldNames;
import com.xiaonei.search2.search.RenrenParallelMultiSearcher;
import com.xiaonei.search2.server.processor.CollegeProcessor;
import com.xiaonei.search2.server.processor.ElementarySchoolProcessor;
import com.xiaonei.search2.server.processor.GeneralProcessor;
import com.xiaonei.search2.server.processor.HighSchoolProcessor;
import com.xiaonei.search2.server.processor.JuniorSchoolProcessor;
import com.xiaonei.search2.server.processor.MapProcessor;
import com.xiaonei.search2.server.processor.RegionProcessor;
import com.xiaonei.search2.server.processor.UniversityProcessor;
import com.xiaonei.search2.server.processor.WorkplaceProcessor;
import com.xiaonei.search2.util.AdUserBoost;
import com.xiaonei.search2.util.BoostFactors;
import com.xiaonei.search2.util.OccurFactors;

public class SearchTool {
	// 查询的processor
	private static Map<String, MapProcessor.Processor> map_processor_query_;
	// 用户资料的processor
	private static Map<String, MapProcessor.Processor> map_processor_user_;

	private static final BoostFactors boost_factors_;
	private static final OccurFactors occur_factors_must_;
	private static final OccurFactors occur_factors_should_;

	static {
		GeneralProcessor general_query = new GeneralProcessor();
		GeneralProcessor general_user = new GeneralProcessor();

		general_query.addProcessor(new GeneralProcessor.QueryProcessor(1.0f,
				Occur.MUST));
		map_processor_query_ = MapProcessor.getInstance().buildProcessorMap(
				new MapProcessor.Processor[] { general_query });

		general_user.addProcessor(new GeneralProcessor.HomeProvinceProcessor(
				1.0f, Occur.SHOULD), new GeneralProcessor.HomeCityProcessor(
				1.0f, Occur.SHOULD));
		map_processor_user_ = MapProcessor.getInstance().buildProcessorMap(
				new MapProcessor.Processor[] { general_user,
						new HighSchoolProcessor(1.0f, Occur.SHOULD),
						new UniversityProcessor(1.0f, Occur.SHOULD),
						new WorkplaceProcessor(1.0f, Occur.SHOULD),
						new RegionProcessor(1.0f, Occur.SHOULD),
						new ElementarySchoolProcessor(1.0f, Occur.SHOULD),
						new JuniorSchoolProcessor(1.0f, Occur.SHOULD),
						new CollegeProcessor(1.0f, Occur.SHOULD) });

		boost_factors_ = new BoostFactors.Builder().build();
		occur_factors_must_ = new OccurFactors.Builder(Occur.MUST).build();
		occur_factors_should_ = new OccurFactors.Builder(Occur.SHOULD).build();
	}

	public static Explanation explain(IndexCondition condition, int friendId) {
		Explanation explanation = null;
		// 处理User
		BooleanQuery query = new BooleanQuery(true);
		BooleanQuery query_query = new BooleanQuery();
		// 处理query
		MapProcessor.getInstance().process(query_query,
				condition.queryCondition, map_processor_query_, boost_factors_,
				occur_factors_must_);
		query.add(query_query, Occur.MUST);
		// 处理user资料
		BooleanQuery query_user = new BooleanQuery();
		MapProcessor.getInstance().process(query_user,
				condition.opUserCondition, map_processor_user_, boost_factors_,
				occur_factors_should_);
		query.add(AdUserBoost.getInstance().getQuery(), Occur.SHOULD);
		if (query_user.clauses().isEmpty()) {
			// filter queries from out-of-site searching
			query.add(new TermQuery(new Term(UserFieldNames.BROWSE_CONFIG,
					"100")), Occur.MUST);
		} else {
			query.add(query_user, Occur.SHOULD);
		}

		RenrenParallelMultiSearcher searcher_user;
		searcher_user = SearcherPrivatePool.getPool(GroupType.GroupSearch)
				.getUsersearcher();
		try {
			System.out.println("searcher_user : " + searcher_user.maxDoc());
		} catch (IOException e1) {
			e1.printStackTrace();
		}
		int docId = 0;
		try {
			if (!query.clauses().isEmpty()) {
				TopDocs docs;
				// System.out.println("开始搜索");
				docs = searcher_user.search(query, null, 500, new Sort());
				// System.out.println("搜索结束");
				for (ScoreDoc scoreDoc : docs.scoreDocs) {
					if (searcher_user.doc(scoreDoc.doc).get(UserFieldNames.ID)
							.equals(String.valueOf(friendId))) {
						docId = scoreDoc.doc;
						break;
					}
				}
				// System.out.println("开始explain");
				explanation = searcher_user.explain(query, docId);
				// System.out.println("结束explain");
				return explanation;
			}
		} catch (IOException e) {
			e.printStackTrace();
			return null;
		}
		return null;
	}

}
