package com.xiaonei.xce.generalsuggestion.test;

import java.util.ArrayList;
import java.util.List;
import java.util.Map;
import java.util.Map.Entry;
import java.util.concurrent.Callable;
import java.util.concurrent.ExecutionException;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;
import java.util.concurrent.Future;

public class PerformanceTest {
	private final ExecutorService executor;
	private Performance tester;
	private List<Future<Integer>> results;
	
	public PerformanceTest(int poolSize) {
		executor = Executors.newFixedThreadPool(poolSize);
		tester = new Performance();
		results = new ArrayList<Future<Integer>>();
	}
	
	public void searchTest(final int id, final String condition, final int prefixLength) {
		Future<Integer> result = executor.submit(new Callable<Integer>() {
			@Override
			public Integer call() throws Exception {
				int res = tester.search(id, condition, prefixLength);
				return res;
			}
		});
		results.add(result);
//		int res = tester.search(id, condition, prefixLength);
//		System.out.println("结果数: " + res);
	}
	
	public List<Future<Integer>> getResults() {
		return results;
	}
	
	public static void main(String[] args) {
		if (args.length < 3) {
			System.out.println("./Performance: prefixLength threadPoolSize isPinyinTest");
			System.exit(0);
		}
		System.setProperty("isTestSearchChannel", "true");
		PinYinDict.getInstance().init();
		int prefixLen = Integer.parseInt(args[0]);
		int poolSize = Integer.parseInt(args[1]);
		boolean isPinyinTest = args[2].equals("true");
		
		PerformanceTest tester = new PerformanceTest(poolSize);
		Map<Integer, String> id2Name = Performance.getFriendOfFriendName(193577421);
		long start = System.currentTimeMillis();
		for (Entry<Integer, String> entry : id2Name.entrySet()) {
			int id = entry.getKey();
			String name = entry.getValue();
			if (isPinyinTest) {
				List<String> pinyins = Performance.getAllPinyin(name);
				for (String pinyin : pinyins) {
					tester.searchTest(id, pinyin, prefixLen);
				}
			} else {
				tester.searchTest(id, name, prefixLen);
//				try {
//					Thread.sleep(100);
//				} catch (InterruptedException e) {
//					e.printStackTrace();
//				}
			}
		}
		List<Future<Integer>> result = tester.getResults();
		for (Future<Integer> future : result) {
			try {
				System.out.println("结果数: " + future.get());
			} catch (InterruptedException e) {
				e.printStackTrace();
			} catch (ExecutionException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
		}
		System.out.println("测试结束, 时间: " + (System.currentTimeMillis() - start) + "ms");
		System.exit(0);
	}
}
