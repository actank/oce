import java.io.DataOutput;
import java.io.DataInput;
import java.io.IOException;

import java.util.*;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

import org.apache.hadoop.mapreduce.lib.input.FileInputFormat;
import org.apache.hadoop.mapreduce.lib.input.TextInputFormat;
import org.apache.hadoop.mapreduce.lib.output.FileOutputFormat;
import org.apache.hadoop.mapreduce.lib.output.TextOutputFormat;

import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.conf.Configured;
import org.apache.hadoop.fs.FSDataInputStream;
import org.apache.hadoop.fs.FileSystem;
import org.apache.hadoop.fs.Path;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.io.Text; //import org.apache.hadoop.mapred.FileInputFormat;
//import org.apache.hadoop.mapred.FileOutputFormat;
import org.apache.hadoop.mapred.JobClient;
import org.apache.hadoop.mapred.KeyValueTextInputFormat; //import org.apache.hadoop.mapred.Mapper;
import org.apache.hadoop.mapred.OutputCollector; //import org.apache.hadoop.mapred.Reducer;
import org.apache.hadoop.mapred.Reporter; //import org.apache.hadoop.mapred.TextOutputFormat;
import org.apache.hadoop.mapred.lib.ChainMapper;
import org.apache.hadoop.util.Tool;
import org.apache.hadoop.util.ToolRunner;
import org.apache.hadoop.util.bloom.BloomFilter;
import org.apache.hadoop.util.bloom.Key;
import org.apache.hadoop.mapreduce.Mapper;
import org.apache.hadoop.mapreduce.Reducer;
import org.apache.hadoop.mapreduce.Job;
import org.apache.hadoop.io.*; //import org.apache.hadoop.mapred.lib.MultipleInputs;
import org.apache.hadoop.mapreduce.lib.input.MultipleInputs;

//import MyEntry;

public class PreparePeopleRank extends Configured implements Tool {
	static enum MapCount {
		CLICK, REPLY, NEWS, GOODACTOR, GOODSTYPE, BADACTOR, BADSTYPE, ALL
	};

	public static class MapClass extends
			Mapper<LongWritable, Text, LongWritable, Text> {

		HashMap<Long, ArrayList<MyEntry<Long, Float>>> from_to_score = new HashMap<Long, ArrayList<MyEntry<Long, Float>>>();

		// protected void setup(
		// Context context) {
		// }
		// public void Insert(long to, long from,float score){
		// if(from_to_score.containsKey(to)){
		// ArrayList<MyEntry<Long, Float> > tmp = from_to_score.get(to);
		// MyEntry<Long, Float> e = new MyEntry<Long, Float> (from,score);
		// tmp.add(e) ;
		// }else{
		// ArrayList<MyEntry<Long, Float> > tmp = new ArrayList<MyEntry<Long,
		// Float> > ();
		// tmp.add(new MyEntry<Long, Float>(from,score) );
		// from_to_score.put(to, tmp);
		// }
		// }

		// 输入格式 to feature score
		public void map(LongWritable key, Text value, Context context)
				throws IOException, InterruptedException {
			try {
				String valueline = value.toString();
				String keyline = key.toString();
				String[] tokens = valueline.split("\t");
				long to = Long.parseLong(tokens[0]);
				
				
				if(tokens[1].equals("315r")){
					return;
				}
				long feature = Long.parseLong(tokens[1]);
				if (feature < 0) {
					return;
				}
				context.write(new LongWritable(to), new Text(tokens[1] + "\t"
						+ tokens[2]));
			} catch (InterruptedException e) {
				e.printStackTrace();
			}
			// float score = Float.parseFloat(tokens[2]);
			// Insert(to,feature,score);
		}

		// nid c claim
		protected void cleanup(Mapper.Context context) throws IOException {
			try {
				Iterator it = from_to_score.entrySet().iterator();
				while (it.hasNext()) {
					Map.Entry pairs = (Map.Entry) it.next();
					context.write(new Text(pairs.getKey().toString()),
							new Text("c\t" + pairs.getValue().toString()));
					System.out.println("cleanup. target:" + pairs.getKey()
							+ " claim:" + pairs.getValue().toString());
				}
			} catch (InterruptedException e) {
				e.printStackTrace();
			}
		}

	}

	public static class Reduce extends
			Reducer<LongWritable, Text, LongWritable, Text> {
		static enum ReduceCount {
			VALID, INVALID
		};

		Map<String, Map<String, Integer>> from_stype_map = new HashMap<String, Map<String, Integer>>();
		Map<String, Map<String, Map<String, Integer>>> from_to_stype_map = new HashMap<String, Map<String, Map<String, Integer>>>();

		public void reduce(LongWritable key, Iterable<Text> values,
				Context context) throws IOException, InterruptedException {
			String out = "";
//			out += "1.0\tn\t";
			out += "n\t1.0\t";
			HashMap<String, Float> scores = new HashMap<String, Float>();
			float allscore = 0.0f;
			for (Text val : values) {
				String valueline = val.toString();
				String[] tokens = valueline.split("\t");
				float score = Float.parseFloat(tokens[1]);
				scores.put(tokens[0], score);
				allscore += score;
			}
			for (Map.Entry<String, Float> e : scores.entrySet()) {
				out += e.getKey() + "\t" + e.getValue() / allscore + "\t";
			}
			context.write(key, new Text(out));
		}
	}

	public int run(String[] args) throws Exception {
		Configuration conf = getConf();
		Job job = new Job(conf, "PreparePeopleRank");
		job.setJarByClass(PreparePeopleRank.class);
		Path clickpath = new Path(args[0]);

		Path outpath = new Path(args[1]);

		FileInputFormat.setInputPaths(job, clickpath);
		FileOutputFormat.setOutputPath(job, outpath);

		job.setMapperClass(MapClass.class);
		job.setReducerClass(Reduce.class);

		job.setInputFormatClass(TextInputFormat.class);
		job.setOutputFormatClass(TextOutputFormat.class);
		job.setMapOutputKeyClass(LongWritable.class);
		job.setMapOutputValueClass(Text.class);
		job.setOutputKeyClass(Text.class);
		job.setOutputValueClass(Text.class);

		System.exit(job.waitForCompletion(true) ? 0 : 1);

		return 0;
	}

	public static void main(String[] args) throws Exception {
		// System.out.println("path:"+System.getProperty("java.classpath"));

		int res = ToolRunner.run(new Configuration(), new PreparePeopleRank(),
				args);

		System.exit(res);
	}
}