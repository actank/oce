package engine.index;

import java.io.IOException;
import java.io.StringReader;
import java.util.HashMap;
import java.util.Map.Entry;
import java.util.concurrent.ArrayBlockingQueue;

import net.paoding.analysis.analyzer.PaodingAnalyzer;

import org.apache.lucene.analysis.Analyzer;
import org.apache.lucene.analysis.TokenStream;
import org.apache.lucene.analysis.tokenattributes.TermAttribute;

import engine.common.FieldConfig;

public class FieldConsumer extends Thread {

	private static String white = "@###@";
	InvertedDocList postingLists;
	ArrayBlockingQueue<DocField> queue = new ArrayBlockingQueue<DocField>(1000);

	int field_num;
	int field_config;
	Analyzer analyzer = new PaodingAnalyzer();

	public FieldConsumer(int field_num, int field_config, String path) {
		this.field_num = field_num;
		this.field_config = field_config;
		postingLists = new InvertedDocList(field_num, field_config, path);

	}

	public void addDocument(DocField doc) {
		try {
			if (doc.doc == -1
					|| (doc.field.getValue() != null && doc.field.getValue()
							.length() > 0)) {
				queue.put(doc);
			}
		} catch (InterruptedException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
	}

	public void consumer(DocField doc) {
		// System.out.println(doc.doc+" "+doc.field.getValue());
		if (FieldConfig.isNoAnalyzer(field_config)) {
			postingLists.addTermDoc(doc.field.getValue(), doc.doc, 1, null);
		} else if (FieldConfig.isWhitespace(field_config)) {
			HashMap<String, Integer> terms = new HashMap<String, Integer>();
			String[] values = doc.field.getValue().split(white);
			for (String value : values) {
				Integer cnt = terms.get(value);
				if (cnt == null) {
					cnt = new Integer(1);
					terms.put(value, cnt);
				} else {
					terms.put(value, cnt + 1);
				}
				// System.out.println(termAtt.term() + " "
				// + offAtt.startOffset() + " " + offAtt.endOffset());
			}
			for (Entry<String, Integer> entry : terms.entrySet()) {
				postingLists.addTermDoc(entry.getKey(), doc.doc,
						entry.getValue(), null);
			}
		} else if (FieldConfig.usePaoding(field_config)) {

			TokenStream stream = null;
			try {
				stream = analyzer.reusableTokenStream("null", new StringReader(
						doc.field.getValue()));
			} catch (IOException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}

			if (stream == null)
				return;
			TermAttribute termAtt = (TermAttribute) stream
					.addAttribute(TermAttribute.class);
			// OffsetAttribute offAtt = (OffsetAttribute) stream
			// .addAttribute(OffsetAttribute.class);
			try {
				HashMap<String, Integer> terms = new HashMap<String, Integer>();
				while (stream.incrementToken()) {
					Integer cnt = terms.get(termAtt.term());
					if (cnt == null) {
						cnt = new Integer(1);
						terms.put(termAtt.term(), cnt);
					} else {
						terms.put(termAtt.term(), cnt + 1);
					}
					// System.out.println(termAtt.term() + " "
					// + offAtt.startOffset() + " " + offAtt.endOffset());
				}
				for (Entry<String, Integer> entry : terms.entrySet()) {
					postingLists.addTermDoc(entry.getKey(), doc.doc,
							entry.getValue(), null);
				}
				// postingLists.addTermDoc(termAtt.term(), doc.getDoc(), tf,
				// position)

			} catch (IOException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
		}
	}

	public void run() {
		while (true) {
			// System.out.println(field_num + " " + "running");
			DocField doc = null;
			try {
				doc = queue.take();
			} catch (InterruptedException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
			if (doc == null) {
				continue;
			}
			if (doc.doc != -1) {
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
}
