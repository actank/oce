import java.io.IOException;
import java.util.*;

import org.apache.hadoop.mapreduce.lib.input.FileInputFormat;
import org.apache.hadoop.mapreduce.lib.input.TextInputFormat;
import org.apache.hadoop.mapreduce.lib.output.FileOutputFormat;
import org.apache.hadoop.mapreduce.lib.output.TextOutputFormat;

import org.apache.hadoop.fs.FSDataOutputStream;
import org.apache.hadoop.fs.FSDataInputStream;
import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.conf.Configured;
import org.apache.hadoop.fs.Path;
import org.apache.hadoop.io.Text; //import org.apache.hadoop.mapred.FileInputFormat;
//import org.apache.hadoop.mapred.FileOutputFormat;
import org.apache.hadoop.mapred.JobClient;

//import org.apache.hadoop.mapred.JobConf;
import org.apache.hadoop.mapred.KeyValueTextInputFormat;
import org.apache.hadoop.mapred.MapReduceBase; //import org.apache.hadoop.mapred.Mapper;
import org.apache.hadoop.mapred.OutputCollector; //import org.apache.hadoop.mapred.Reducer;
import org.apache.hadoop.mapred.Reporter; //import org.apache.hadoop.mapred.TextOutputFormat;
import org.apache.hadoop.util.Tool;
import org.apache.hadoop.util.ToolRunner;
import org.apache.hadoop.mapreduce.Mapper;
import org.apache.hadoop.mapreduce.Reducer;
import org.apache.hadoop.mapreduce.Job;
import org.apache.hadoop.io.LongWritable;

import org.apache.hadoop.mapred.lib.IdentityMapper;
import org.apache.hadoop.mapred.lib.IdentityReducer;
import org.apache.hadoop.util.hash.Hash;
import org.apache.hadoop.util.bloom.BloomFilter;
import org.apache.hadoop.util.bloom.Filter;
import org.apache.hadoop.util.bloom.Key;
import org.apache.hadoop.fs.FileSystem;

import org.apache.hadoop.mapreduce.lib.output.NullOutputFormat;
import org.apache.hadoop.util.GenericOptionsParser;


public class FilterBad extends Configured implements Tool {
	private static final String FILTERFILE = "FilterBad.BloomFilter";
	static enum MyCount { MATCHACTOR,MATCHSTYPE,ALLACTOR,ALLSTYPE};
	public static class MapClass extends
			Mapper<LongWritable, Text, Text, Text> {

		Set<Integer> ids = new HashSet<Integer>();
		BloomFilter filter = new BloomFilter();
		Context ctx = null;

		protected void setup(
				org.apache.hadoop.mapreduce.Mapper<LongWritable, Text, Text, Text>.Context context) {
			System.out.println("MapClass setup");

//			Path file = new Path(context.getConfiguration().get(
//					"mapred.output.dir")
//					+ "/../output4/mybloomfilter");
			
			
			
//			Path file = new Path("stat/task/bloomfilter/input/mybloomfilter");
			
			try {
				
				//本地模式
//				FileSystem fs = FileSystem.get(context.getConfiguration());
//				FSDataInputStream is = fs.open(new Path(context.getConfiguration().get(FILTERFILE)));
				//集群模式1
//				FileSystem fs = FileSystem.get(context.getConfiguration());
//				FSDataInputStream is = fs.open(new Path("/user/jingwei.ye/stat/task/bloomfilter/filter/mybloomfilter"));		
				
				//集群模式2
				FileSystem fs = FileSystem.get(context.getConfiguration());
				FSDataInputStream is = fs.open(new Path(context.getConfiguration().get(FILTERFILE)));	
				
				filter.readFields(is);
				is.close();
			} catch (IOException e) {
				e.printStackTrace();
			}

			String s = new String("abc");
			Key key = new Key(s.getBytes());
			// System.out.println("key:" + key.toString() + " s:" + s);
			boolean b1 = filter.membershipTest(key);
			String s2 = new String("abc2344");
			Key key2 = new Key(s2.getBytes());
			boolean b2 = filter.membershipTest(key2);
			System.out.println("11:" + b1 + " 22:" + b2);
			// System.out.println("key:" + key.toString() + " s:" + s);
		}

		public void map(LongWritable key, Text value, Context context)
				throws IOException, InterruptedException {
			if (ctx == null) {
				ctx = context;
			}
			//n 250082199 239899520 110 240196185 701 699122634 2008
			String[] tokens = value.toString().split(" ");
			if(tokens.length <= 4){
				return;
			}
			String toid = tokens[1];
			String actor,stype;
			
			for(int i = 2; i < tokens.length;){
				actor = tokens[i++];
				stype = tokens[i++];
				if(filter.membershipTest(new Key((toid+"\t"+actor).getBytes()))){
					context.getCounter(MyCount.MATCHACTOR).increment(1);					
				}
				if(filter.membershipTest(new Key((toid+"\t-"+stype).getBytes()))){
					context.getCounter(MyCount.MATCHSTYPE).increment(1);					
				}
				context.getCounter(MyCount.ALLACTOR).increment(1);	
				context.getCounter(MyCount.ALLSTYPE).increment(1);	
			}
			

		}

	}

	public static class Reduce extends Reducer<Text, Text, Text, Text> {

		BloomFilter filter = new BloomFilter();

		// Job job = null;
		// protected void setup(Reducer.Context context) {
		// this.job = context.getj;
		// }

		// public void configure(Job job) {
		// this.job = job;
		// }
		public void reduce(Text key, Iterable<Text> values,
				Context context) throws IOException, InterruptedException {

			for(Text val:values){
				context.write(key, val);
			}
		}


	}

	public int run(String[] args) throws Exception {

		Configuration conf = getConf();
		
//		Configuration conf = new Configuration();		
//		String[] otherArgs = new GenericOptionsParser(conf, args).getRemainingArgs();
		
		Job job = new Job(conf, "FilterBad");
		job.setJarByClass(FilterBad.class);
		

		
		for(String arg : args){
			System.out.println("arg:" +arg);
		}
		Path in = new Path(args[0]);
		Path out = new Path(args[1]);
		FileInputFormat.setInputPaths(job, in);
		FileOutputFormat.setOutputPath(job, out);
		job.setMapperClass(MapClass.class);
		job.setReducerClass(Reduce.class);

		job.setInputFormatClass(TextInputFormat.class);
		job.setOutputFormatClass(TextOutputFormat.class);

		job.setOutputValueClass(Text.class);
		job.setOutputKeyClass(Text.class);
		
//		FileSystem fs = FileSystem.get(job.getConfiguration());
//		String tmpFile = "/tmp/FilterBad" + System.currentTimeMillis();
//		fs.copyFromLocalFile(new Path(args[2]), new Path(tmpFile));
		job.getConfiguration().set(FILTERFILE, args[2]);		
		
		System.exit(job.waitForCompletion(true) ? 0 : 1);
		return 0;
	}

	public static void main(String[] args) throws Exception {
		int res = ToolRunner.run(new Configuration(), new FilterBad(), args);

		System.exit(res);
	}
}