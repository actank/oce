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
import org.apache.hadoop.io.FloatWritable;
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
import org.apache.hadoop.io.*;
//import org.apache.hadoop.mapred.lib.MultipleInputs;
import org.apache.hadoop.mapreduce.lib.input.MultipleInputs;


//import FilterBad.MyCount;


public class SortPeopleRank extends Configured implements Tool {
	static enum MapCount { CLICK,REPLY, NEWS,GOODACTOR,GOODSTYPE,BADACTOR,BADSTYPE,ALL };

	public static class MapClass extends
			Mapper<LongWritable, Text, FloatWritable, LongWritable> {

		Map<Long,Float> target_claim = new HashMap<Long,Float>(); 


		
		//输入格式 nid n selfscore t1 s1 t2 s2;
		public void map(LongWritable key, Text value, Context context)
				throws IOException, InterruptedException {
			String valueline = value.toString();
			String keyline = key.toString();
			String[] tokens = valueline.split("\t");
			long nid =Long.parseLong(tokens[0]);
			float selfscore = Float.parseFloat(tokens[2]);

			
			context.write(new FloatWritable(selfscore),new LongWritable( nid));
		}

	}

	public static class Reduce extends Reducer<FloatWritable, LongWritable, LongWritable, FloatWritable> {
		static enum ReduceCount { VALID,INVALID};

		public void reduce(FloatWritable key, Iterable<LongWritable> values,
				Context context) throws IOException, InterruptedException {
			for( LongWritable val:values){
				context.write(val, key);
			}
		}
	}

	public int run(String[] args) throws Exception {
		Configuration conf = getConf();
		Job job = new Job(conf, "SortPeopleRank");
		job.setJarByClass(SortPeopleRank.class);
		Path clickpath = new Path(args[0]);

		Path outpath = new Path(args[1]);

		FileInputFormat.setInputPaths(job, clickpath);
		FileOutputFormat.setOutputPath(job, outpath);		
		
		job.setMapperClass(MapClass.class);
		job.setReducerClass(Reduce.class);
		
		job.setInputFormatClass(TextInputFormat.class);
		job.setOutputFormatClass(TextOutputFormat.class);
		job.setMapOutputKeyClass(FloatWritable.class);
		job.setMapOutputValueClass(LongWritable.class);
		job.setOutputKeyClass(LongWritable.class);
		job.setOutputValueClass(FloatWritable.class);
		
		System.exit(job.waitForCompletion(true) ? 0 : 1);
		
		return 0;
	}

	public static void main(String[] args) throws Exception {
//		System.out.println("path:"+System.getProperty("java.classpath"));

		int res = ToolRunner.run(new Configuration(), new SortPeopleRank(),
				args);

		System.exit(res);
	}
}