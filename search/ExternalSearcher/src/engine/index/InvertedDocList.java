package engine.index;

import java.io.File;
import java.io.IOException;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.LinkedList;
import java.util.Map.Entry;

import build.TimeCost;
import engine.common.FieldConfig;
import engine.index.TermDocs.Doc;

public class InvertedDocList {

	int field_num;
	String dir;

	boolean omitTf;

	public InvertedDocList(int field_num, int field_config, String dir) {
		this.field_num = field_num;
		this.dir = dir;
		omitTf = FieldConfig.isOmitTf(field_config);
	}

	HashMap<String, LinkedList<TermDocs>> invertList = new HashMap<String, LinkedList<TermDocs>>();

	// ArrayList<TermDocs> docs = new ArrayList<TermDocs>();

	public void addTermDoc(String term, int doc, int tf, int[] position) {
		// System.out.println("term: "+term+" doc: "+doc+" tf: "+tf);
		if (omitTf)
			tf = -1;
		LinkedList<TermDocs> indexs = invertList.get(term);
		if (indexs == null) {
			indexs = new LinkedList<TermDocs>();
			TermDocs termDocs = new TermDocs();
			termDocs.addTermDoc(doc, tf);
			indexs.add(termDocs);
			invertList.put(term, indexs);
		} else {
			TermDocs termDocs = indexs.getLast();
			if (termDocs.isEnd()) {
				TermDocs e = new TermDocs();
				e.addTermDoc(doc, tf);
				indexs.add(e);
			} else {
				termDocs.addTermDoc(doc, tf);
			}
		}
	}

	public void writeTermDocs(int field_config) throws IOException {
		TimeCost tc = TimeCost.begin("FieldWriter:" + field_num);
		File file1 = new File(dir + "/" + field_num + ".idx");
		SimpleFSIndexOutput out1 = new SimpleFSIndexOutput(file1);
		File file2 = new File(dir + "/" + field_num + ".frq");
		SimpleFSIndexOutput out2 = new SimpleFSIndexOutput(file2);
		out1.writeVInt(invertList.size());
		// System.out.println("Write TermDocs field: "+field_num+" terms: "+invertList.size());
		for (Entry<String, LinkedList<TermDocs>> entry : invertList.entrySet()) {
			out1.writeString(entry.getKey());
			LinkedList<TermDocs> indexs = entry.getValue();
			int docFreq = TermDocs.MAX * (indexs.size() - 1)
					+ indexs.getLast().getDocAndTf().size();
			out1.writeVInt(docFreq);
			out1.writeVLong(out2.getFilePointer());
			StringBuilder builder = new StringBuilder();
			builder.append("Field:" + field_num + " " + entry.getKey()
					+ " DocFreq: " + docFreq + " List: ");
			int pre = 0;
			for (TermDocs index : indexs) {
				ArrayList<Doc> doctfs = index.getDocAndTf();

				for (Doc info : doctfs) {
					out2.writeVInt(info.getDoc() - pre);
					if (!omitTf) {
						out2.writeVInt(info.getTf());
						builder.append(info.getDoc() + "(" + info.getTf() + ")");
					} else {
						builder.append(info.getDoc() + "(" + info.getTf() + ")");
					}
					pre = info.doc;
				}
			}
			System.err.println(builder.toString());
		}
		tc.endFinally();
		out1.close();
		out2.close();
	}
}
