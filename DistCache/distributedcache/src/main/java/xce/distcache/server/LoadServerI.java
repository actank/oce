package xce.distcache.server;

import java.io.IOException;
import java.util.ArrayList;
import java.util.List;

import org.apache.zookeeper.KeeperException;
import Ice.Current;

/**
 * @author guanghe.ge@opi-corp.com
 * @since 2010-08-09
 */
public class LoadServerI extends xce.distcache.server._LoadServerDisp {

	private static final long serialVersionUID = 1L;
	private static final int CONSUMERSIZE = 1;//消费线程数
	private List<LoadProducerThread>[] _producerThreads;
	private List<LoadConsumerThread> _consumerThreads = new ArrayList<LoadConsumerThread>();
	
	private String _name;
	private int _blockSize;

	protected void initialize(String name) throws IOException, KeeperException,
			InterruptedException {

		this._name = name;
		LoadClusterHelper.getInstance().initialize(name);
		_blockSize = LoadClusterHelper.getInstance().getBlockSize();
		System.out.println("LoadServerI::initialize begin! name: " + name + " _blockSize: " + _blockSize);
		_producerThreads = new List[_blockSize];
		for (int block = 0; block < _blockSize; block ++) {
			_producerThreads[block] = new ArrayList<LoadProducerThread>();
			int TTClusterSize = LoadClusterHelper.getInstance().getTTClusterSize(block);
			for (int tt = 0; tt < TTClusterSize; tt ++) {
				LoadProducerThread thread = new LoadProducerThread(block, tt);
				System.out.println("LoadServerI, TTClusterSize: " + TTClusterSize + " tt:" + tt);
				_producerThreads[block].add(thread);
			}
		}
		
		for (List<LoadProducerThread> lt : _producerThreads) {
			for (LoadProducerThread pt : lt) {
				pt.start();
			}
		}

		for (int i = 0; i < CONSUMERSIZE; i++) {
			_consumerThreads.add(new LoadConsumerThread());
		}

		for (LoadConsumerThread el : _consumerThreads) {
			el.start();
		}
		System.out.println("LoadServerI::initialize end!");
	}

	@Override
	public void loadByNode(Current current) {
		LoadClusterHelper.getInstance().initialize(_name);
		for (int block = 0; block < _blockSize; block ++) {
			if (LoadClusterHelper.getInstance().isNeedLoad(block)) {
				for (LoadProducerThread lt : _producerThreads[block]) {
					lt.invoke();
				}
			}
		}
		return;
	}

	public void close() throws InterruptedException {
	}
}