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

//import clickreplynews.MapCount;

//import FilterBad.MyCount;


public class clickreplyrecv extends Configured implements Tool {
	static enum MapCount { CLICKREPLY, RECV,GOODACTOR,GOODSTYPE,BADACTOR,BADSTYPE,ALL };
	private static final String FILTERFILE = "FilterBad.BloomFilter";
	public static class ClickReplyRecv implements Writable {
		public void write(DataOutput out) throws IOException {
			
			out.writeInt(click);
			out.writeInt(reply);
			out.writeInt(recv);
		}
	    public void readFields(DataInput in) throws IOException {
	        click = in.readInt();
	        reply = in.readInt();
	        recv = in.readInt();
	    }
	    public String toString(){
	    	return Integer.toString(click) + "\t"+ Integer.toString(reply)+"\t"+Integer.toString(recv);
	    }
		public int click = 0;
		public int reply = 0;
		public int recv = 0;
	}

	public static class MapClass extends
			Mapper<LongWritable, Text, Text, ClickReplyRecv> {

		String actor, stype,toid;
		Pattern pattern_stype, pattern_actor;
		Matcher matcher_stype, matcher_actor;

		Map<String, ClickReplyRecv> resultmap = new HashMap<String, ClickReplyRecv>();

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
		
		private void insert(String to, String feature, int click, int reply,int recv) {
//			String tofeature = Long.toString(to) + "\t" 
//					+ Long.toString(feature);
			String tofeature = to + "\t" + feature;

			ClickReplyRecv cr = resultmap.get(tofeature);
			if (cr == null) {
				ClickReplyRecv tmp = new ClickReplyRecv();
				tmp.click += click;
				tmp.reply += reply;
				tmp.recv += recv;
				resultmap.put(tofeature, tmp);
			} else {
				cr.click += click;
				cr.reply += reply;
				cr.recv += recv;
			}
		}

		public void map(LongWritable key, Text value, Context context)
				throws IOException, InterruptedException {			
			String valueline = value.toString();
			String keyline = key.toString();
			String[] tokens;
			if(valueline.charAt(0)== 'd'){
				tokens = valueline.split(" ");
			}else{
				tokens = valueline.split("\t");
			}
			if(valueline.charAt(0)== 'd'){
				//  os << "d " << seconds << " "<<feed->actor << " " << (feed->type & 0x0000FFFF) << " " << feed->feed;

				//d 1312128187(second) 361169113(actor) 2002(stype) 11199281460(fid) spage_600441106_1 l1 361169113
				for(int i = 5 ; i < tokens.length; ++i){
					if(tokens[i].charAt(0) == 's' || tokens[i].charAt(0) == 'l'){
						continue;
					}
					if(filter.membershipTest(new Key((tokens[i]+"\t"+tokens[2]).getBytes()))){
						insert(tokens[i],tokens[2],0,0,1);
						context.getCounter(MapCount.GOODACTOR).increment(1);
					}else{
						context.getCounter(MapCount.BADACTOR).increment(1);
					}
					if(filter.membershipTest(new Key((tokens[i]+"\t-"+tokens[3]).getBytes()))){
						insert(tokens[i],"-"+tokens[3],0,0,1);
						context.getCounter(MapCount.GOODSTYPE).increment(1);
					}else{
						context.getCounter(MapCount.BADSTYPE).increment(1);
					}
				}
			}else if(valueline.charAt(0) != ' '){				
				insert(tokens[0],tokens[1],Integer.parseInt(tokens[2]),Integer.parseInt(tokens[3]),0);
				context.getCounter(MapCount.CLICKREPLY).increment(1);
			}

		}

		protected void cleanup(Mapper.Context context) throws IOException {
			try {
				Iterator it = resultmap.entrySet().iterator();
				while (it.hasNext()) {
					Map.Entry pairs = (Map.Entry) it.next();
					
					context.write(new Text(pairs.getKey().toString()), pairs.getValue());
				}
			} catch (InterruptedException e) {
				e.printStackTrace();
			}

		}
	}

	public static class Reduce extends Reducer<Text, ClickReplyRecv, Text, Text> {
		static enum ReduceCount { VALID,INVALID};
		Map<String, Map<String, Integer>> from_stype_map = new HashMap<String, Map<String, Integer>>();
		Map<String, Map<String, Map<String, Integer>>> from_to_stype_map = new HashMap<String, Map<String, Map<String, Integer>>>();

		Map<String, ClickReplyRecv> clickmap = new HashMap<String, ClickReplyRecv>();

		public void reduce(Text key, Iterable<ClickReplyRecv> values,
				Context context) throws IOException, InterruptedException {
			Map<String, Integer> from_type = new HashMap<String, Integer>();
			ClickReplyRecv cr = new ClickReplyRecv();
			for (ClickReplyRecv val : values) {
				cr.click += val.click;
				cr.reply += val.reply;
				cr.recv += val.recv;
			}
//			context.write(key, new Text(cr.toString()));
			if(cr.click != 0 || cr.reply != 0){
				context.write(key, new Text(cr.toString()));
				context.getCounter(ReduceCount.VALID).increment(1);
			}else{
				context.getCounter(ReduceCount.INVALID).increment(1);
			}


		}
	}

	public int run(String[] args) throws Exception {
		Configuration conf = getConf();
		Job job = new Job(conf, "MyJob");
		job.setJarByClass(clickreplyrecv.class);
		Path clickreplypath = new Path(args[0]);
		Path dispatchpath = new Path(args[1]);
		Path outpath = new Path(args[2]);
//		System.out.println("In:" + in.toString() );
//		System.out.println("Out:" + out.toString() );
//		FileInputFormat.setInputPaths(job, in);
		MultipleInputs.addInputPath(job, clickreplypath, TextInputFormat.class);
		MultipleInputs.addInputPath(job, dispatchpath, TextInputFormat.class);
		FileOutputFormat.setOutputPath(job, outpath);
		job.setMapperClass(MapClass.class);
		job.setReducerClass(Reduce.class);
		
//		job.setInputFormatClass(TextInputFormat.class);
		job.setOutputFormatClass(TextOutputFormat.class);
		job.setMapOutputKeyClass(Text.class);
		job.setMapOutputValueClass(ClickReplyRecv.class);
		job.setOutputKeyClass(Text.class);
		job.setOutputValueClass(Text.class);
		
		job.getConfiguration().set(FILTERFILE, args[3]);		
		System.exit(job.waitForCompletion(true) ? 0 : 1);
		
		return 0;
	}

	public static void main(String[] args) throws Exception {
//		System.out.println("path:"+System.getProperty("java.classpath"));

		int res = ToolRunner.run(new Configuration(), new clickreplyrecv(),
				args);

		System.exit(res);
	}
}