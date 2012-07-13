package com.renren.search2.logger.util;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.StringReader;

import org.htmlparser.lexer.InputStreamSource;

public class Analyze {
	private static boolean isWanted(String s) {
		boolean result = false;
		result = isWeight(s) || isQueryWeight(s) || isFieldWeight(s)
				|| isQueryNorm(s) || isFieldNorm(s) || isBoost(s) || isIdf(s)
				|| isQueryNorm(s) || isTf(s) || isFieldNorm(s);
		return result;
	}

	private static boolean isWeight(String s) {
		return s.contains("(MATCH) weight");
	}

	private static boolean isQueryWeight(String s) {
		return s.contains("queryWeight");
	}

	private static boolean isFieldWeight(String s) {
		return s.contains("(MATCH) fieldWeight");
	}

	private static boolean isBoost(String s) {
		return s.contains(" = boost");
	}

	private static boolean isIdf(String s) {
		return s.contains(" = idf");
	}

	private static boolean isQueryNorm(String s) {
		return s.contains(" = queryNorm");
	}

	private static boolean isTf(String s) {
		return s.contains(" = tf");
	}

	private static boolean isFieldNorm(String s) {
		return s.contains(" = fieldNorm");
	}

	public static String parse(String content) {
		try {
			BufferedReader br = new BufferedReader(new StringReader(content));
			StringBuilder sb = new StringBuilder();
			String line = "";
			while ((line = br.readLine()) != null) {
				if (isWeight(line)) {
					line = line.replaceAll("<li>", "");
					line = line.replaceAll("<ul>", "");
					line = line.replaceAll("<br />", "");
					sb.append(line + "|");
				}
			}
			return sb.toString();
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		return "";
	}

	public static void main(String[] args) {

	}
}
