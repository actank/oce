package worker;

import parse.CommonParse;
import parse.ProcessBlog;

public class BlogPreprocess {
	private static BlogPreprocess instance = null;
	
	public static BlogPreprocess getInstance() {
		if (null == instance) {
			synchronized (BlogPreprocess.class) {
				if (null == instance) {
					try {
						instance = new BlogPreprocess();
					} catch (Exception e) {
						e.printStackTrace();
						instance = null;
					}
				}
			}
		}
		return instance;
	}
	
	private BlogPreprocess() {
	}
	
	public void preprocess(String source, String dest) {
		System.out.println("[BlogRecommend] Preprocess source:" + source);
		System.out.println("[BlogRecommend] Preprocess dest:" +dest);
		
		ProcessBlog process = new CommonParse();
		
		//处理文本
		if (process.process(source, dest)) {
			System.out.println("parse file success!");
		} else {
			System.err.println("parse file error!");
		}
	}
	
	public static void main(String[] args) {
		if (2 != args.length) {
			System.err.println("error input args!!");
			return;
		}
		
		BlogPreprocess.getInstance().preprocess(args[0], args[1]);		//数据预处理
		System.exit(0);
	}
}
