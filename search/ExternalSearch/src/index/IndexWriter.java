package index;

import java.io.File;
import java.io.IOException;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.Map.Entry;
import java.util.concurrent.atomic.AtomicInteger;

import org.apache.lucene.analysis.Analyzer;

import common.FieldConfig;

public class IndexWriter {
	private HashMap<String, Long> fieldMap = new HashMap<String, Long>();
	int max_fields = 0;
	String path;
	Analyzer analyzer;
	AtomicInteger maxDoc = new AtomicInteger(-1);

	int field_max;
	ArrayList<FieldConsumer> consumer = new ArrayList<FieldConsumer>();
	MultiFieldConsumer multi_consumer;
	SimpleFSIndexOutput out;

	public IndexWriter(String dir, Analyzer analyzer, int field_max)
			throws IOException {
		path = dir;
		this.field_max = field_max;
		multi_consumer = new MultiFieldConsumer(field_max, 0, path);
		out = new SimpleFSIndexOutput(new File(path + "/" + "field.fnm"));
		out.writeVInt(field_max);
	}

	public int addDocument(Document doc) throws Exception {
		if (doc.getFieldSize() != max_fields) {
			throw new Exception("this document is not belongs this writer");
		}
		doc.setDoc(maxDoc.incrementAndGet());
		// TODO
		for (int i = 0; i < max_fields; ++i) {
			consumer.get(i).addDocument(
					new DocField(doc.getDoc(), doc.getField(i)));
		}
		multi_consumer.addDocument(doc);
		return doc.getDoc();
	}

	public void stop() {
		Document doc = new Document(max_fields);
		doc.setDoc(-1);
		for (int i = 0; i < max_fields; ++i) {
			consumer.get(i).addDocument(
					new DocField(doc.getDoc(), new Field("", "")));
		}
		multi_consumer.addDocument(doc);
	}

	public void addField(String field_name, int config) throws IOException {
		fieldMap.put(field_name, ((long) max_fields) << 32 | (long) config);

		consumer.add(new FieldConsumer(max_fields, config, path));
		consumer.get(consumer.size() - 1).start();

		out.writeString(field_name);
		out.writeInt(max_fields);
		out.writeInt(config);
		++max_fields;
		if (field_max == max_fields) {
			out.close();
			multi_consumer.start();
		}
		// TODO
		// Write Field Info Into .fnm
	}

	public static void main(String[] args) throws Exception {
		IndexWriter writer = new IndexWriter("/home/tanbokan/index/test", null,
				2);
		writer.addField("test1", 1 << FieldConfig.multiField);
		writer.addField("test2", 1 << FieldConfig.multiField);
		Document doc1 = new Document(2);
		doc1.addField(new Field("test1", "a b c de"));
		doc1.addField(new Field("test2", "a b c de"));
		writer.addDocument(doc1);

		Document doc2 = new Document(2);
		doc2.addField(new Field("test1", "bb cc de o ab bb ab a"));
		doc2.addField(new Field("test2", "a b c de ef g"));
		// writer.addDocument(doc1);
		writer.addDocument(doc2);
		writer.stop();
	}

}
