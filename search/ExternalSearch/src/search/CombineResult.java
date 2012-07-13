package search;

import java.io.IOException;
import java.util.ArrayList;
import java.util.Collections;
import java.util.List;

import search.IndexSearcher.FieldOffset;

import common.FieldConfig;

public class CombineResult {

	class HitResult implements Comparable<HitResult> {
		public int advance = 1;
		public FieldOffset doclist;
		public long offset = 0;
		public int doc_freq = 0;
		public int field_config = 0;
		public MMapIndexInput input;
		public DocTf docTf = new DocTf();

		public HitResult(FieldOffset doclist) throws IOException {
			this.doclist = doclist;
			this.offset = doclist.getOffset_();
			this.doc_freq = doclist.getDoc_freq_();
			this.input = doclist.getInput_();
			this.field_config = doclist.getField_config_();
			input.seek(offset);

			if (FieldConfig.isOmitTf(field_config)) {
				DocTf docTf = new DocTf(input.readVInt(), 1);
				this.docTf = docTf;
			} else {
				DocTf docTf = new DocTf(input.readVInt(), input.readVInt());
				this.docTf = docTf;
			}

		}

		public DocTf doctf() throws NumberFormatException, IOException {
			// if (doc_freq > 0 && advance < doc_freq) {
			// if (FieldConfig.isOmitTf(field_config)) {
			// DocTf docTf = new DocTf(input.readVInt(), 1);
			// return docTf;
			// } else {
			// DocTf docTf = new DocTf(input.readVInt(), input.readVInt());
			// return docTf;
			// }
			return docTf;
			// } else
			// return null;
		}

		public DocTf skip(DocTf docTf) throws NumberFormatException,
				IOException {
			DocTf dtf = new DocTf();
			while (advance < doc_freq) {
				if (FieldConfig.isOmitTf(field_config)) {
					dtf.doc = input.readVInt();
					dtf.tf = 1;
				} else {
					dtf.doc = input.readVInt();
					dtf.tf = input.readVInt();
				}
				advance++;
				if (dtf.doc >= docTf.doc)
					return this.docTf = dtf;

			}
			return this.docTf = null;
		}

		public DocTf next() throws IOException {
			if (doc_freq > 0 && advance < doc_freq) {
				if (FieldConfig.isOmitTf(field_config)) {
					int id = input.readVInt();
					this.docTf.doc = id;
					this.docTf.tf = 1;
				} else {
					int id = input.readVInt();
					int tf = input.readVInt();
					this.docTf.doc = id;
					this.docTf.tf = tf;
				}
				++advance;
				return this.docTf;
			} else {
				return this.docTf = null;
			}
		}

		// public List<String> fields;

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

	public boolean add(FieldOffset offset) throws IOException {
		// System.out.println(hits[i].length());
		if (offset.getDoc_freq_() == 0)
			return false;
		result.add(new HitResult(offset));
		return true;
	}

	public List<DocTf> union() throws IOException {
		System.out.println("Union waiting");

		Collections.sort(result);
		List<DocTf> docs = new ArrayList<DocTf>();

		DocTf docTf;
		while ((docTf = doNext()) != null) {
			DocTf dtf = new DocTf();
			dtf.doc = docTf.doc;
			dtf.tf = docTf.tf;
			docs.add(dtf);
			if (result.get(result.size() - 1).next() == null) {
				break;
			}
		}
		return docs;
	}

	private DocTf doNext() throws IOException {
		int first = 0;
		DocTf docTf = result.get(result.size() - 1).doctf();
		HitResult firstRes;

		while ((firstRes = result.get(first)).doctf().doc < docTf.doc) {
			// System.out.println("First : " + firstRes.docID()+" Doc :" + doc);
			// if (firstRes.doctf() == null)
			// break;
			docTf = firstRes.skip(docTf);
			// System.out.println("Skip : " + doc);
			first = first == result.size() - 1 ? 0 : first + 1;
			if (docTf == null)
				break;
		}
		return docTf;
	}
}
