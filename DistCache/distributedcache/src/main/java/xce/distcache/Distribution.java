package xce.distcache;

/**
 * 管理id->block的对应关系，固定关系。
 * 
 * */
public final class Distribution {
    public static final Distribution instance(int blockSize) {
	return new Distribution(blockSize);
    }

    private final int _blockSize;

    private Distribution(int blockSize) {
	if (blockSize < 1) {
	    blockSize = 1;
	}
	_blockSize = blockSize;
    }

    public int get(long id) {
	return (int) (Math.abs(id) % _blockSize);
    }
}
