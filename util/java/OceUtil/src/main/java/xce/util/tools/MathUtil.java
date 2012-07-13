package xce.util.tools;

import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.Set;

public class MathUtil {
	public static final Set<Integer> generateRandomInts(int num, int max) {
		Map<Integer, Integer> results = new HashMap<Integer, Integer>();
		while (results.size() < num) {
			results.put((int) (Math.random() * max), 1);
		}
		return results.keySet();
	}

	public static final int[] list2array(List<Integer> list) {
		Integer[] array = list.toArray(new Integer[list.size()]);
		int[] result = new int[array.length];
		for (int i = 0; i < array.length; ++i) {
			result[i] = array[i];
		}
		return result;
	}

	public static final byte[] list2array(List<Byte> list) {
		Byte[] array = list.toArray(new Byte[list.size()]);
		byte[] result = new byte[array.length];
		for (int i = 0; i < array.length; ++i) {
			result[i] = array[i];
		}
		return result;
	}
}
