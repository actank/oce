package common;

public class Blog {
	public Blog() {
		shareId = 0;
		blogId = 0;
		hostId = 0;
		time = "";
		count = 0;
		firstShareId = 0;
		weight = 0;
		title = "";
		content = "";
		weightStr = "";
	}
	
	public long shareId;	//分享id
	public long blogId;		//blog id
	public int hostId;		//该blog的拥有者id
	public String time;		//blog的被分享时间
	public int count;		//blog被分享的次数
	public int firstShareId;		//第一次分享的用户
	public float weight;		//属于当前type的权重，临时值，同一个blog对不同的weight值
	public String title;		//blog的title
	public String content;		//blog的内容
	public String weightStr;		//blog的weight序列
}
