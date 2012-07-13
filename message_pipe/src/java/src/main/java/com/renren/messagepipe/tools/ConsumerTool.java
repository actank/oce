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
import java.util.List;

import com.renren.messagepipe.consumer.Consumer;
import com.renren.messagepipe.producer.MessageData;

/**
 * A simple tool for consuming messages
 * 
 * 
 */
public class ConsumerTool extends Consumer {

	public ConsumerTool(String cluster, String subject) {
		super(cluster, subject);
	}

	private static int parallelThreads = 1;
	private static String subject = "FUCK";
	private static String cluster = "TEST";
	private long messagesReceived = 0;
	private static int types = 1;

	public static void main(String[] args) {
		ArrayList<ConsumerTool> threads = new ArrayList();
		ConsumerTool consumerTool = new ConsumerTool("TEST", "FUCK");
		CommandLineSupport.setOptions(consumerTool, args);
		for (int threadCount = 1; threadCount <= parallelThreads; threadCount++) {
			for (int type = 0; type < types; ++type) {
				ConsumerTool tool = new ConsumerTool(cluster, subject + "_"
						+ type);
				threads.add(tool);
			}
			// CommandLineSupport.setOptions(consumerTool, args);

		}
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

	@Override
	public List<MessageData> onMessage(List<MessageData> receiveMessages) {
		System.out.println("receive Message size :" + receiveMessages.size());
		messagesReceived += receiveMessages.size();
		return null;
	}

	public void setTypes(int types) {
		this.types = types;
	}

}
