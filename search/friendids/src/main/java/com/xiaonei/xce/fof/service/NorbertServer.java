package com.xiaonei.xce.fof.service;

import java.io.File;
import java.io.FilenameFilter;
import java.net.DatagramSocket;
import java.net.InetAddress;
import java.net.InetSocketAddress;
import java.net.MalformedURLException;
import java.net.URL;
import java.net.URLClassLoader;
import java.util.HashSet;
import java.util.List;

import javax.management.StandardMBean;

import org.apache.log4j.Logger;

import scala.actors.threadpool.Arrays;

import com.linkedin.norbert.javacompat.cluster.ClusterClient;
import com.linkedin.norbert.javacompat.cluster.Node;
import com.linkedin.norbert.javacompat.network.NetworkServer;
import com.linkedin.norbert.network.NetworkingException;
import com.xiaonei.search.tools.JmxUtil;
import com.xiaonei.search.tools.NorbertServerBuilder;
import com.xiaonei.xce.fof.service.impl.CoreFriendOfFriendServiceImpl;
import com.xiaonei.xce.fof.service.impl.FriendCore;
import com.xiaonei.xce.fof.service.impl.PreloadFriendOfFriendServiceImpl;

public class NorbertServer {
	private static final Logger logger = Logger.getLogger(NorbertServer.class);

	private static final String AVAILABLE = "available";
	private static final String UNAVAILABLE = "unavailable";
	private static final String DUMMY_OUT_IP = "74.125.224.0";

	private int _id;
	private int _port;
	private int[] _partitions;
	private NetworkServer _networkServer;
	private ClusterClient _clusterClient;
	protected volatile Node _serverNode;
	private FriendCore _core;
	private final CoreFriendOfFriendServiceImpl _innerSvc;
	private final List<AbstractNorbertCoreService<AbstractNorbertRequest, AbstractNorbertResult>> _externalSvc;

	// private Server _adminServer;

	protected volatile boolean _available = false;

	public NorbertServer(
			int id,
			int port,
			int[] partitions,
			NetworkServer networkServer,
			ClusterClient clusterClient,
			List<AbstractNorbertCoreService<AbstractNorbertRequest, AbstractNorbertResult>> externalSvc) {
		_id = id;
		_port = port;
		_partitions = partitions;
		_networkServer = networkServer;
		_clusterClient = clusterClient;
		_externalSvc = externalSvc;
		_core = new FriendCore(_id,_partitions);
		_innerSvc = new CoreFriendOfFriendServiceImpl(_core);
	}

	private static String help() {
		StringBuffer buffer = new StringBuffer();
		buffer.append("Usage: <conf.dir> [availability]\n");
		buffer.append("====================================\n");
		buffer.append("conf.dir - server configuration directory, required\n");
		buffer.append("availability - \"available\" or \"unavailable\", optional default is \"available\"\n");
		buffer.append("====================================\n");
		return buffer.toString();
	}

	public void shutdown() {
		try {
			logger.info("shutting down node...");
			try {
				_clusterClient.removeNode(_id);
				_clusterClient.shutdown();
				_serverNode = null;
			} catch (Exception e) {
				logger.warn(e.getMessage());
			} finally {
				if (_networkServer != null) {
					_networkServer.shutdown();
				}
			}
		} catch (Exception e) {
			logger.error(e.getMessage(), e);
		}
	}

	public void start(boolean available) throws Exception {
		// ClusterClient clusterClient =
		// ClusterClientFactory.newInstance().newZookeeperClient();
		String clusterName = _clusterClient.getServiceName();

		logger.info("Cluster Name: " + clusterName);
		logger.info("Cluster info: " + _clusterClient.toString());

		AbstractNorbertCoreService coreService = new CoreFriendOfFriendServiceImpl(
				_core);
		NorbertCoreServiceMessageHandler msgHandler = new NorbertCoreServiceMessageHandler(
				coreService);

		_networkServer.registerHandler(msgHandler,
				CoreFriendOfFriendServiceImpl.SERIALIZER);

		AbstractNorbertCoreService preloadService = new PreloadFriendOfFriendServiceImpl(
				_core);
		NorbertCoreServiceMessageHandler preloadMsgHandler = new NorbertCoreServiceMessageHandler(
				preloadService);

		_networkServer.registerHandler(preloadMsgHandler,
				PreloadFriendOfFriendServiceImpl.SERIALIZER);

		// if (_externalSvc != null) {
		// for (AbstractNorbertCoreService svc : _externalSvc) {
		// _networkServer.registerHandler(
		// new NorbertCoreServiceMessageHandler(svc),
		// svc.getSerializer());
		// }
		// }
		HashSet<Integer> partition = new HashSet<Integer>();
		for (int partId : _partitions) {
			partition.add(partId);
		}

		boolean nodeExists = false;
		try {
			logger.info("waiting to connect to cluster...");
			_clusterClient.awaitConnectionUninterruptibly();
			_serverNode = _clusterClient.getNodeWithId(_id);
			nodeExists = (_serverNode != null);
			if (!nodeExists) {
				DatagramSocket ds = new DatagramSocket();
				ds.connect(InetAddress.getByName(DUMMY_OUT_IP), 80);
				String ipAddr = (new InetSocketAddress(ds.getLocalAddress(),
						_port)).toString().replaceAll("/", "");

				logger.info("Node id : " + _id + " IP address : " + ipAddr);

				_serverNode = _clusterClient.addNode(_id, ipAddr, partition);

				logger.info("added node id: " + _id);
			} else {
				// node exists

			}
		} catch (Exception e) {
			logger.error(e.getMessage(), e);
			throw e;
		}

		try {
			logger.info("binding server ...");
			_networkServer.bind(_id, available);

			// exponential backoff
			Thread.sleep(1000);

			_available = available;
			logger.info("started [markAvailable=" + available + "] ...");
			if (nodeExists) {
				logger.warn("existing node found, will try to overwrite.");
				try {
					// remove node above
					_clusterClient.removeNode(_id);
					_serverNode = null;
				} catch (Exception e) {
					logger.error(
							"problem removing old node: " + e.getMessage(), e);
				}
				DatagramSocket ds = new DatagramSocket();
				ds.connect(InetAddress.getByName(DUMMY_OUT_IP), 80);
				String ipAddr = (new InetSocketAddress(ds.getLocalAddress(),
						_port)).toString().replaceAll("/", "");
				_serverNode = _clusterClient.addNode(_id, ipAddr, partition);
				Thread.sleep(1000);

				logger.info("added node id: " + _id);
			}
		} catch (NetworkingException e) {
			logger.error(e.getMessage(), e);

			try {
				if (!nodeExists) {
					_clusterClient.removeNode(_id);
					_serverNode = null;
				}
			} catch (Exception ex) {
				logger.warn(ex.getMessage());
			} finally {
				try {
					_networkServer.shutdown();
					_networkServer = null;

				} finally {
					_clusterClient.shutdown();
					_clusterClient = null;
				}
			}
			throw e;
		}

		NorbertServerAdminMBean norbertAdminMBean = getAdminMBean();
		StandardMBean bean = new StandardMBean(norbertAdminMBean,
				NorbertServerAdminMBean.class);
		JmxUtil.registerMBean(bean, "name", "norbert-server-" + _id);
	}

	private NorbertServerAdminMBean getAdminMBean() {
		return new NorbertServerAdminMBean() {
			public int getId() {
				return _id;
			}

			public int getPort() {
				return _port;
			}

			public String getPartitions() {
				StringBuffer sb = new StringBuffer();
				if (_partitions.length > 0)
					sb.append(String.valueOf(_partitions[0]));
				for (int i = 1; i < _partitions.length; i++) {
					sb.append(',');
					sb.append(String.valueOf(_partitions[i]));
				}
				return sb.toString();
			}

			public boolean isAvailable() {
				return NorbertServer.this.isAvailable();
			}

			public void setAvailable(boolean available) {
				NorbertServer.this.setAvailable(available);
			}
		};
	}

	public void setAvailable(boolean available) {
		if (available) {
			logger.info("making available node " + _id + " @port:" + _port
					+ " for partitions: " + Arrays.toString(_partitions));
			_networkServer.markAvailable();
			try {
				Thread.sleep(1000);
			} catch (InterruptedException e) {
			}
		} else {
			logger.info("making unavailable node " + _id + " @port:" + _port
					+ " for partitions: " + Arrays.toString(_partitions));
			_networkServer.markUnavailable();
		}
		_available = available;
	}

	public boolean isAvailable() {
		if (_serverNode != null && _serverNode.isAvailable() == _available)
			return _available;

		try {
			Thread.sleep(1000);
			_serverNode = _clusterClient.getNodeWithId(_id);
			if (_serverNode != null && _serverNode.isAvailable() == _available)
				return _available;
		} catch (Exception e) {
			logger.error(e.getMessage(), e);
		}
		_available = (_serverNode != null ? _serverNode.isAvailable() : false);

		return _available;
	}

	private static void loadJars(File extDir) {
		File[] jarfiles = extDir.listFiles(new FilenameFilter() {
			public boolean accept(File dir, String name) {
				return name.endsWith(".jar");
			}
		});

		if (jarfiles != null && jarfiles.length > 0) {
			try {
				URL[] jarURLs = new URL[jarfiles.length];
				ClassLoader parentLoader = Thread.currentThread()
						.getContextClassLoader();
				for (int i = 0; i < jarfiles.length; ++i) {
					jarURLs[i] = new URL("jar:file://"
							+ jarfiles[i].getAbsolutePath() + "!/");
				}
				URLClassLoader classloader = new URLClassLoader(jarURLs,
						parentLoader);
				Thread.currentThread().setContextClassLoader(classloader);
			} catch (MalformedURLException e) {
				logger.error("problem loading extension: " + e.getMessage(), e);
			}
		}
	}

	public static void main(String[] args) throws Exception {
		if (args.length < 1) {
			System.out.println(help());
			System.exit(1);
		}

		File confDir = null;

		try {
			confDir = new File(args[0]);
		} catch (Exception e) {
			System.out.println(help());
			System.exit(1);
		}

		boolean available = true;
		for (int i = 1; i < args.length; i++) {
			if (args[i] != null) {
				if (AVAILABLE.equalsIgnoreCase(args[i])) {
					available = true;
				}
				if (UNAVAILABLE.equalsIgnoreCase(args[i])) {
					available = false;
				}
			}
		}

		File extDir = new File(confDir, "ext");

		if (extDir.exists()) {
			logger.info("loading extension jars...");
			loadJars(extDir);
			logger.info("finished loading extension jars");
		}

		NorbertServerBuilder senseiServerBuilder = new NorbertServerBuilder(
				confDir);

		final NorbertServer server = senseiServerBuilder.buildServer();

		server.start(available);
	}

}
