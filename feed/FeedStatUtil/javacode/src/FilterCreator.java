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
import org.apache.hadoop.mapred.JobConf;
import org.apache.hadoop.mapred.KeyValueTextInputFormat;
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

import org.apache.hadoop.mapreduce.lib.output.NullOutputFormat;

public class FilterCreator extends Configured implements Tool {
	static enum MyCount { INSERTED};
	public static String DATETIME = "DATATIMETAG";
	public static final int bsize = 500000000;
	public static class MapClass extends
			Mapper<LongWritable, Text, Text, BloomFilter> {

		BloomFilter filter = new BloomFilter(FilterCreator.bsize,4,Hash.JENKINS_HASH  );
		Context ctx = null;

		protected void setup(Context context) {
			String s = new String("abc");
			Key key = new Key(s.getBytes());
			filter.add(key);
			boolean b1 = filter.membershipTest(key);
			String s2 = new String("abc2344");
			Key key2 = new Key(s2.getBytes());
			boolean b2 = filter.membershipTest(key2);
//			System.out.println("11:" + b1 + " 22:" + b2);
		}

		public void map(LongWritable key, Text value, Context context)
				throws IOException, InterruptedException {
			if (ctx == null) {
				ctx = context;
			}
			String[] tokens = value.toString().split("\t");		
			System.out.println("Size:" + tokens.length);

			String tofrom = tokens[0]+"\t"+tokens[1];
			filter.add(new Key(tofrom.getBytes()));
			context.getCounter(MyCount.INSERTED).increment(1);
//			filter.add(new Key(value.toString().getBytes()));
		}

		protected void cleanup(Mapper.Context context) throws IOException {
			System.out.println("Map CLOSE");
			try {
				ctx.write(new Text("testkey"), filter);
			} catch (InterruptedException e) {
				e.printStackTrace();
			}
			// oc.collect(new Text("testkey"), filter);
		}
//		public void close() throws IOException {
//			System.out.println("Map CLOSE");
//			try {
//				ctx.write(new Text("testkey"), filter);
//			} catch (InterruptedException e) {
//				e.printStackTrace();
//			}
//			// oc.collect(new Text("testkey"), filter);
//		}
	}

	public static class Reduce extends Reducer<Text, BloomFilter, Text, Text> {

		BloomFilter filter =  new BloomFilter(FilterCreator.bsize,4,Hash.JENKINS_HASH  );

		public void reduce(Text key, Iterable<BloomFilter> values, Context context)
				throws IOException, InterruptedException {
			System.out.println("Reduce BEGIN");
			for (BloomFilter val : values) {
				System.out.println("Reduce for loop");

				filter.or(val);
//				context.write(key, val);
			}


		}

		protected void cleanup(Reducer.Context context) throws IOException  {
			Path file = new Path(context.getConfiguration().get("mapred.output.dir") + "/bloomfilter." +
					context.getConfiguration().get(DATETIME));			

			FSDataOutputStream out = file.getFileSystem(context.getConfiguration()).create(file);
			filter.write(out);
			out.close();
		}
//		public void close() throws IOException {
//			System.out.println("Reduce CLOSE");
//			Path file = new Path(job.getConfiguration().get("mapred.output.dir") + "/mybloomfilter");			
//
//			FSDataOutputStream out = file.getFileSystem(job.getConfiguration()).create(file);
//			filter.write(out);
//			out.close();
//		}
	}

	public int run(String[] args) throws Exception {
		Configuration conf = getConf();
		Job job = new Job(conf, "MyJob");
		job.setJarByClass(FilterCreator.class);
		Path in = new Path(args[0]);
		Path out = new Path(args[1]);		
		FileInputFormat.setInputPaths(job, in);
		FileOutputFormat.setOutputPath(job, out);
		job.setMapperClass(MapClass.class);
		job.setReducerClass(Reduce.class);
		// job.setReducerClass(IdentityReducer.class);


//		job.setOutputFormatClass(TextOutputFormat.class);

		
		job.setOutputKeyClass(Text.class);
		job.setOutputValueClass(BloomFilter.class);

		job.setInputFormatClass(TextInputFormat.class);
		job.setOutputFormatClass(NullOutputFormat.class);
		job.setNumReduceTasks(1);
		job.getConfiguration().set(DATETIME, args[2]);		
		System.exit(job.waitForCompletion(true) ? 0 : 1);
		return 0;
	}

	public static void main(String[] args) throws Exception {
		int res = ToolRunner.run(new Configuration(), new FilterCreator(), args);

		System.exit(res);
	}
}