package strategy;
public interface Strategy {
	/**
	 * 创建监控邮件
	 * @param source 原始文件路径
	 * @param target 产生的监控邮件路径
	 */
	void createMail(String source, String target);
}
