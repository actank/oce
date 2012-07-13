package engine.search;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.IOException;
import java.io.StringReader;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

import net.paoding.analysis.analyzer.PaodingAnalyzer;

import org.apache.lucene.analysis.Analyzer;
import org.apache.lucene.analysis.SimpleAnalyzer;
import org.apache.lucene.analysis.TokenStream;
import org.apache.lucene.analysis.tokenattributes.OffsetAttribute;
import org.apache.lucene.analysis.tokenattributes.TermAttribute;

import engine.search.BooleanClause.Occur;

public class ConditionToQuery {

	private ArrayList<FieldOption> fieldOptions;

	private int field_num;
	// private ArrayList<Term> terms = new ArrayList<Term>();
	// Analyzer analyzer = new PaodingAnalyzer();
	Analyzer analyzer = new SimpleAnalyzer();
	private static HashMap<String, ArrayList<String>> university_alias = new HashMap<String, ArrayList<String>>();

	static {
		try {
			BufferedReader reader = new BufferedReader(new FileReader(new File(
					"/home/dong/index2/university_alias.txt")));
			String line;

			while ((line = reader.readLine()) != null) {
				// System.out.println("LINE::::" + line);
				String[] splits = line.split("\\t");
				List<String> res = new ArrayList<String>();
				for (String split : splits) {
					String s = split.trim();
					if (!s.equals("")) {
						res.add(s);
					}
				}
				if (res.size() >= 2) {
					for (int i = 1; i < res.size(); ++i) {
						ArrayList<String> alias = university_alias.get(res
								.get(i));
						// System.out.println("res.get+" + i + "+ " + res.get(i)
						// + " res.get(0):" + res.get(0));
						if (alias == null) {
							alias = new ArrayList<String>();
							alias.add(res.get(0));
							university_alias.put(res.get(i), alias);
						} else {
							alias.add(res.get(0));
						}
					}
				}
			}
		} catch (FileNotFoundException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
	}

	public ConditionToQuery(IndexReader reader) {
		this.fieldOptions = reader.getFieldOptions();
		this.field_num = reader.getFieldNum();
	}

	public BooleanQuery getQuery(Map<String, String> condition)
			throws IOException {
		BooleanQuery booleanQuery = new BooleanQuery();
		for (Map.Entry<String, String> entry : condition.entrySet()) {
			if (entry.getKey() == "DEFAULT") {
				int id = getFieldNum("DEFAULT");
				String[] querys = entry.getValue().split(" ");
				for (String query : querys) {
					System.out.println("query:" + query);
					query = query.trim();
					if (query.equals(""))
						continue;
					ArrayList<String> alias;
					if ((alias = university_alias.get(query)) != null) {
						if (!alias.isEmpty()) {
							System.out.println("query:" + query
									+ " university_alias.size:"
									+ university_alias.size() + " alias.size:"
									+ alias.size());
							int univ = getFieldNum("univ.name");
							Term term = new Term(univ, alias.get(0));
							TermQuery termQuery = new TermQuery(term);
							booleanQuery.add(termQuery, Occur.MUST);
							// terms.add(term);
						}
						continue;
					}
					TokenStream stream = null;
					try {
						stream = analyzer.reusableTokenStream("null",
								new StringReader(query));
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
						ArrayList<String> termToken = new ArrayList<String>();
						while (stream.incrementToken()) {
							termToken.add(termAtt.term());
							System.out.println(termAtt.term() + " "
									+ offAtt.startOffset() + " "
									+ offAtt.endOffset());
						}
						for (int i = 0; i < termToken.size(); i++) {
							Term term = new Term(id, termToken.get(i));
							TermQuery termQuery = new TermQuery(term);
							booleanQuery.add(termQuery, Occur.MUST);
							// terms.add(term);
						}
					} catch (IOException e) {
						e.printStackTrace();
					}
				}
			} else {
				String key = entry.getKey();
				int id = getFieldNum(key);
				if (id >= 0) {
					Term term = new Term(id, entry.getValue());
					TermQuery termQuery = new TermQuery(term);
					booleanQuery.add(termQuery, Occur.MUST);
					// terms.add(term);
				}
			}
		}
		return booleanQuery;
	}

	public int getFieldNum(String fieldName) {
		for (int i = 0; i < fieldOptions.size(); i++) {
			String field_name = fieldOptions.get(i).getField_name();
			if (field_name.equals(fieldName)) {
				return fieldOptions.get(i).getField_id();
			}
		}
		return -1;
	}

}
