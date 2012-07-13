package index;

import java.io.IOException;
import java.io.StringReader;
import java.util.HashMap;
import java.util.Map.Entry;
import java.util.concurrent.LinkedBlockingQueue;

import org.apache.lucene.analysis.SimpleAnalyzer;
import org.apache.lucene.analysis.TokenStream;
import org.apache.lucene.analysis.tokenattributes.OffsetAttribute;
import org.apache.lucene.analysis.tokenattributes.TermAttribute;

import common.FieldConfig;

public class FieldComsumer extends Thread {
	InvertedDocList postingLists;

	LinkedBlockingQueue<DocField> queue = new LinkedBlockingQueue<DocField>(
			10000);

	int field_num;
	int field_config;

	public FieldComsumer(int field_num, int field_config, String path) {
		this.field_num = field_num;
		this.field_config = field_config;
		postingLists = new InvertedDocList(field_num, path);
	}

	public void addDocument(DocField doc) {
		try {
			queue.put(doc);
		} catch (InterruptedException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
	}

	public void consumer(DocField doc) {
		if (FieldConfig.isOmitTf(field_config)) {
			postingLists.addTermDoc(doc.field.getValue(), doc.doc, 1, null);
		} else {

			SimpleAnalyzer analyzer = new SimpleAnalyzer();
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
			OffsetAttribute offAtt = (OffsetAttribute) stream
					.addAttribute(OffsetAttribute.class);
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
					System.out.println(termAtt.term() + " "
							+ offAtt.startOffset() + " " + offAtt.endOffset());
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
