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
import org.apache.hadoop.mapred.JobConf;
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

import org.apache.hadoop.mapreduce.lib.output.NullOutputFormat;

public class UserBF extends Configured implements Tool {

	public static class MapClass extends
			Mapper<LongWritable, Text, Text, BloomFilter> {

		Set<Integer> ids = new HashSet<Integer>();
		BloomFilter filter = new BloomFilter();
		Context ctx = null;

		protected void setup(
				org.apache.hadoop.mapreduce.Mapper<LongWritable, Text, Text, BloomFilter>.Context context) {
			System.out.println("MapClass setup");

			Path file = new Path(context.getConfiguration().get(
					"mapred.output.dir")
					+ "/../blout/mybloomfilter");
			try {
				String s = new String("bc");
				Key key = new Key(s.getBytes());
//				filter.add(key);
				FSDataInputStream in = file.getFileSystem(
						context.getConfiguration()).open(file);
				int ver = in.readInt();
				int nbHash = in.readInt();
				int hashType = in.readByte();
				int vsize = in.readInt();
				System.out.println("ver:" + ver + " nbHash:" + nbHash + " hashType" + hashType 
						+" vs:" + vsize);
				filter.readFields(in);
				in.close();
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
			// String valueline = value.toString();
			// String[] tokens = valueline.split(" ");
			// if(ids.contains(Integer.parseInt(tokens[0])) ){
			// context.write(new Text(tokens[0]), new Text(tokens[1]));
			// }

			// String valueline = value.toString();
			// String keyline = key.toString();
			// System.out.println("keyline:" + keyline);
			// System.out.println("valueline:" + valueline);
			// String[] tokens = valueline.split(" ");
			// System.out.println("token size:" + tokens.length);
			// String actor = tokens[2];
			// String stype = tokens[3];
			// int tmp = Integer.parseInt(actor) / 100;
			// if (tmp == 29) {
			// return;
			// }
			// for (int i = 5; i < tokens.length; ++i) {
			// if (tokens[i].charAt(0) == 's' || tokens[i].charAt(0) == 'l') {
			// continue;
			// }
			// String toid = tokens[i];
			// if (toid == actor) {
			// continue;
			// }
			// context.write(new Text(toid), new Text(actor + ",1"));
			// context.write(new Text(toid), new Text("-" + stype + ",1"));
			//
			// }

		}

//		protected void cleanup(Mapper.Context context) throws IOException {
//			System.out.println("Map CLOSE");
//			try {
//				ctx.write(new Text("testkey"), filter);
//			} catch (InterruptedException e) {
//				e.printStackTrace();
//			}
//			// oc.collect(new Text("testkey"), filter);
//		}
		// public void close() throws IOException {
		// System.out.println("Map CLOSE");
		// try {
		// ctx.write(new Text("testkey"), filter);
		// } catch (InterruptedException e) {
		// e.printStackTrace();
		// }
		// // oc.collect(new Text("testkey"), filter);
		// }
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
		public void reduce(Text key, Iterable<BloomFilter> values,
				Context context) throws IOException, InterruptedException {

			for (BloomFilter val : values) {
				filter.or(val);
				// context.write(key, val);
			}
			// Map<String, Integer> from_type = new HashMap<String, Integer>();
			// for (Text val:values) {
			// String[] segs = val.toString().split(",");
			// String feature = segs[0];
			// int cnt = Integer.parseInt(segs[1]);
			// Integer sum = from_type.get(feature);
			// from_type.put(feature, sum == null ? cnt : cnt + sum);
			//
			// }
			// for (Map.Entry<String, Integer> entry : from_type.entrySet()) {
			// String cnt = String.valueOf(entry.getValue());
			// context.write(key, new Text(entry.getKey() + "," + cnt));
			// }

		}

//		protected void cleanup(Reducer.Context context) throws IOException {
//			System.out.println("Reduce CLOSE");
//
//			Path file = new Path(context.getConfiguration().get(
//					"mapred.output.dir")
//					+ "/mybloomfilter");
//
//			FSDataOutputStream out = file.getFileSystem(
//					context.getConfiguration()).create(file);
//			filter.write(out);
//			out.close();
//		}
		
		// public void close() throws IOException {
		// System.out.println("Reduce CLOSE");
		// Path file = new Path(job.getConfiguration().get("mapred.output.dir")
		// + "/mybloomfilter");
		//
		// FSDataOutputStream out =
		// file.getFileSystem(job.getConfiguration()).create(file);
		// filter.write(out);
		// out.close();
		// }
	}

	public int run(String[] args) throws Exception {
		Configuration conf = getConf();
		Job job = new Job(conf, "MyJob");
		job.setJarByClass(UserBF.class);
		Path in = new Path(args[0]);
		Path out = new Path(args[1]);
		FileInputFormat.setInputPaths(job, in);
		FileOutputFormat.setOutputPath(job, out);
		job.setMapperClass(MapClass.class);
		job.setReducerClass(Reduce.class);
		// job.setReducerClass(IdentityReducer.class);

		job.setInputFormatClass(TextInputFormat.class);
		// job.setOutputFormatClass(TextOutputFormat.class);

		job.setOutputValueClass(BloomFilter.class);
		job.setOutputKeyClass(Text.class);
		job.setOutputFormatClass(NullOutputFormat.class);
		System.exit(job.waitForCompletion(true) ? 0 : 1);
		return 0;
	}

	public static void main(String[] args) throws Exception {
		int res = ToolRunner.run(new Configuration(), new UserBF(), args);

		System.exit(res);
	}
}