import java.io.IOException;

import java.util.Iterator;
import java.net.URI;


import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.conf.Configured;
import org.apache.hadoop.fs.FSDataOutputStream;
import org.apache.hadoop.fs.FSDataInputStream;
import org.apache.hadoop.fs.Path;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapred.FileInputFormat;
import org.apache.hadoop.mapred.FileOutputFormat;
import org.apache.hadoop.mapred.JobClient;
import org.apache.hadoop.mapred.JobConf;
import org.apache.hadoop.mapred.KeyValueTextInputFormat;
import org.apache.hadoop.mapred.MapReduceBase;
import org.apache.hadoop.mapred.Mapper;
import org.apache.hadoop.mapred.OutputCollector;
import org.apache.hadoop.mapred.Reducer;
import org.apache.hadoop.mapred.Reporter;
import org.apache.hadoop.mapred.TextOutputFormat;
import org.apache.hadoop.mapred.lib.NullOutputFormat;
import org.apache.hadoop.util.Tool;
import org.apache.hadoop.util.ToolRunner;
import org.apache.hadoop.util.bloom.BloomFilter;

import org.apache.hadoop.mapred.lib.IdentityMapper;
import org.apache.hadoop.mapred.lib.IdentityReducer;

import org.apache.hadoop.filecache.DistributedCache;

public class MakeBloom extends Configured implements Tool {
    
	
    public static class MapClass extends MapReduceBase
//        implements Mapper<Text, Text, Text, BloomFilter<String>> {
    implements Mapper<Text, Text, Text, Text> {
        MyBloomFilter<String> bf = new MyBloomFilter<String>();
        OutputCollector<Text, MyBloomFilter<String>> oc = null;
        String flag = "Undefine";
        public void configure(JobConf conf){
        	System.out.println("configure begin");
        	try {
//                Path [] cacheFiles = DistributedCache.getLocalCacheFiles(conf);
                Path [] cacheFiles = DistributedCache.getLocalCacheArchives(conf);
                if (cacheFiles != null && cacheFiles.length > 0) {
//                    String line;
//                    String[] tokens;
                    FSDataInputStream in = cacheFiles[0].getFileSystem(conf).open(cacheFiles[0]);
                    bf.readFields(in);          
                    System.out.println("Bad man ");
                    flag = "OK";
                }else{
                    System.out.println("configure bad");
                    flag = "BAD";

                }
                System.out.println("configure done");
            } catch (IOException e) {
                System.err.println("Exception reading DistributedCache: " + e);
            }
        }
        
        public void map(Text key, Text value,
                        OutputCollector<Text, Text> output,
                        Reporter reporter) throws IOException {
            
//            if (oc == null) oc = output; 
            boolean b = bf.contains(key.toString());
            if(b){
            	output.collect(new Text(key.toString()),new Text(flag));
            }else{
            	output.collect(new Text(key.toString()),new Text(flag));
            }
            System.out.printf("%b,%s\n",b,key.toString());

        }
        
//        public void close() throws IOException {
//            oc.collect(new Text("testkey"), bf);
//        }
    }
    
    public static class Reduce extends MapReduceBase
//        implements Reducer<Text, BloomFilter<String>, Text, Text> {
    implements Reducer<Text, Text, Text, Text> {
        JobConf job = null;
        MyBloomFilter<String> bf = new MyBloomFilter<String>();
        
        public void configure(JobConf job) {
            this.job = job;
        }
        
        public void reduce(Text key, Iterator<Text> values,
                           OutputCollector<Text, Text> output,
                           Reporter reporter) throws IOException {
        	while(values.hasNext()){
        		output.collect(key, values.next());
        	}
        	output.collect(key,key);
//            while (values.hasNext()) {
//                bf.union((BloomFilter<String>)values.next());
//            }
        }
        
//        public void close() throws IOException {
//            Path file = new Path(job.get("mapred.output.dir") +
//                                 "/bloomfilter");
//            FSDataOutputStream out = file.getFileSystem(job).create(file);
//            bf.write(out);
//            out.close(); 
//        }
    }
    
    public int run(String[] args) throws Exception {
        Configuration conf = getConf(); 
        JobConf job = new JobConf(conf, MakeBloom.class);
//        System.out.println("hihi:"+args[0]);
//        System.out.println("Path:"+new Path(args[0]).toUri().toString());
//        DistributedCache.addCacheFile(new Path(args[0]).toUri(), conf);
        DistributedCache.addCacheFile(new URI("/user/jingwei.ye/stat/task/test/bloomfilter"), conf);
        DistributedCache.addCacheArchive(new URI("/user/jingwei.ye/stat/task/test/bloomfilter"), conf);

//        Path in = new Path(args[1]);
//        Path out = new Path(args[2]);
        Path in = new Path(args[0]);
        Path out = new Path(args[1]);
        FileInputFormat.setInputPaths(job, in);
        FileOutputFormat.setOutputPath(job, out);

        job.setJobName("Bloom Filter");
        job.setMapperClass(MapClass.class);
        
//        job.setMapperClass(IdentityMapper.class);
//        job.setReducerClass(Reduce.class);
        job.setReducerClass(IdentityReducer.class);
        job.setNumReduceTasks(1);
        
        job.setInputFormat(KeyValueTextInputFormat.class);
        job.setOutputFormat(TextOutputFormat.class);//空的
        job.setOutputKeyClass(Text.class);
        job.setOutputValueClass(Text.class);
//        job.setOutputValueClass(BloomFilter.class);
        job.set("key.value.separator.in.input.line", ",");
        
        JobClient.runJob(job);
        
        return 0;
    }
    
    public static void main(String[] args) throws Exception {
        int res = ToolRunner.run(new Configuration(),
                                 new MakeBloom(),
                                 args);
        
        System.exit(res);
    }
}
