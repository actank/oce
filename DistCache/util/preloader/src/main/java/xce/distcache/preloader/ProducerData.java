package xce.distcache.preloader;

import java.util.ArrayList;
import java.util.List;

public class ProducerData {
    private List<Long> ids_;
    private long max_;
    private long min_;

    public ProducerData() {
	min_ = Long.MAX_VALUE;
	max_ = Long.MIN_VALUE;
	ids_ = new ArrayList<Long>();
    }

    public ProducerData(List<Long> ids, long min, long max) {
	ids_ = ids;
	min_ = min;
	max_ = max;
    }

    public List<Long> getIds() {
	return ids_;
    }

    public long getMax() {
	return max_;
    }

    public long getMin() {
	return min_;
    }
}
