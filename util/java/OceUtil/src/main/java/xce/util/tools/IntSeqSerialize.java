package xce.util.tools;

import java.util.ArrayList;
import java.util.List;
/**
 * int数组的序列化和反序列化
 * @author zhangnan
 *
 */
public class IntSeqSerialize {
	/**
	 * 将int数组序列化为byte数组
	 * @param seq
	 * @return
	 */
	public static byte[] serialize(final List<Integer> seq) {
		int size = 4 * seq.size();		//int占4个字节
		byte[] data = new byte[size];
		for (int i = 0; i < seq.size(); ++i) {
			data[i * 4] = (byte)(seq.get(i) & 0xff);		//最低位
			data[i * 4 + 1] = (byte)((seq.get(i) >> 8) & 0xff);		//次底位
			data[i * 4 + 2] = (byte)((seq.get(i) >> 16) & 0xff);		//次高位
			data[i * 4 + 3] = (byte)((seq.get(i) >> 24) & 0xff);		//最高位
		}
		return data;
	}
	
	/**
	 * 将byte数组反序列化为int数组
	 * @param data
	 * @return
	 */
	public static List<Integer> unserialize(final byte[] data) {
		List<Integer> intList = new ArrayList<Integer>();
		if (null == data) {
			return intList;
		}
		int length = data.length;
		for (int i = 0; i < length; i = i + 4) {
			int value = ((data[i + 3] & 0xff) << 24) + ((data[i + 2] & 0xff) << 16)		//java采用补码形式存储byte
					+ ((data[i + 1] & 0xff) << 8) + (data[i] & 0xff);
			intList.add(value);
		}
		return intList;
	}
}
