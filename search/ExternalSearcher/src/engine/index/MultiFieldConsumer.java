package engine.index;

import java.io.IOException;
import java.io.StringReader;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.HashSet;
import java.util.Map.Entry;
import java.util.concurrent.ArrayBlockingQueue;
import java.util.concurrent.LinkedBlockingQueue;
import java.util.concurrent.TimeUnit;

import net.paoding.analysis.analyzer.PaodingAnalyzer;

import org.apache.lucene.analysis.SimpleAnalyzer;
import org.apache.lucene.analysis.TokenStream;
import org.apache.lucene.analysis.tokenattributes.TermAttribute;

import engine.common.FieldConfig;

public class MultiFieldConsumer extends Thread {
	InvertedDocList postingLists;
	ArrayBlockingQueue<Document> queue = new ArrayBlockingQueue<Document>(5000);

	int field_num;

	private int[] field_configs;
	SimpleAnalyzer simple = new SimpleAnalyzer();
	PaodingAnalyzer paoding = new PaodingAnalyzer();

	private int field_config;

	public MultiFieldConsumer(int field_num, int field_config, String path) {
		this.field_num = field_num;
		this.field_config = field_config;
		postingLists = new InvertedDocList(field_num, field_config, path);
	}

	public void addDocument(Document doc) {
		try {
			queue.put(doc);
		} catch (InterruptedException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
	}

	public HashSet<String> paoding(String term) {
		HashSet<String> s = new HashSet<String>();
		TokenStream stream = null;

		try {

			stream = paoding
					.reusableTokenStream("null", new StringReader(term));
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		s.add(term);
		if (stream == null)
			return s;
		TermAttribute termAtt = (TermAttribute) stream
				.addAttribute(TermAttribute.class);

		try {

			while (stream.incrementToken()) {
				s.add(termAtt.term());
			}

		} catch (IOException e) {
			e.printStackTrace();
		}
		return s;
	}

	public void consumer(Document doc) {

		HashMap<String, Integer> terms = new HashMap<String, Integer>();

		for (int i = 0; i < field_num; ++i) {
			if (FieldConfig.isMultiField(field_configs[i])) {
				TokenStream stream = null;
				try {
					stream = paoding.reusableTokenStream("null",
							new StringReader(doc.getField(i).getValue()));
				} catch (IOException e) {
					// TODO Auto-generated catch block
					e.printStackTrace();
				}

				if (stream == null)
					return;
				TermAttribute termAtt = (TermAttribute) stream
						.addAttribute(TermAttribute.class);

				try {
					while (stream.incrementToken()) {
						Integer cnt = terms.get(termAtt.term());
						if (cnt == null) {
							cnt = new Integer((1 << i));
							terms.put(termAtt.term(), cnt);
						} else {
							terms.put(termAtt.term(), cnt | (1 << i));
						}
					}

				} catch (IOException e) {
					e.printStackTrace();
				}
			}

		}
		for (Entry<String, Integer> entry : terms.entrySet()) {
			postingLists.addTermDoc(entry.getKey(), doc.getDoc(),
					entry.getValue(), null);

		}
	}

	public void run() {
		while (true) {
			Document doc = null;
			try {
				doc = queue.poll(1, TimeUnit.SECONDS);
			} catch (InterruptedException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
			if (doc == null) {
				continue;
			}
			if (doc.getDoc() != -1) {
				consumer(doc);
			} else {
				try {
					postingLists.writeTermDocs(field_config);
				} catch (IOException e) {
					// TODO Auto-generated catch block
					e.printStackTrace();
				}
				break;
			}
		}
	}

	public void setFieldConfig(int[] field_configs) {
		// TODO Auto-generated method stub

		this.field_configs = field_configs;
	}
}
