package engine.search;

import java.util.ArrayList;
import java.util.BitSet;
import java.util.HashMap;

import org.apache.lucene.store.IndexInput;

public final class FieldOffset {
	final int field_id_;
	final int doc_freq_;
	final long offset_;
	final MMapIndexInput input_;
	final int field_config_;
	final BitSet doc_mask_;

	FieldOffset(Term term, IndexReader reader) {
		ArrayList<HashMap<String, IndexOption>> wordOffset = reader
				.getWordOffset();
		HashMap<String, BitSet> filter = reader.getFilter();
		ArrayList<FieldOption> fieldOptions = reader.getFieldOptions();
		ArrayList<IndexInput> mmapinputs = reader.getMmapinputs();

		int field = term.getField_();
		String word = term.getWord_();
		doc_mask_ = filter.get(field + ":" + word);
		if (doc_mask_ == null && wordOffset.get(field).containsKey(word)) {
			field_id_ = field;
			doc_freq_ = wordOffset.get(field).get(word).getDoc_freq();
			offset_ = wordOffset.get(field).get(word).getOffset();
			input_ = (MMapIndexInput) mmapinputs.get(field);
			field_config_ = fieldOptions.get(field).getField_config();
		} else {
			field_id_ = field;
			doc_freq_ = 0;
			offset_ = -1;
			field_config_ = fieldOptions.get(field).getField_config();
			input_ = null;
		}
	}

	public BitSet getDocMask() {
		return doc_mask_;
	}

	public long getOffset_() {
		return offset_;
	}

	public int getField_id_() {
		return field_id_;
	}

	public int getDoc_freq_() {
		return doc_freq_;
	}

	public MMapIndexInput getInput_() {
		return input_;
	}

	public int getField_config_() {
		return field_config_;
	}

}