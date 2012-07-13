package engine.search;

import java.io.IOException;
import java.util.ArrayList;

import org.apache.lucene.store.IndexInput;

import engine.common.FieldConfig;

final class TermScorer extends Scorer {

	private FieldOffset fieldOffset;
	public long offset = 0;
	public int doc_freq = 0;
	public int field_config = 0;
	public IndexInput input;
	public DocTf docTf = new DocTf();
	public int advance = 1;

	TermScorer(FieldOffset fieldOffset) throws IOException {
		// super(similarity);
		this.fieldOffset = fieldOffset;
		this.offset = fieldOffset.getOffset_();
		this.doc_freq = fieldOffset.getDoc_freq_();
		this.input = (IndexInput) fieldOffset.getInput_().clone();
		this.field_config = fieldOffset.getField_config_();
		input.seek(offset);

	}

	@Override
	public void score(Collector collector) throws IOException {
		collector.setScorer(this);
		DocTf docTf;
		while ((docTf = nextDocTf()) != NO_MORE_DOCS) {
			collector.collect(docTf);
		}
	}

	@Override
	protected boolean score(Collector collector, int max, int firstDocID)
			throws IOException {
		collector.setScorer(this);
		DocTf docTf = docTf();
		int doc = docTf.doc;
		while (doc < max) {
			collector.collect(docTf);
			docTf = nextDocTf();
		}
		return docTf != NO_MORE_DOCS;
	}

	@Override
	public float score() throws IOException {
		return docTf().tf;
	}

	@Override
	public DocTf docTf() {
		return docTf;
	}

	@Override
	public DocTf nextDocTf() throws IOException {
		if (doc_freq > 0 && advance <= doc_freq) {
			if (FieldConfig.isOmitTf(field_config)) {
				int id = input.readVInt();
				this.docTf.doc += id;
				this.docTf.tf = 1;
			} else {
				int id = input.readVInt();
				int tf = input.readVInt();
				this.docTf.doc += id;
				this.docTf.tf = tf;
			}
			++advance;
			System.out.println("doc:" + this.docTf.doc + " tf:" + this.docTf.tf
					+ " advance:" + advance);
			return this.docTf;
		} else {
			return this.docTf = new DocTf(Integer.MAX_VALUE, -1);
		}
	}

	@Override
	public DocTf advance(DocTf target) throws IOException {
		while (advance <= doc_freq) {
			if (FieldConfig.isOmitTf(field_config)) {
				this.docTf.doc += input.readVInt();
				this.docTf.tf = 1;
			} else {
				this.docTf.doc += input.readVInt();
				this.docTf.tf = input.readVInt();
			}
			System.out.println("advance===doc:" + this.docTf.doc + " tf:"
					+ this.docTf.tf);
			advance++;
			if (this.docTf.doc >= target.doc)
				return this.docTf;

		}
		return this.docTf = new DocTf(Integer.MAX_VALUE, -1);
	}

}
