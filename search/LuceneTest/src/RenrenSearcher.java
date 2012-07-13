import java.io.IOException;
import java.util.List;

import org.apache.lucene.document.Document;
import org.apache.lucene.index.CorruptIndexException;
import org.apache.lucene.index.IndexReader;
import org.apache.lucene.search.Filter;
import org.apache.lucene.search.IndexSearcher;
import org.apache.lucene.search.TopDocs;
import org.apache.lucene.search.Weight;

/** 搜索类的抽象类,子类来实现接口RenrenSearchable的两个方法 **/
public abstract class RenrenSearcher extends IndexSearcher implements
		RenrenSearchable {

	/** 构造方法 **/
	public RenrenSearcher(IndexReader reader) throws CorruptIndexException,
			IOException {
		super(reader);
	}

	public abstract Document[] docidsToDocuments(int[] docids)
			throws IOException;

	public abstract int[] docidsToIDs(int[] docids) throws IOException;

	public abstract TopDocs search(Weight weight, Filter filter, int nDocs,
			List<Integer> n, List<Integer> m) throws IOException;
}
