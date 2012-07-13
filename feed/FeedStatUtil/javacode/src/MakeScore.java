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
import org.apache.hadoop.util.Tool;
import org.apache.hadoop.util.ToolRunner;
import org.apache.hadoop.util.bloom.BloomFilter;
import org.apache.hadoop.util.bloom.Key;
import org.apache.hadoop.mapreduce.Mapper;
import org.apache.hadoop.mapreduce.Reducer;
import org.apache.hadoop.mapreduce.Job;
import org.apache.hadoop.io.*; //import org.apache.hadoop.mapred.lib.MultipleInputs;
import org.apache.hadoop.mapreduce.lib.input.MultipleInputs;
import org.apache.hadoop.fs.FileStatus;

//import FilterBad.MyCount;

public class MakeScore extends Configured implements Tool {
	public static boolean has_old = true;

	public static class ScoreInfo implements Writable {
		public void write(DataOutput out) throws IOException {
			out.writeFloat(score);
			out.writeBoolean(is_today);
		}

		public void readFields(DataInput in) throws IOException {
			score = in.readFloat();
			is_today = in.readBoolean();
		}

		public String toString() {
			return Float.toString(score) + "\t" + Boolean.toString(is_today);
		}

		public float score = 0;
		public boolean is_today = true;

	}

	public static class MapClass extends
			Mapper<LongWritable, Text, Text, ScoreInfo> {
		static enum MapCount {
			OLDSCORE, NEWSCORE
		};

		String actor, stype, toid;
		Pattern pattern_stype, pattern_actor;
		Matcher matcher_stype, matcher_actor;

		Map<String, ScoreInfo> resultmap = new HashMap<String, ScoreInfo>();

		BloomFilter filter = new BloomFilter();

		public static ScoreInfo sc = new ScoreInfo();

		public void map(LongWritable key, Text value, Context context)
				throws IOException, InterruptedException {
			String valueline = value.toString();
			String keyline = key.toString();
			String[] tokens = valueline.split("\t");
			// to from click reply news
			if (tokens.length == 5) {
				float score = 
					(Float.parseFloat(tokens[2]) + 3.0f	* Float.parseFloat(tokens[3]))
						/ (Float.parseFloat(tokens[4]) + 2.0f);
				sc.score = score;
				sc.is_today = true;
				context.write(new Text(tokens[0] + "\t" + tokens[1]), sc);
				context.getCounter(MapCount.NEWSCORE).increment(1);
			} else if (tokens.length == 3) {
				sc.score = Float.parseFloat(tokens[2]);
				sc.is_today = false;
				context.write(new Text(tokens[0] + "\t" + tokens[1]), sc);
				context.getCounter(MapCount.OLDSCORE).increment(1);
			}
		}
	}

	public static class Reduce extends Reducer<Text, ScoreInfo, Text, Text> {
		static enum ReduceCount {
			VALID, INVALID
		};
		float oldscore,todayscore,score;
		public void clear(){
			oldscore =0.0f;
			todayscore = 0.0f;
			score = 0.0f;
		}
		public void reduce(Text key, Iterable<ScoreInfo> values, Context context)
				throws IOException, InterruptedException {
			clear();
			
			for (ScoreInfo sc : values) {
				if (sc.is_today) {
					todayscore = sc.score;
				} else {
					oldscore = sc.score;
				}
			}
			if(todayscore != 0.0f && oldscore !=0.0f ){
				score = todayscore / 13.0f + oldscore * 12.0f / 13.0f; 
			}else if(todayscore !=0){
				score = todayscore;
			}else{
				score = oldscore * 25.0f/ 26.0f;
			}

			if (score > 0.001f) {
				context.write(key, new Text(Float.toString(score)));
				context.getCounter(ReduceCount.VALID).increment(1);
			} else {
				context.getCounter(ReduceCount.INVALID).increment(1);
			}
		}
	}

	public int run(String[] args) throws Exception {
		Configuration conf = getConf();
		Job job = new Job(conf, "MakeScore");
		job.setJarByClass(MakeScore.class);
		Path oldscorepath = new Path(args[0]);
		Path todaypath = new Path(args[1]);// 必须强制按照顺序
		Path outpath = new Path(args[2]);
		FileSystem fs = outpath.getFileSystem(conf);

		System.out.println("Exist:" + fs.exists(oldscorepath));
//		if (fs.exists(oldscorepath)) {
			FileStatus[] fss = fs.globStatus(oldscorepath);
			System.out.println("##oldscorepath size:" + fss.length);

			System.out.println("##oldscorepath:" + fss.toString() + " size:"
					+ fss.length);

			if (fss.length != 0) {
				MultipleInputs.addInputPath(job, oldscorepath,
						TextInputFormat.class);
				has_old = true;
			} else {
				has_old = false;
			}
//		}else{
//			has_old = false;
//		}
		MultipleInputs.addInputPath(job, todaypath, TextInputFormat.class);

		FileOutputFormat.setOutputPath(job, outpath);
		job.setMapperClass(MapClass.class);
		job.setReducerClass(Reduce.class);

		// job.setInputFormatClass(TextInputFormat.class);
		job.setOutputFormatClass(TextOutputFormat.class);
		job.setMapOutputKeyClass(Text.class);
		job.setMapOutputValueClass(ScoreInfo.class);
		job.setOutputKeyClass(Text.class);
		job.setOutputValueClass(Text.class);

		System.exit(job.waitForCompletion(true) ? 0 : 1);

		return 0;
	}

	public static void main(String[] args) throws Exception {
		// System.out.println("path:"+System.getProperty("java.classpath"));

		int res = ToolRunner.run(new Configuration(), new MakeScore(), args);

		System.exit(res);
	}

}