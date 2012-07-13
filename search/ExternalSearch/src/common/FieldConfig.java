package common;

public class FieldConfig {
	public static int omitTf = 0;
	public static int multiField = 1;

	public static boolean isOmitTf(int config) {
		return (config & (1 << omitTf)) > 0;
	}

	public static boolean isMultiField(int config) {
		return (config & (1 << multiField)) > 0;
	}
}
