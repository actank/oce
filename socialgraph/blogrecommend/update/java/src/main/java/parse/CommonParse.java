package parse;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.OutputStreamWriter;
import java.io.PrintWriter;
import java.util.ArrayList;
import java.util.List;
import java.util.regex.Pattern;

import org.apache.commons.lang.StringEscapeUtils;

public class CommonParse implements ProcessBlog {
	private static int LIMIT = 25;		//title和日志概要控制在25个字之内
	private static int NUM = 9;			//源文件的列数
	
	private String input;		//输入源文件
	private String output;		//输出目标文件
	
	private BufferedReader reader; 		//读取文件信息
	private PrintWriter writer;		//写入目标文件

	/**
	 * 继承统一的接口，处理文本
	 */
	public boolean process(String input, String output) {
		this.input = input;
		this.output = output;
		
		try {
			initialize();		//初始化
		} catch (FileNotFoundException e) {
			e.printStackTrace();
			return false;
		}
		
		return parse();		//处理文本
	}
	
	/**
	 * 初始化文件读写对象
	 * @throws FileNotFoundException
	 */
	private void initialize() throws FileNotFoundException {
		File fInput = new File(this.input);
		if (!fInput.exists()) {
			System.err.println(this.input + " doesn't exist!!");
			return;
		}
		File fOutput = new File(this.output);
		if (fOutput.exists()) {
			fOutput.delete();
		}
		reader = new BufferedReader(new InputStreamReader(new FileInputStream(this.input)));
		writer = new PrintWriter(new OutputStreamWriter(new FileOutputStream(this.output, true)), true);
	}
	
	
	/**
	 * 遍历文件，生成目标文件
	 */
	private boolean parse() {
		boolean result = true;
		String line;
		try {
			line = reader.readLine();
			while (null != line) {
				String[] itemList = line.split("");			//以该特殊字符为分割符分割文件
				if (NUM == itemList.length) {
					String title = html2Text(itemList[6], LIMIT);
					String content = html2Text(itemList[7], LIMIT);
					List<String> stringList = new ArrayList<String>();
					for (int i = 0; i < 6; ++i) {
						stringList.add(itemList[i]);
					}
					stringList.add(title);
					stringList.add(content);
					stringList.add(itemList[8]);
					write(stringList);
				}
				line = reader.readLine();
			}
		} catch (FileNotFoundException e) {
			e.printStackTrace();
			result = false;
		} catch (IOException e) {
			e.printStackTrace();
			result = false;
		} finally {
			writer.close();
		}		
		return result;
	}

	/**
   * 过滤文本
   * 
   * @param inputString
   *            要过滤的文本
   * @param count
   *            文本要显示的字数
   * @return
   */
  private String html2Text(String inputString, int count) {
      String htmlStr = inputString; // 含html标签的字符串
      String textStr = "";
      java.util.regex.Pattern p_script;
      java.util.regex.Matcher m_script;
      java.util.regex.Pattern p_style;
      java.util.regex.Matcher m_style;
      java.util.regex.Pattern p_html;
      java.util.regex.Matcher m_html;
      try {
          String regEx_script = "<[\\s]*?script[^>]*?>[\\s\\S]*?<[\\s]*?\\/[\\s]*?script[\\s]*?>"; // 定义script的正则表达式{或<script[^>]*?>[\\s\\S]*?<\\/script>
          // }
          String regEx_style = "<[\\s]*?style[^>]*?>[\\s\\S]*?<[\\s]*?\\/[\\s]*?style[\\s]*?>"; // 定义style的正则表达式{或<style[^>]*?>[\\s\\S]*?<\\/style>
          // }
          String regEx_html = "<[^>]+>"; // 定义HTML标签的正则表达式

          p_script = Pattern.compile(regEx_script, Pattern.CASE_INSENSITIVE);
          m_script = p_script.matcher(htmlStr);
          htmlStr = m_script.replaceAll(""); // 过滤script标签

          p_style = Pattern.compile(regEx_style, Pattern.CASE_INSENSITIVE);
          m_style = p_style.matcher(htmlStr);
          htmlStr = m_style.replaceAll(""); // 过滤style标签

          p_html = Pattern.compile(regEx_html, Pattern.CASE_INSENSITIVE);
          m_html = p_html.matcher(htmlStr);
          htmlStr = m_html.replaceAll(""); // 过滤html标签

          textStr = htmlStr;// replaceAll(" ", "");
          textStr = textStr.replaceAll("&hellip;&hellip;", "……");
          textStr = textStr.replaceAll("&nbsp;", "").trim();
      } catch (Exception e) {
      	e.printStackTrace();
      }
      if (textStr.length() > count){
          textStr = StringEscapeUtils.unescapeHtml(textStr);
          if(textStr.length() > count)
          textStr= textStr.substring(0, count) + "...";
          return textStr;
      } 
      return textStr.replaceAll("\n", "");// 返回文本字符串
  }
  
  /**
   * 向目标文件中写入数据
   * @param stringList 要写入的文本
   */
  private void write(List<String> stringList) {
		for (String item : stringList) {
			writer.write(item + "");
		}
		writer.write("\n");
		writer.flush();
  }
}
