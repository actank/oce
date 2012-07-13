package search;

import java.io.IOException;
import java.io.StringReader;
import java.util.ArrayList;
import java.util.Map;

import org.apache.lucene.analysis.SimpleAnalyzer;
import org.apache.lucene.analysis.TokenStream;
import org.apache.lucene.analysis.tokenattributes.OffsetAttribute;
import org.apache.lucene.analysis.tokenattributes.TermAttribute;

public class ConditionToQuery {

	private int field_num = IndexReader.getFieldNum();
	private ArrayList<TermQuery> termQuerys = new ArrayList<TermQuery>();

	public ArrayList<TermQuery> getTermQuery(Map<String, String> condition) {
		for (Map.Entry<String, String> entry : condition.entrySet()) {
			if (entry.getKey() == "DEFAULT") {
				SimpleAnalyzer analyzer = new SimpleAnalyzer();
				TokenStream stream = null;
				try {
					stream = analyzer.reusableTokenStream("null",
							new StringReader(entry.getValue()));
				} catch (IOException e) {
					e.printStackTrace();
				}

				if (stream == null) {
					continue;
				}
				TermAttribute termAtt = (TermAttribute) stream
						.addAttribute(TermAttribute.class);
				OffsetAttribute offAtt = (OffsetAttribute) stream
						.addAttribute(OffsetAttribute.class);
				try {
					ArrayList<String> terms = new ArrayList<String>();
					while (stream.incrementToken()) {
						terms.add(termAtt.term());
						System.out.println(termAtt.term() + " "
								+ offAtt.startOffset() + " "
								+ offAtt.endOffset());
					}
					for (int i = 0; i < terms.size(); i++) {
						TermQuery termQuery = new TermQuery(field_num - 1,
								terms.get(i));
						termQuerys.add(termQuery);
					}
				} catch (IOException e) {
					e.printStackTrace();
				}
			} else {
				String key = entry.getKey();
				int id = getFieldNum(key);
				if (id >= 0) {
					TermQuery termQuery = new TermQuery(id, entry.getValue());
					termQuerys.add(termQuery);
				}
			}
		}
		return termQuerys;
	}

	public int getFieldNum(String fieldName) {
		for (int i = 0; i < IndexReader.fieldOptions.size(); i++) {
			String field_name = IndexReader.fieldOptions.get(i).getField_name();
			if (field_name.equals(fieldName)) {
				return IndexReader.fieldOptions.get(i).getField_id();
			}
		}
		return -1;
	}
}
