import java.util.Map;

public class MyEntry<K, V> implements Map.Entry<K, V> {
	private final K key;
	private V value;

	public MyEntry(K key, V value) {
		this.key = key;
		this.value = value;
	}

	@Override
	public K getKey() {
		return key;
	}

	@Override
	public V getValue() {
		return value;
	}

	@Override
	public V setValue(V value) {
		V old = this.value;
		this.value = value;
		return old;
	}
	
	static public void main(String [] args) {
		String s = "8.556604E-4";
		float f = Float.parseFloat(s);
		
		System.out.println(f);
		System.out.println(f* 100);
		System.out.println("hh");
	}
	
}