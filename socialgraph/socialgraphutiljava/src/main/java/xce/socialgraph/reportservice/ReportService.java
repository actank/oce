package xce.socialgraph.reportservice;

/**
 * ReportService 接口定义
 * 
 * @author michael
 * @email liyong@renren-inc.com
 * 
 */
public interface ReportService {

    /** 取一个job_id */
    String applyJobId(String biz);

    /** 开始任务 */
    void start(String job_id, String msg);

    /** 启动任务 */
    void running(String job_id, String msg);

    /** 任务完成 */
    void finish(String job_id, String msg);

    /** 任务中断 */
    void interrupt(String job_id, String msg);

    /** 开始并启动 */
    void startAndRunning(String job_id, String msg);

}
