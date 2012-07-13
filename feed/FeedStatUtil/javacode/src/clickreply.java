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


public class clickreply extends Configured implements Tool {
	static enum MyCount { CLICK,REPLY, ALL };
	public static class ClickReply implements Writable {
		public void write(DataOutput out) throws IOException {
			out.writeInt(click);
			out.writeInt(reply);
		}
	    public void readFields(DataInput in) throws IOException {
	        click = in.readInt();
	        reply = in.readInt();
	    }
	    public String toString(){
	    	return Integer.toString(click) + "\t"+ Integer.toString(reply);
	    }
		public int click = 0;
		public int reply = 0;
	}

	public static class MapClass extends
			Mapper<LongWritable, Text, Text, ClickReply> {

		String actor, stype,toid;
		Pattern pattern_stype, pattern_actor;
		Matcher matcher_stype, matcher_actor;

		Map<String, ClickReply> clickmap = new HashMap<String, ClickReply>();

		private void insert(String to, String feature, int click, int reply) {
//			String tofeature = Long.toString(to) + "\t" 
//					+ Long.toString(feature);
			String tofeature = to + "\t" + feature;
			ClickReply cr = clickmap.get(tofeature);
			if (cr == null) {
				ClickReply tmp = new ClickReply();
				tmp.click += click;
				tmp.reply += reply;
				clickmap.put(tofeature, tmp);
			} else {
				cr.click += click;
				cr.reply += reply;
			}
		}

		public void map(LongWritable key, Text value, Context context)
				throws IOException, InterruptedException {
			
			String valueline = value.toString();
			String keyline = key.toString();
			String[] tokens = valueline.split(" ");
			if(tokens.length >= 4 && tokens[3].charAt(0) == 'h'){				
				//1305807631463 254717103 218.10.70.116 http://photo.renren.com/photo/251299271/photo-4374292486?ref=hotnewsfeed&sfet=709&fin=0&ff_id=251299271&_resourceId=/photo/{owner}/photo-{id}
				toid = tokens[1];
				if(toid.equals("0")){
					return;
				}	
				pattern_stype = Pattern.compile("sfet=(\\d+)");
				matcher_stype = pattern_stype.matcher(tokens[3]);
				if (matcher_stype.find()) {
					stype = matcher_stype.group(1);
				} else {
					return;
				}
				pattern_actor = Pattern.compile("ff_id=(\\d+)");
				matcher_actor = pattern_actor.matcher(tokens[3]);
				if (matcher_actor.find()) {
					actor = matcher_actor.group(1);
				} else {
					return;
				}
				if (actor.equals(toid)) {					
					return;
				}
		
				insert(toid,
						actor, 1, 0);
				insert(toid,
						"-"+stype, 1, 0);
				context.getCounter(MyCount.CLICK).increment(1);
			}			else if (tokens[0].equals("r")) {
				// r 221707350(to) 239326012(from) 708(stype) 4401807988
				//r 1311217197(time) 343213596(to) 343213596(from) 502(stype) 2233844271 10986043328
				context.getCounter(MyCount.REPLY).increment(1);
				insert(tokens[2],
						tokens[3], 0, 1);
				insert(tokens[2],
						"-"+tokens[4], 0, 1);
			} 

		}

		protected void cleanup(Mapper.Context context) throws IOException {
//			System.out.println("Map CLOSE,clickmap size:" + clickmap.size());
			try {
				Iterator it = clickmap.entrySet().iterator();
				while (it.hasNext()) {
					Map.Entry pairs = (Map.Entry) it.next();
					
					context.write(new Text(pairs.getKey().toString()), pairs.getValue());
					System.out.println("key," + pairs.getKey());
				}
			} catch (InterruptedException e) {
				e.printStackTrace();
			}
			// oc.collect(new Text("testkey"), filter);
		}
	}

	public static class Reduce extends Reducer<Text, ClickReply, Text, Text> {

		Map<String, Map<String, Integer>> from_stype_map = new HashMap<String, Map<String, Integer>>();
		Map<String, Map<String, Map<String, Integer>>> from_to_stype_map = new HashMap<String, Map<String, Map<String, Integer>>>();

		Map<String, ClickReply> clickmap = new HashMap<String, ClickReply>();

		public void reduce(Text key, Iterable<ClickReply> values,
				Context context) throws IOException, InterruptedException {
			Map<String, Integer> from_type = new HashMap<String, Integer>();
			ClickReply cr = new ClickReply();
			for (ClickReply val : values) {
				cr.click += val.click;
				cr.reply += val.reply;
			}
			context.write(key, new Text(cr.toString()));
			// for (Map.Entry<String, Integer> entry : from_type.entrySet()) {
			// String cnt = String.valueOf(entry.getValue());
			// context.write(key, new Text(entry.getKey() + "," + cnt));
			// }

		}
	}

	public int run(String[] args) throws Exception {
		Configuration conf = getConf();
		Job job = new Job(conf, "MyJob");
		job.setJarByClass(clickreply.class);
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
		job.setMapOutputValueClass(ClickReply.class);
		job.setOutputKeyClass(Text.class);
		job.setOutputValueClass(Text.class);
		System.exit(job.waitForCompletion(true) ? 0 : 1);
		return 0;
	}

	public static void main(String[] args) throws Exception {
		System.out.println("path:"+System.getProperty("java.classpath"));

		int res = ToolRunner.run(new Configuration(), new clickreply(),
				args);

		System.exit(res);
	}
}