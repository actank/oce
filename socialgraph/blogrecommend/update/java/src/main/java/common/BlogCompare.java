package common;

import java.util.Comparator;

public class BlogCompare implements Comparator<Object> {
	public int compare(Object o1, Object o2) {
		// TODO Auto-generated method stub
		Blog b1 = (Blog)o1;
		Blog b2 = (Blog)o2;
		if (b1.weight < b2.weight) {
			return 1;
		} else {
			return 0;
		}
	}
}
