
import org.apache.lucene.search.Filter;
import org.apache.lucene.search.Searchable;
import org.apache.lucene.search.TopDocs;
import org.apache.lucene.search.Weight;
import org.apache.lucene.document.Document;

import java.io.IOException;
import java.util.List;

/** 所有搜索类需要实现的接口 **/
public interface RenrenSearchable extends Searchable {
	public Document[] docidsToDocuments(int[] docids) throws IOException;

	public int[] docidsToIDs(int[] docids) throws IOException;

	public TopDocs search(Weight weight, Filter filter, int nDocs,
			List<Integer> n, List<Integer> m) throws IOException;

}
