package engine.search;

import java.io.IOException;
import java.util.ArrayList;
import java.util.BitSet;
import java.util.Collections;
import java.util.List;

import net.paoding.analysis.dictionary.Hit;

import org.apache.lucene.store.IndexInput;

import engine.common.FieldConfig;

public class CombineResult {

	class HitResult implements Comparable<HitResult> {
		public int advance = 1;
		public FieldOffset doclist;
		public long offset = 0;
		public int doc_freq = 0;
		public int field_config = 0;
		public IndexInput input;
		public DocTf docTf = new DocTf();
		BitSet set = new BitSet();

		public HitResult(FieldOffset doclist) throws IOException {
			this.doclist = doclist;
			this.offset = doclist.getOffset_();
			this.doc_freq = doclist.getDoc_freq_();
			this.input = (IndexInput) doclist.getInput_().clone();
			this.field_config = doclist.getField_config_();
			input.seek(offset);

			if (FieldConfig.isOmitTf(field_config)) {
				DocTf docTf = new DocTf(input.readVInt(), 1);
				this.docTf = docTf;
				// System.out.println("read doc: " + docTf.doc + " tf: 0");
			} else {
				DocTf docTf = new DocTf(input.readVInt(), input.readVInt());
				this.docTf = docTf;
				// System.out
				// .println("read doc: " + docTf.doc + " tf:" + docTf.tf);
			}

		}

		public DocTf doctf() throws NumberFormatException, IOException {
			return docTf;
		}

		public DocTf skip(DocTf docTf) throws NumberFormatException,
				IOException {
			while (advance < doc_freq) {
				if (FieldConfig.isOmitTf(field_config)) {
					this.docTf.doc += input.readVInt();
					this.docTf.tf = 1;
					// System.out.println("read doc: " + docTf.doc + " tf: 0");
				} else {
					this.docTf.doc += input.readVInt();
					this.docTf.tf = input.readVInt();
					// System.out.println("read doc: " + docTf.doc + " tf:"
					// + docTf.tf);
				}
				advance++;
				if (this.docTf.doc >= docTf.doc)
					return this.docTf;

			}
			return this.docTf = null;
		}

		public DocTf next() throws IOException {
			if (doc_freq > 0 && advance < doc_freq) {
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
				return this.docTf;
			} else {
				return this.docTf = null;
			}
		}

		@Override
		public int compareTo(HitResult o) {
			// TODO Auto-generated method stub
			try {
				return this.doctf().doc - o.doctf().doc;
			} catch (NumberFormatException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			} catch (IOException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
			return 0;
		}
	};

	private List<HitResult> result = new ArrayList<HitResult>();;

	private List<BitSet> filter = new ArrayList<BitSet>();

	public boolean add(FieldOffset offset) throws IOException {
		if (offset.getDoc_freq_() == 0 && offset.getDocMask() == null)
			return false;
		if (offset.getDocMask() != null) {
			filter.add(offset.getDocMask());
		} else {
			result.add(new HitResult(offset));
		}
		return true;
	}

	public int getQuerySize() {
		return result.size() + filter.size();
	}

	public boolean passFilter(int doc) {
		if (filter.isEmpty()) {
			return true;
		} else {
			for (BitSet set : filter) {
				if (!set.get(doc)) {
					return false;
				}
			}
			return true;
		}
	}

	public ArrayList<DocTf> union() throws IOException {
		System.out.println("Union waiting");

		Collections.sort(result);
		ArrayList<DocTf> docs = new ArrayList<DocTf>(500);

		DocTf docTf;
		int count = 0;
		while ((docTf = doNext()) != null) {
			DocTf dtf = new DocTf();
			dtf.doc = docTf.doc;
			dtf.tf = docTf.tf;
			if (passFilter(dtf.doc))
				docs.add(dtf);
			if ((result.get(result.size() - 1).next() == null)) {
				break;
			}
			count++;
		}
		return docs;
	}

	private DocTf doNext() throws IOException {
		int first = 0;
		DocTf docTf = result.get(result.size() - 1).doctf();
		HitResult firstRes = result.get(first);

		while (firstRes.doctf().doc < docTf.doc) {
			// System.out.println("First : " + first + " Doc :"
			// + firstRes.doctf().doc);
			// if (firstRes.doctf() == null)
			// break;
			// System.out.println("Last Doc :" + docTf.doc);
			docTf = firstRes.skip(docTf);

			// System.out.println("Skip : " + doc);
			first = first == result.size() - 1 ? 0 : first + 1;
			firstRes = result.get(first);
			if (docTf == null)
				break;
		}
		return docTf;
	}
}
