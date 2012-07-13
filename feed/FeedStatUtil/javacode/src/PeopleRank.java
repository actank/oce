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
import org.apache.hadoop.io.*;
//import org.apache.hadoop.mapred.lib.MultipleInputs;
import org.apache.hadoop.mapreduce.lib.input.MultipleInputs;


//import FilterBad.MyCount;


public class PeopleRank extends Configured implements Tool {
	static enum MapCount { CLICK,REPLY, NEWS,GOODACTOR,GOODSTYPE,BADACTOR,BADSTYPE,ALL };

	public static class MapClass extends
			Mapper<LongWritable, Text, Text, Text> {

		Map<Long,Float> target_claim = new HashMap<Long,Float>(); 

		protected void setup(
				Context context) {
		}
		public void InsertTargetClaim(long target,float claim){
			System.out.println("InsertTargetClaim. target:" + target +" claim:" + claim);
			if(target_claim.containsKey(target)){
				target_claim.put(target, target_claim.get(target)+claim);
			}else{
				target_claim.put(target, claim);
			}
		}
		
		//输入格式 nid n selfscore t1 s1 t2 s2;
		public void map(LongWritable key, Text value, Context context)
				throws IOException, InterruptedException {
			String valueline = value.toString();
			String keyline = key.toString();
			String[] tokens = valueline.split("\t");
			long nid =Long.parseLong(tokens[0]);
			float selfscore = Float.parseFloat(tokens[2]);
			for(int i =3; i < tokens.length; ){
				long target = Long.parseLong(tokens[i++]);
				float claim = Float.parseFloat(tokens[i++]);
				claim *= selfscore;
				InsertTargetClaim(target,claim);
			}
			String str = "";
			for(int i =1; i < tokens.length; ++i){
				str+= tokens[i] + "\t";
			}
			
			context.write(new Text(tokens[0]),new Text( str));
		}
		//nid c claim
		protected void cleanup(Mapper.Context context) throws IOException {
			try {
				Iterator it = target_claim.entrySet().iterator();
				while (it.hasNext()) {
					Map.Entry pairs = (Map.Entry) it.next();					
					context.write(new Text(pairs.getKey().toString()), new Text("c\t"+pairs.getValue().toString()));
					System.out.println("cleanup. target:"+pairs.getKey()
							+" claim:" +pairs.getValue().toString() );
				}
			} catch (InterruptedException e) {
				e.printStackTrace();
			}
		}

	}

	public static class Reduce extends Reducer<Text, Text, Text, Text> {
		static enum ReduceCount { VALID,INVALID};
		Map<String, Map<String, Integer>> from_stype_map = new HashMap<String, Map<String, Integer>>();
		Map<String, Map<String, Map<String, Integer>>> from_to_stype_map = new HashMap<String, Map<String, Map<String, Integer>>>();


		public void reduce(Text key, Iterable<Text> values,
				Context context) throws IOException, InterruptedException {
			
			
			
			float allscore = 0.0f;
			String nodeinfo = "";
			for( Text val:values){
				String valueline = val.toString();
				String[] tokens = valueline.split("\t");
				//输入格式 key:nid  value:n selfscore t1 s1 t2 s2;
				if(tokens[0].equals("n")){
					
					for(int i =2; i < tokens.length; ){
						nodeinfo += tokens[i++] + "\t";
						nodeinfo += tokens[i++] + "\t";
					}
				}else{
					//c claim
					allscore += Float.parseFloat(tokens[1]);
				}
			}
			allscore = 0.1f + 0.9f * allscore;
			nodeinfo = "n\t" + allscore + "\t" + nodeinfo; 
			context.write(key,new Text(nodeinfo) );

		}
	}

	public int run(String[] args) throws Exception {
		Configuration conf = getConf();
		Job job = new Job(conf, "PeopleRank");
		job.setJarByClass(PeopleRank.class);
		Path clickpath = new Path(args[0]);

		Path outpath = new Path(args[1]);

		FileInputFormat.setInputPaths(job, clickpath);
		FileOutputFormat.setOutputPath(job, outpath);		
		
		job.setMapperClass(MapClass.class);
		job.setReducerClass(Reduce.class);
		
		job.setInputFormatClass(TextInputFormat.class);
		job.setOutputFormatClass(TextOutputFormat.class);
		job.setMapOutputKeyClass(Text.class);
		job.setMapOutputValueClass(Text.class);
		job.setOutputKeyClass(Text.class);
		job.setOutputValueClass(Text.class);

		System.exit(job.waitForCompletion(true) ? 0 : 1);
		
		return 0;
	}

	public static void main(String[] args) throws Exception {
//		System.out.println("path:"+System.getProperty("java.classpath"));

		int res = ToolRunner.run(new Configuration(), new PeopleRank(),
				args);

		System.exit(res);
	}
}