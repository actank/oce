package xce.tools.objectcache;

import java.util.List;
import java.util.concurrent.BlockingQueue;

public class SpecificIdsAction extends Producer {

	private final String[] args;

	protected SpecificIdsAction(BlockingQueue<Long[]> queue, String[] args) {
		super(queue);
		this.args = args;
	}

	@Override
	protected boolean produce(List<Long> emptyData) {
		for (int i = 1; i < args.length; ++i) {
			try {
				Long userId = Long.valueOf(args[i]);
				emptyData.add(userId);
				System.out.println("Produced " + userId);
			} catch (NumberFormatException e) {
				e.printStackTrace();
			}
		}
		
		return false;
	}

}
