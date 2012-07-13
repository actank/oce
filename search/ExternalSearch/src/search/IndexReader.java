package search;

import java.io.File;
import java.io.IOException;
import java.util.ArrayList;
import java.util.HashMap;

import org.apache.lucene.store.IndexInput;

public class IndexReader {

	private static int field_num = 0;
	private String index_path;
	public static ArrayList<HashMap<String, IndexOption>> wordOffset = new ArrayList<HashMap<String, IndexOption>>();
	public static ArrayList<FieldOption> fieldOptions = new ArrayList<FieldOption>();
	public static ArrayList<IndexInput> mmapinputs = new ArrayList<IndexInput>();

	public IndexReader(String path) {
		this.index_path = path;
	}

	public void readField() throws IOException {
		File fieldFile = new File(index_path + "/field.fnm");
		SimpleFSIndexInput input = new SimpleFSIndexInput(fieldFile);
		field_num = input.readVInt();
//		field_num = 16;
		for (int i = 0; i < field_num; i++) {
			FieldOption option = new FieldOption();
			option.setField_name(input.readString());
			option.setField_id(input.readInt());
			option.setField_config(input.readInt());
			fieldOptions.add(option);
		}
	}

	public void readIndex() throws IOException {
		for (int i = 0; i < fieldOptions.size(); i++) {
			FieldOption option = fieldOptions.get(i);
			File idxField = new File(index_path + "/" + option.getField_id()
					+ ".idx");
			SimpleFSIndexInput input = new SimpleFSIndexInput(idxField);
			// boolean isOmitTf =
			// FieldConfig.isOmitTf(option.getField_config());
			int word_num = input.readVInt();
			HashMap<String, IndexOption> wordInOp = new HashMap<String, IndexOption>();
			for (int j = 0; j < word_num; j++) {
				String word = input.readString();
				IndexOption indexOption = new IndexOption();
				indexOption.setDoc_freq(input.readVInt());
				indexOption.setOffset(input.readVLong());
				wordInOp.put(word, indexOption);
			}
			wordOffset.add(wordInOp);
		}
	}

	public void readFrequency() throws IOException {
		for (int i = 0; i < fieldOptions.size(); i++) {
			FieldOption option = fieldOptions.get(i);
			File frepField = new File(index_path + "/" + option.getField_id()
					+ ".frq");
			mmapinputs.add(new MMapIndexInput(frepField));
		}
	}

	public static int getFieldNum() {
		return field_num;
	}
}
