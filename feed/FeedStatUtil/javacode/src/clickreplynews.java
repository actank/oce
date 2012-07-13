


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
import org.apache.hadoop.io.*;
//import org.apache.hadoop.mapred.lib.MultipleInputs;
import org.apache.hadoop.mapreduce.lib.input.MultipleInputs;

//import FilterBad.MyCount;


public class clickreplynews extends Configured implements Tool {
	static enum MapCount { CLICK,REPLY, NEWS,GOODACTOR,GOODSTYPE,BADACTOR,BADSTYPE,ALL };
	private static final String FILTERFILE = "FilterBad.BloomFilter";
	public static class ClickReplyNews implements Writable {
		public void write(DataOutput out) throws IOException {
			
			out.writeInt(click);
			out.writeInt(reply);
			out.writeInt(news);
		}
	    public void readFields(DataInput in) throws IOException {
	        click = in.readInt();
	        reply = in.readInt();
	        news = in.readInt();
	    }
	    public String toString(){
	    	return Integer.toString(click) + "\t"+ Integer.toString(reply)+"\t"+Integer.toString(news);
	    }
		public int click = 0;
		public int reply = 0;
		public int news = 0;
	}

	public static class MapClass extends
			Mapper<LongWritable, Text, Text, ClickReplyNews> {

		String actor, stype,toid;
		Pattern pattern_stype, pattern_actor;
		Matcher matcher_stype, matcher_actor;

		Map<String, ClickReplyNews> resultmap = new HashMap<String, ClickReplyNews>();

		BloomFilter filter = new BloomFilter();
		protected void setup(
				Context context) {
			System.out.println("MapClass setup");


			try {
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
		
		private void insert(String to, String feature, int click, int reply,int news) {
//			String tofeature = Long.toString(to) + "\t" 
//					+ Long.toString(feature);
			String tofeature = to + "\t" + feature;

			ClickReplyNews cr = resultmap.get(tofeature);
			if (cr == null) {
				ClickReplyNews tmp = new ClickReplyNews();
				tmp.click += click;
				tmp.reply += reply;
				tmp.news += news;
				resultmap.put(tofeature, tmp);
			} else {
				cr.click += click;
				cr.reply += reply;
				cr.news += news;
			}
		}

		public void map(LongWritable key, Text value, Context context)
				throws IOException, InterruptedException {			
			String valueline = value.toString();
			String keyline = key.toString();
			String[] tokens = valueline.split(" ");
			if(tokens[0].equals("n")){
				if(tokens.length <= 4){
					return;
				}

				String toid = tokens[1];
				String actor,stype;				
				for(int i = 2; i < tokens.length;){
					actor = tokens[i++];
					stype = tokens[i++];
					if(toid.equals(actor)){
						continue;
					}
					if(filter.membershipTest(new Key((toid+"\t"+actor).getBytes()))){
						insert(toid,
								actor, 0, 0,1);
						context.getCounter(MapCount.GOODACTOR).increment(1);
					}else{
						context.getCounter(MapCount.BADACTOR).increment(1);
					}
					if(filter.membershipTest(new Key((toid+"\t-"+stype).getBytes()))){
						insert(toid,
								"-"+stype, 0, 0,1);
						context.getCounter(MapCount.GOODSTYPE).increment(1);
					}else{
						context.getCounter(MapCount.BADSTYPE).increment(1);
					}

					context.getCounter(MapCount.NEWS).increment(1);
				}
			}
			else if(tokens.length >= 4 && tokens[3].charAt(0) == 'h'){				
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
						actor, 1, 0,0);
				insert(toid,
						"-"+stype, 1, 0,0);
				context.getCounter(MapCount.CLICK).increment(1);
			}else if (tokens[0].equals("r")) {
				// r 221707350(to) 239326012(from) 708(stype) 4401807988
				//r 1311217197(time) 343213596(to) 343213596(from) 502(stype) 2233844271 10986043328
				context.getCounter(MapCount.REPLY).increment(1);
				insert(tokens[2],
						tokens[3], 0, 1,0);
				insert(tokens[2],
						"-"+tokens[4], 0, 1,0);
			} 

		}

		protected void cleanup(Mapper.Context context) throws IOException {
//			System.out.println("Map CLOSE,clickmap size:" + clickmap.size());
			try {
				Iterator it = resultmap.entrySet().iterator();
				while (it.hasNext()) {
					Map.Entry pairs = (Map.Entry) it.next();
					
					context.write(new Text(pairs.getKey().toString()), pairs.getValue());
				}
			} catch (InterruptedException e) {
				e.printStackTrace();
			}
			// oc.collect(new Text("testkey"), filter);
		}
	}

	public static class Reduce extends Reducer<Text, ClickReplyNews, Text, Text> {
		static enum ReduceCount { VALID,INVALID};
		Map<String, Map<String, Integer>> from_stype_map = new HashMap<String, Map<String, Integer>>();
		Map<String, Map<String, Map<String, Integer>>> from_to_stype_map = new HashMap<String, Map<String, Map<String, Integer>>>();

		Map<String, ClickReplyNews> clickmap = new HashMap<String, ClickReplyNews>();

		public void reduce(Text key, Iterable<ClickReplyNews> values,
				Context context) throws IOException, InterruptedException {
			Map<String, Integer> from_type = new HashMap<String, Integer>();
			ClickReplyNews cr = new ClickReplyNews();
			for (ClickReplyNews val : values) {
				cr.click += val.click;
				cr.reply += val.reply;
				cr.news += val.news;
			}
			if(cr.click != 0 || cr.reply != 0){
				context.write(key, new Text(cr.toString()));
				context.getCounter(ReduceCount.VALID).increment(1);
			}else{
				context.getCounter(ReduceCount.INVALID).increment(1);
			}
			// for (Map.Entry<String, Integer> entry : from_type.entrySet()) {
			// String cnt = String.valueOf(entry.getValue());
			// context.write(key, new Text(entry.getKey() + "," + cnt));
			// }

		}
	}

	public int run(String[] args) throws Exception {
		Configuration conf = getConf();
		Job job = new Job(conf, "MyJob");
		job.setJarByClass(clickreplynews.class);
		Path clickpath = new Path(args[0]);
		Path replypath = new Path(args[1]);
		Path newspath = new Path(args[2]);
		Path outpath = new Path(args[3]);
//		System.out.println("In:" + in.toString() );
//		System.out.println("Out:" + out.toString() );
//		FileInputFormat.setInputPaths(job, in);
		MultipleInputs.addInputPath(job, clickpath, TextInputFormat.class);
		MultipleInputs.addInputPath(job, replypath, TextInputFormat.class);
		MultipleInputs.addInputPath(job, newspath, TextInputFormat.class);
		FileOutputFormat.setOutputPath(job, outpath);
		job.setMapperClass(MapClass.class);
		job.setReducerClass(Reduce.class);
		
//		job.setInputFormatClass(TextInputFormat.class);
		job.setOutputFormatClass(TextOutputFormat.class);
		job.setMapOutputKeyClass(Text.class);
		job.setMapOutputValueClass(ClickReplyNews.class);
		job.setOutputKeyClass(Text.class);
		job.setOutputValueClass(Text.class);
		
		job.getConfiguration().set(FILTERFILE, args[4]);		
		System.exit(job.waitForCompletion(true) ? 0 : 1);
		
		return 0;
	}

	public static void main(String[] args) throws Exception {
//		System.out.println("path:"+System.getProperty("java.classpath"));

		int res = ToolRunner.run(new Configuration(), new clickreplynews(),
				args);

		System.exit(res);
	}
}