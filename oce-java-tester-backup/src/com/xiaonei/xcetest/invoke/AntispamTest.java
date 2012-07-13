package com.xiaonei.xcetest.invoke;

import java.util.HashMap;
import java.util.Map;

import junit.framework.TestCase;
import mop.hi.oce.adapter.impl.Antispam2AdapterImpl;
import mop.hi.oce.adapter.impl.AntispamAdapterImpl;

public class AntispamTest extends TestCase {

	public AntispamAdapterImpl adapter = new AntispamAdapterImpl();

	public void test1() throws InterruptedException {

		// System.out.println(adapter.check(20007, t0));
		// Thread.sleep(1000);
		// System.out.println(adapter.check(20007, t1));
		// Thread.sleep(1000);
		// System.out.println(adapter.check(20007, t2));
		// Thread.sleep(1000);
		// System.out.println(adapter.check(20007, t3));
		// Thread.sleep(1000);
		// System.out.println(adapter.check(20007, t4));
		// Thread.sleep(1000);
		// System.out.println(adapter.check(20007, t5));
	}

	public void test2() {
		String a = "万学海文专业课教育学、心理学、历史学全面开课，北京大学、中国人民大学、北京师范大学名师纷纷隆重登场。上课时间：八月二十五日至八月三十一日名师讲解敬请关注，开课在即，报名从速! 报名即送2008教育学、心理学、历史学大纲解析（权威教授独家解析，不对外发售）。你还想了解更多吗？请TEL：13141396785 或登陆www.vipkaoyan.com 更多内容等着您！！！ 万学海文统考专业课名师：教育学刘教授：北京师范大学教育学院教授，中国考研教育学原理辅导第一人。刘教授：北京师范大学心理学院教授，教育心理学的权威版本《当代教育心理学》的编著者。王教授：北京师范大学教育学院副教授，中国著名的中外教育史青年专家。鲍教授：北京师范大学教育管理学院老师，中国高等教育界的教坛新秀。心理学刘教授：北京师范大学心理学院教授，中国学习与教学心理学方面的最高权威。沃教授：";
		adapter.addNewSpam(a);
	}

	public void test3() {
		adapter.stopCheck(0, 0);
	}

	public void test4() {
		adapter.changeRepeatslot(40);
		adapter.changeTimeslot(4);
	}
	
	public void test5()
	{
		Antispam2AdapterImpl adapter = new Antispam2AdapterImpl();
		//adapter.check(1, 0, 9, "我");

		adapter.check(1, 0, 9, "adfd&#1法d轮d功法d轮法d轮d功d功法d轮d功法法d轮d功");
	}
	public void test6()
	{
		String [] a ={"163ggmm.cn"
				,"ip3366.cn"
				,"xmphp.com"
				,"0033.cn"
				,"lqiang.cn"
				,"73366.cn"
				,"336.cc"};
 ;
		Antispam2AdapterImpl adapter = new Antispam2AdapterImpl();
		for(int i = 0 ;i< a.length; ++i){
			System.out.println(a[i]);
			adapter.addNewDictWord(a[i], adapter.ACTIVE_TYPE_BLOCK);
		}
	}
	public void test7()
	{
		Antispam2AdapterImpl adapter = new Antispam2AdapterImpl();
		Map<String,Integer> props = new HashMap<String, Integer>();
		props.put(adapter.FAST_OVER_COUNT, 6);
		props.put(adapter.SAME_OVER_COUNT, 8);
		props.put(adapter.SAME_TIMESLOT,10);
		props.put(adapter.FAST_OVER_TIMESLOT, 300);
		props.put(adapter.SAME_OVER_TIMESLOT, 400);
		props.put(adapter.FORBIDDEN_BY_BLOCK, 60*60);
		props.put(adapter.FORBIDDEN_BY_SAME_OVER, 60*60*2);
		props.put(adapter.FORBIDDEN_BY_FAST_OVER, 60*60*2);
		props.put(adapter.MAYBE_OVER_COUNT, 1);
		adapter.control(props);
	}
	public void test8()
	{

		Antispam2AdapterImpl adapter = new Antispam2AdapterImpl();
		String text = "<img <djfdfjj><p １１２２w w1ＷＥＲ/>１１２２w w1ＷＥＲ<A <href='sdfdfd' &#ddf/>&fdfd&#122;#122#dfdf;#dfd</a> <ddf<BR>dfkdjfjj>,dkdk<p>dfjdj</p>jjjj";
		System.out.print(adapter.htmlFilter(text));
		
	}

}
