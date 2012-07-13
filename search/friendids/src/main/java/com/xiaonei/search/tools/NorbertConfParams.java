package com.xiaonei.search.tools;

import java.util.Comparator;

public interface NorbertConfParams {
	public static final String NODE_ID = "norbert.node.id";
	public static final String PARTITIONS = "norbert.node.partitions";

	public static final String SERVER_PORT = "norbert.server.port";
	public static final String SERVER_REQ_THREAD_POOL_SIZE = "norbert.server.requestThreadCorePoolSize";
	public static final String SERVER_REQ_THREAD_POOL_MAXSIZE = "norbert.server.requestThreadMaxPoolSize";
	public static final String SERVER_REQ_THREAD_POOL_KEEPALIVE = "norbert.server.requestThreadKeepAliveTimeSecs";
	
	public static final String NORBERT_CLUSTER_CLIENT_NAME = "norbert.cluster.client-name";
	public static final String NORBERT_CLUSTER_NAME = "norbert.cluster.name";
	public static final String NORBERT_CLUSTER_URL = "norbert.cluster.url";
	public static final String NORBERT_CLUSTER_TIMEOUT = "norbert.cluster.timeout";

	public static final String NORBERT_PLUGIN_SVCS = "norbert.plugin.services";
}
