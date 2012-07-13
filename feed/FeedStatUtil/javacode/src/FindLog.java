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
import org.apache.hadoop.fs.Path;
import org.apache.hadoop.io.Text; //import org.apache.hadoop.mapred.FileInputFormat;
//import org.apache.hadoop.mapred.FileOutputFormat;
import org.apache.hadoop.mapred.JobClient;
import org.apache.hadoop.mapred.KeyValueTextInputFormat; //import org.apache.hadoop.mapred.Mapper;
import org.apache.hadoop.mapred.OutputCollector; //import org.apache.hadoop.mapred.Reducer;
import org.apache.hadoop.mapred.Reporter; //import org.apache.hadoop.mapred.TextOutputFormat;
import org.apache.hadoop.util.Tool;
import org.apache.hadoop.util.ToolRunner;
import org.apache.hadoop.mapreduce.Mapper;
import org.apache.hadoop.mapreduce.Reducer;
import org.apache.hadoop.mapreduce.Job;
import org.apache.hadoop.io.*;
//import org.apache.hadoop.mapred.lib.MultipleInputs;
import org.apache.hadoop.mapreduce.lib.input.MultipleInputs;


public class FindLog extends Configured implements Tool {
	static enum MyCount { CLICK,REPLY, ALL };

	public static class MapClass extends
			Mapper<LongWritable, Text, Text, Text> {

		public void map(LongWritable key, Text value, Context context)
				throws IOException, InterruptedException {
			
			String valueline = value.toString();
			String keyline = key.toString();
			
			int index1 = valueline.indexOf("220678672");
			int index2 = valueline.indexOf("239556923");

			if(index1 != -1 || index2 != -1){

				context.write(new Text("1"),value);
			}
			

		}

		
	}

	public static class Reduce extends Reducer<Text, Text, Text, Text> {
		public void reduce(Text key, Iterable<Text> values,
				Context context) throws IOException, InterruptedException {

			for (Text val : values) {
				context.write(key, val);
			}
		}
	}

	public int run(String[] args) throws Exception {
		Configuration conf = getConf();
		Job job = new Job(conf, "MyJob");
		job.setJarByClass(FindLog.class);
		Path in0 = new Path(args[0]);
		Path in1 = new Path(args[1]);
		Path out = new Path(args[2]);
//		System.out.println("In:" + in.toString() );
//		System.out.println("Out:" + out.toString() );
//		FileInputFormat.setInputPaths(job, in);
		MultipleInputs.addInputPath(job, in0, TextInputFormat.class);
		MultipleInputs.addInputPath(job, in1, TextInputFormat.class);
		FileOutputFormat.setOutputPath(job, out);
		job.setMapperClass(MapClass.class);
		job.setReducerClass(Reduce.class);
		
//		job.setInputFormatClass(TextInputFormat.class);
		job.setOutputFormatClass(TextOutputFormat.class);
		job.setMapOutputKeyClass(Text.class);
		job.setMapOutputValueClass(Text.class);
		job.setOutputKeyClass(Text.class);
		job.setOutputValueClass(Text.class);
		System.exit(job.waitForCompletion(true) ? 0 : 1);
		return 0;
	}

	public static void main(String[] args) throws Exception {
		System.out.println("path:"+System.getProperty("java.classpath"));

		int res = ToolRunner.run(new Configuration(), new FindLog(),
				args);

		System.exit(res);
	}
}