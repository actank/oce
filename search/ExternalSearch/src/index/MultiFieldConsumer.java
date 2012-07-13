package index;

import java.io.IOException;
import java.io.StringReader;
import java.util.HashMap;
import java.util.Map.Entry;
import java.util.concurrent.LinkedBlockingQueue;
import java.util.concurrent.TimeUnit;

import org.apache.lucene.analysis.SimpleAnalyzer;
import org.apache.lucene.analysis.TokenStream;
import org.apache.lucene.analysis.tokenattributes.TermAttribute;

public class MultiFieldConsumer extends Thread {
	InvertedDocList postingLists;

	LinkedBlockingQueue<Document> queue = new LinkedBlockingQueue<Document>(
			10000);

	int field_num;
	int field_config;

	public MultiFieldConsumer(int field_num, int field_config, String path) {
		this.field_num = field_num;
		this.field_config = field_config;
		postingLists = new InvertedDocList(field_num, path);
	}

	public void addDocument(Document doc) {
		try {
			queue.put(doc);
		} catch (InterruptedException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
	}

	public void consumer(Document doc) {

		HashMap<String, Integer> terms = new HashMap<String, Integer>();
		SimpleAnalyzer analyzer = new SimpleAnalyzer();
		for (int i = 0; i < field_num; ++i) {
			TokenStream stream = null;
			try {
				stream = analyzer.reusableTokenStream("null", new StringReader(
						doc.getField(i).getValue()));
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
						cnt = new Integer((1<<i));
						terms.put(termAtt.term(), cnt);
					} else {
						terms.put(termAtt.term(), cnt|(1<<i));
					}
//					System.out.println(termAtt.term() + " "
//							+ offAtt.startOffset() + " " + offAtt.endOffset());
				}
				

				// postingLists.addTermDoc(termAtt.term(), doc.getDoc(), tf,
				// position)

			} catch (IOException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
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
}
