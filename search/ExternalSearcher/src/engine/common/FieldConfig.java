package engine.common;

public class FieldConfig {
	public static int omitTf = 1 << 0;
	public static int multiField = 1 << 1;
	public static int whiteSpace = 1 << 2;
	public static int noIndex = 1 << 3;
	public static int usePaoding = 1 << 4;
	public static int noAnalyzer = 1 << 5;

	public static boolean isWhitespace(int config) {
		return (config & whiteSpace) > 0;
	}

	public static boolean isOmitTf(int config) {
		return (config & omitTf) > 0;
	}

	public static boolean isMultiField(int config) {
		return (config & multiField) > 0;
	}

	public static boolean isNoIndex(int config) {
		return (config & noIndex) > 0;
	}

	public static boolean usePaoding(int config) {
		return (config & usePaoding) > 0;
	}

	public static boolean isNoAnalyzer(int config) {
		return (config & noAnalyzer) > 0;
	}
}
