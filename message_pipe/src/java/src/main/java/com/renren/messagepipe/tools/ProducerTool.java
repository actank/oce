package com.renren.messagepipe.tools;

/**
 * Licensed to the Apache Software Foundation (ASF) under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.
 * The ASF licenses this file to You under the Apache License, Version 2.0
 * (the "License"); you may not use this file except in compliance with
 * the License.  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
import java.util.ArrayList;
import java.util.Arrays;
import java.util.concurrent.CountDownLatch;

import com.renren.messagepipe.producer.MessageData;
import com.renren.messagepipe.producer.Producer;

/**
 * A simple tool for publishing messages
 * 
 * 
 */
public class ProducerTool extends Thread {

	private static int messageCount = 100000;
	private static int messageSize = 1024;
	private static int parallelThreads = 1;
	private String cluster = "TEST";
	private String subject = "FUCK";
	private CountDownLatch latch;
	private static int types = 1;

	public ProducerTool(CountDownLatch latch) {
		this.latch = latch;
	}

	public static void main(String[] args) throws InterruptedException {
		ArrayList<ProducerTool> threads = new ArrayList();
		ProducerTool producerTool = new ProducerTool(null);
		String[] unknown = CommandLineSupport.setOptions(producerTool, args);
		if (unknown.length > 0) {
			System.out.println("Unknown options: " + Arrays.toString(unknown));
			System.exit(-1);
		}
		System.out.println("Parallel Threads " + parallelThreads);
		CountDownLatch latch = new CountDownLatch(parallelThreads);
		long start = System.currentTimeMillis();
		for (int threadCount = 1; threadCount <= parallelThreads; threadCount++) {
			ProducerTool tools = new ProducerTool(latch);
//			CommandLineSupport.setOptions(producerTool, args);
			tools.start();
			threads.add(tools);
		}

		latch.await();
		System.out.println("All threads completed their work");
		System.out.println("Total Cost : "
				+ (System.currentTimeMillis() - start) + " ms");
	}

	public void run() {
		try {
			sendLoop();
		} catch (Exception e) {
			e.printStackTrace();
		}
		latch.countDown();
	}

	public void sendLoop() throws Exception {

		for (int i = 0; i < messageCount || messageCount == 0; i++) {
			Producer.getInstance().send(createMessageText(i));
		}
	}

	private MessageData createMessageText(int index) {
		byte[] bytes = new byte[messageSize];
		int mod = index % types;
		MessageData data = new MessageData(cluster, subject+"_"+mod, bytes);
		return data;
	}

	public void setMessageCount(int messageCount) {
		this.messageCount = messageCount;
	}

	public void setMessageSize(int messageSize) {
		this.messageSize = messageSize;
	}

	public void setSubject(String subject) {
		this.subject = subject;
	}

	public void setParallelThreads(int parallelThreads) {
		if (parallelThreads < 1) {
			parallelThreads = 1;
		}
		this.parallelThreads = parallelThreads;
	}
	

	public void setTypes(int types){
		this.types = types;
	}
}
