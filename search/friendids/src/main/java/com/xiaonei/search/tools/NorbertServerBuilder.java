package com.xiaonei.search.tools;

import it.unimi.dsi.fastutil.ints.IntOpenHashSet;
import it.unimi.dsi.fastutil.ints.IntSet;

import java.io.File;
import java.util.LinkedList;
import java.util.List;
import java.util.Map;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

import org.apache.commons.configuration.Configuration;
import org.apache.commons.configuration.ConfigurationException;
import org.apache.commons.configuration.MapConfiguration;
import org.apache.commons.configuration.PropertiesConfiguration;
import org.apache.log4j.Logger;
import org.springframework.context.ApplicationContext;
import org.springframework.context.support.FileSystemXmlApplicationContext;
import org.springframework.core.io.Resource;

import scala.actors.threadpool.Arrays;

import com.linkedin.norbert.javacompat.cluster.ClusterClient;
import com.linkedin.norbert.javacompat.cluster.ZooKeeperClusterClient;
import com.linkedin.norbert.javacompat.network.NettyNetworkServer;
import com.linkedin.norbert.javacompat.network.NetworkServer;
import com.linkedin.norbert.javacompat.network.NetworkServerConfig;
import com.xiaonei.xce.fof.service.AbstractNorbertCoreService;
import com.xiaonei.xce.fof.service.AbstractNorbertRequest;
import com.xiaonei.xce.fof.service.AbstractNorbertResult;
import com.xiaonei.xce.fof.service.NorbertServer;

public class NorbertServerBuilder implements NorbertConfParams {

	private static Logger logger = Logger.getLogger(NorbertServerBuilder.class);
	public static final String NORBERT_PROPERTIES = "norbert.properties";
	public static final String PLUGINS = "plugins.xml";

	private final File _norbertConfFile;
	private final Configuration _norbertConf;
	private final ApplicationContext _pluginContext;

	private static ApplicationContext loadSpringContext(File confFile) {
		ApplicationContext springCtx = null;
		if (confFile.exists()) {
			springCtx = new FileSystemXmlApplicationContext("file:"
					+ confFile.getAbsolutePath());
		}
		return springCtx;
	}

	public ClusterClient buildClusterClient() {
		String clusterName = _norbertConf.getString(NORBERT_CLUSTER_NAME);
		String clusterClientName = _norbertConf
				.getString(NORBERT_CLUSTER_CLIENT_NAME);
		String zkUrl = _norbertConf.getString(NORBERT_CLUSTER_URL);
		int zkTimeout = _norbertConf.getInt(NORBERT_CLUSTER_TIMEOUT, 300000);
		ClusterClient clusterClient = new ZooKeeperClusterClient(
				clusterClientName, clusterName, zkUrl, zkTimeout);

		logger.info("Connecting to cluster: " + clusterName + " ...");
		clusterClient.awaitConnectionUninterruptibly();

		logger.info("Cluster: " + clusterName + " successfully connected ");

		return clusterClient;
	}

	private static NetworkServer buildNetworkServer(Configuration conf,
			ClusterClient clusterClient) {
		NetworkServerConfig networkConfig = new NetworkServerConfig();
		networkConfig.setClusterClient(clusterClient);
		networkConfig.setRequestThreadCorePoolSize(conf.getInt(
				SERVER_REQ_THREAD_POOL_SIZE, 20));
		networkConfig.setRequestThreadMaxPoolSize(conf.getInt(
				SERVER_REQ_THREAD_POOL_MAXSIZE, 70));
		networkConfig.setRequestThreadKeepAliveTimeSecs(conf.getInt(
				SERVER_REQ_THREAD_POOL_KEEPALIVE, 300));
		return new NettyNetworkServer(networkConfig);
	}

	static {
		try {
			org.mortbay.log.Log.setLog(new org.mortbay.log.Slf4jLog());
		} catch (Throwable t) {
			logger.error(t.getMessage(), t);
		}
	}

	public NorbertServerBuilder(File confDir, Map<String, Object> properties,
			ApplicationContext customFacetContext,
			ApplicationContext pluginContext) throws Exception {
		if (properties != null) {
			_norbertConfFile = null;
			_norbertConf = new MapConfiguration(properties);
			((MapConfiguration) _norbertConf).setDelimiterParsingDisabled(true);
		} else {
			_norbertConfFile = new File(confDir, NORBERT_PROPERTIES);
			if (!_norbertConfFile.exists()) {
				throw new ConfigurationException("configuration file: "
						+ _norbertConfFile.getAbsolutePath()
						+ " does not exist.");
			}
			_norbertConf = new PropertiesConfiguration();
			((PropertiesConfiguration) _norbertConf)
					.setDelimiterParsingDisabled(true);
			((PropertiesConfiguration) _norbertConf).load(_norbertConfFile);
		}

		if (pluginContext != null)
			_pluginContext = pluginContext;
		else
			_pluginContext = loadSpringContext(new File(confDir, PLUGINS));
	}

	public NorbertServerBuilder(File confDir, Map<String, Object> properties,
			ApplicationContext customFacetContext) throws Exception {
		this(confDir, properties, customFacetContext, null);
	}

	public NorbertServerBuilder(File confDir, Map<String, Object> properties)
			throws Exception {
		this(confDir, properties, null, null);
	}

	public NorbertServerBuilder(File confDir) throws Exception {
		this(confDir, null, null, null);
	}

	public NorbertServerBuilder(Resource confDir,
			Map<String, Object> properties,
			ApplicationContext customFacetContext,
			ApplicationContext pluginContext) throws Exception {
		_norbertConfFile = null;

		_norbertConf = new MapConfiguration(properties);
		((MapConfiguration) _norbertConf).setDelimiterParsingDisabled(true);

		// TODO: conditionally load other contexts.
		_pluginContext = pluginContext;

	}

	static final Pattern PARTITION_PATTERN = Pattern
			.compile("[\\d]+||[\\d]+-[\\d]+");

	public static int[] buildPartitions(String[] partitionArray)
			throws ConfigurationException {
		IntSet partitions = new IntOpenHashSet();
		try {
			for (int i = 0; i < partitionArray.length; ++i) {
				Matcher matcher = PARTITION_PATTERN.matcher(partitionArray[i]);
				if (!matcher.matches()) {
					throw new ConfigurationException("Invalid partition: "
							+ partitionArray[i]);
				}
				String[] partitionRange = partitionArray[i].split("-");
				int start = Integer.parseInt(partitionRange[0]);
				int end;
				if (partitionRange.length > 1) {
					end = Integer.parseInt(partitionRange[1]);
					if (end < start) {
						throw new ConfigurationException(
								"invalid partition range: " + partitionArray[i]);
					}
				} else {
					end = start;
				}

				for (int k = start; k <= end; ++k) {
					partitions.add(k);
				}
			}
		} catch (Exception e) {
			throw new ConfigurationException("Error parsing '"
					+ NORBERT_PROPERTIES + "': " + PARTITIONS + "="
					+ Arrays.toString(partitionArray), e);
		}

		return partitions.toIntArray();
	}

	public NorbertServer buildServer() throws ConfigurationException {
		int port = _norbertConf.getInt(SERVER_PORT);

		ClusterClient clusterClient = buildClusterClient();

		NetworkServer networkServer = buildNetworkServer(_norbertConf,
				clusterClient);

		int nodeid = _norbertConf.getInt(NODE_ID);
		String partStr = _norbertConf.getString(PARTITIONS);
		String[] partitionArray = partStr.split("[,\\s]+");
		int[] partitions = buildPartitions(partitionArray);
		logger.info("partitions to serve: " + Arrays.toString(partitions));

		String externalServices = _norbertConf.getString(NORBERT_PLUGIN_SVCS);
		String[] externalServicList = externalServices.split("[,\\s]+");
		List<AbstractNorbertCoreService<AbstractNorbertRequest, AbstractNorbertResult>> svcList = new LinkedList<AbstractNorbertCoreService<AbstractNorbertRequest, AbstractNorbertResult>>();
		if (externalServicList != null) {
			for (String svcName : externalServicList) {
				if (svcName != null && svcName.trim().length() > 0) {
					try {
						AbstractNorbertCoreService<AbstractNorbertRequest, AbstractNorbertResult> svc = (AbstractNorbertCoreService<AbstractNorbertRequest, AbstractNorbertResult>) _pluginContext
								.getBean(svcName);
						svcList.add(svc);
					} catch (Exception e) {
						throw new ConfigurationException(e.getMessage(), e);
					}
				}
			}
		}
		return new NorbertServer(nodeid, port, partitions, networkServer,
				clusterClient, svcList);
	}
}
