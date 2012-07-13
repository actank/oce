package engine.search;

import java.io.File;
import java.io.FileWriter;
import java.io.IOException;
import java.util.ArrayList;
import java.util.BitSet;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.Map.Entry;

import org.apache.lucene.store.IndexInput;

import com.sleepycat.je.utilint.BitMap;

import engine.common.FieldConfig;

public class IndexReader {

	private int field_num = 0;
	private String index_path;
	private ArrayList<HashMap<String, IndexOption>> wordOffset = new ArrayList<HashMap<String, IndexOption>>();
	private ArrayList<FieldOption> fieldOptions = new ArrayList<FieldOption>();
	private ArrayList<IndexInput> mmapinputs = new ArrayList<IndexInput>();
	private int[] ids;
	private HashMap<String, BitSet> filter = new HashMap<String, BitSet>();

	public IndexReader() {

	}

	public IndexReader(String path) throws IOException {
		System.out.println("IndexReader load : " + path);
		this.index_path = path;
		readIds();
		readField();
		readIndex();
		// readFrequency();

	}

	private void readField() throws IOException {
		File fieldFile = new File(index_path + "/field.fnm");
		SimpleFSIndexInput input = new SimpleFSIndexInput(fieldFile);
		field_num = input.readVInt();
		for (int i = 0; i < field_num; i++) {
			FieldOption option = new FieldOption();
			option.setField_name(input.readString());
			option.setField_id(input.readInt());
			option.setField_config(input.readInt());
			fieldOptions.add(option);
		}
		FieldOption option = new FieldOption();
		option.setField_name("DEFAULT");
		option.setField_id(field_num);
		option.setField_config(0);
		fieldOptions.add(option);
	}

	private void readIndex() throws IOException {
		FileWriter fw = new FileWriter("/home/dong/index2/test.log",
				true);
		for (int i = 0; i < fieldOptions.size(); i++) {
			HashMap<String, IndexOption> filter_words = new HashMap<String, IndexOption>();
			FieldOption option = fieldOptions.get(i);
			File idxField = new File(index_path + "/" + option.getField_id()
					+ ".idx");
			SimpleFSIndexInput input = new SimpleFSIndexInput(idxField);
			int word_num = input.readVInt();
			HashMap<String, IndexOption> wordInOp = new HashMap<String, IndexOption>();
			for (int j = 0; j < word_num; j++) {
				String word = input.readString();
				IndexOption indexOption = new IndexOption();
				indexOption.setDoc_freq(input.readVInt());
				indexOption.setOffset(input.readVLong());
				wordInOp.put(word, indexOption);
				if (indexOption.getDoc_freq() > 1000000) {
					filter_words.put(word, indexOption);
				}
				fw.write("field:" + option.getField_id() + " word:" + word
						+ " doc_freq:" + indexOption.getDoc_freq() + " offset:"
						+ indexOption.getOffset() + "\n");
			}

			wordOffset.add(wordInOp);

			File frepField = new File(index_path + "/" + option.getField_id()
					+ ".frq");
			MMapIndexInput inputs = new MMapIndexInput(frepField);
			mmapinputs.add(inputs);
			boolean omitTf = FieldConfig.isOmitTf(option.getField_config());
			for (Entry<String, IndexOption> filter_word : filter_words
					.entrySet()) {
				System.out.println("loading filter word : " + i + " : "
						+ filter_word.getKey());
				BitSet set;
				if (!omitTf)
					set = readFilterWithTf(inputs, filter_word.getValue());
				else
					set = readFilterWithoutTf(inputs, filter_word.getValue());
				filter.put(i + ":" + filter_word.getKey(), set);
			}
		}
		fw.close();
	}

	public BitSet readFilterWithTf(IndexInput input, IndexOption option)
			throws IOException {
		BitSet set = new BitSet(ids.length);
		int total = option.getDoc_freq();
		input.seek(option.getOffset());
		int doc = 0;
		for (int i = 0; i < total; ++i) {
			doc += input.readVInt();
			set.set(doc);
			input.readVInt();
		}
		return set;
	}

	public BitSet readFilterWithoutTf(IndexInput input, IndexOption option)
			throws IOException {
		BitSet set = new BitSet(ids.length);
		int total = option.getDoc_freq();
		input.seek(option.getOffset());
		int doc = 0;
		for (int i = 0; i < total; ++i) {
			doc += input.readVInt();
			set.set(doc);
		}
		return set;
	}

	// private void readFrequency() throws IOException {
	// for (int i = 0; i < fieldOptions.size(); i++) {
	// FieldOption option = fieldOptions.get(i);
	// File frepField = new File(index_path + "/" + option.getField_id()
	// + ".frq");
	// mmapinputs.add(new MMapIndexInput(frepField));
	// }
	// }

	private void readIds() throws IOException {
		File file = new File(index_path + "/docids");
		SimpleFSIndexInput input = new SimpleFSIndexInput(file);
		ids = new int[input.readInt()];
		for (int i = 0; i < ids.length; ++i) {
			ids[i] = input.readInt();
		}
	}

	public ArrayList<Integer> mappingId(ScoreDoc[] scoreDocs) {
		ArrayList<Integer> res = new ArrayList<Integer>(scoreDocs.length);
		// System.out.println("ids.size " + ids.length);
		for (int i = 0; i < scoreDocs.length; ++i) {
			// System.out.println("doc:" + doctfs.get(i).doc + " id: "
			// + ids[doctfs.get(i).doc]);
			res.add(ids[scoreDocs[i].doc]);
			// System.out
			// .println("doc:" + doctfs.get(i).doc + " userId:" + res[i]);
		}
		return res;
	}

	public HashMap<String, BitSet> getFilter() {
		return filter;
	}

	public int getFieldNum() {
		return field_num;
	}

	public ArrayList<FieldOption> getFieldOptions() {
		return fieldOptions;
	}

	public ArrayList<HashMap<String, IndexOption>> getWordOffset() {
		return wordOffset;
	}

	public ArrayList<IndexInput> getMmapinputs() {
		return mmapinputs;
	}
}