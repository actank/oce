package search;

import java.io.IOException;
import java.util.ArrayList;
import java.util.List;
import java.util.Queue;

public class IndexSearcher {
	// @SuppressWarnings("unused")
	public class FieldOffset {
		private int field_id_;
		private int doc_freq_;
		private long offset_;
		private MMapIndexInput input_;
		private int field_config_;

		public void setOffset_(long offset) {
			this.offset_ = offset;
		}

		public long getOffset_() {
			return offset_;
		}

		public void setField_id_(int field_id) {
			this.field_id_ = field_id;
		}

		public int getField_id_() {
			return field_id_;
		}

		public void setDoc_freq_(int doc_freq_) {
			this.doc_freq_ = doc_freq_;
		}

		public int getDoc_freq_() {
			return doc_freq_;
		}

		public void setInput_(MMapIndexInput input_) {
			this.input_ = input_;
		}

		public MMapIndexInput getInput_() {
			return input_;
		}

		public void setField_config_(int field_config_) {
			this.field_config_ = field_config_;
		}

		public int getField_config_() {
			return field_config_;
		}

	}

	private Queue<ScoreDoc>[] scoreDocs = null;

	public ArrayList<FieldOffset> getOffset(ArrayList<TermQuery> querys) {
		ArrayList<FieldOffset> fieldOffsets = new ArrayList<FieldOffset>();
		for (int i = 0; i < querys.size(); i++) {
			int field = querys.get(i).getField_();
			String word = querys.get(i).getWord_();
			if (IndexReader.wordOffset.get(field).containsKey(word)) {
				FieldOffset field_offset = new FieldOffset();
				field_offset.setField_id_(field);
				field_offset.setDoc_freq_(IndexReader.wordOffset.get(field)
						.get(word).getDoc_freq());
				field_offset.setOffset_(IndexReader.wordOffset.get(field)
						.get(word).getOffset());
				MMapIndexInput input = (MMapIndexInput) IndexReader.mmapinputs
						.get(field).clone();
				int field_config = IndexReader.fieldOptions.get(field)
						.getField_config();
				field_offset.setInput_(input);
				field_offset.setField_config_(field_config);
				fieldOffsets.add(field_offset);
			} else {
				FieldOffset field_offset = new FieldOffset();
				field_offset.setField_id_(field);
				field_offset.setDoc_freq_(0);
				field_offset.setOffset_(-1);
				int field_config = IndexReader.fieldOptions.get(field)
						.getField_config();
				field_offset.setInput_(null);
				field_offset.setField_config_(field_config);
				fieldOffsets.add(field_offset);
			}
		}
		return fieldOffsets;
	}

	public List<DocTf> getUnion(ArrayList<FieldOffset> fieldOffsets)
			throws IOException {
		List<DocTf> list = new ArrayList<DocTf>();
		CombineResult result = new CombineResult();
		for (int i = 0; i < fieldOffsets.size(); i++) {
			if (!result.add(fieldOffsets.get(i))) {
				return list;
			}
		}
		list = result.union();
		return list;
	}

}
