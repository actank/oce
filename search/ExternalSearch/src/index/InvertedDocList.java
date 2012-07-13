package index;

import index.TermDocs.DocAndTf;

import java.io.File;
import java.io.IOException;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.Map.Entry;

import common.FieldConfig;

public class InvertedDocList {

	int field_num;
	String dir;

	public InvertedDocList(int field_num, String dir) {
		this.field_num = field_num;
		this.dir = dir;
	}

	HashMap<String, Integer> invertList = new HashMap<String, Integer>();
	ArrayList<TermDocs> docs = new ArrayList<TermDocs>();

	public void addTermDoc(String term, int doc, int tf, int[] position) {
		// System.out.println("term: "+term+" doc: "+doc+" tf: "+tf);
		Integer index = invertList.get(term);
		if (index == null) {
			index = docs.size();
			invertList.put(term, index);
			TermDocs termDocs = new TermDocs();
			termDocs.addTermDoc(doc, tf);
			docs.add(termDocs);
		} else {
			TermDocs termDocs = docs.get(index);
			termDocs.addTermDoc(doc, tf);
		}
	}

	public void writeTermDocs(int field_config) throws IOException {
		boolean isOmitTf = FieldConfig.isOmitTf(field_config);
		File file1 = new File(dir + "/" + field_num + ".idx");
		SimpleFSIndexOutput out1 = new SimpleFSIndexOutput(file1);
		File file2 = new File(dir + "/" + field_num + ".frq");
		SimpleFSIndexOutput out2 = new SimpleFSIndexOutput(file2);
		out1.writeVInt(invertList.size());
		// System.out.println("Write TermDocs field: "+field_num+" terms: "+invertList.size());
		for (Entry<String, Integer> entry : invertList.entrySet()) {
			out1.writeString(entry.getKey());
			int index = entry.getValue();

			ArrayList<DocAndTf> doctfs = docs.get(index).getDocAndTf();

			out1.writeVInt(doctfs.size());
			out1.writeVLong(out2.getFilePointer());
			StringBuilder builder = new StringBuilder();
			builder.append("Field:" + field_num + " " + entry.getKey()
					+ " Index: " + entry.getValue() + " DocFreq: "
					+ doctfs.size() + " DocList: ");
			for (DocAndTf info : doctfs) {
				out2.writeVInt(info.doc);
				if (!isOmitTf) {
					out2.writeVInt(info.tf);
					builder.append(info.doc + "(" + info.tf + ")");
				} else {
					builder.append(info.doc + "(None)");
				}
			}
			System.out.println(builder.toString());
		}
		out1.close();
		out2.close();
	}
}
