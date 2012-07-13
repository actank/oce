package com.xiaonei.xce.feed;

import java.io.StringReader;
import java.util.HashMap;
import java.util.Map;

import javax.xml.parsers.DocumentBuilderFactory;

import org.xml.sax.InputSource;

import com.xiaonei.xce.log.Oce;

import xce.feed.FeedSeed;

public class Feed {

	private static final int ACTOR_FLAG = 1;
	private static final int EXTRA_PROPS_FLAG = 2;
	private static final int PARENT_FLAG = 4;
	private static final int SOURCE_FLAG = 8;
	private static final int STYPE_FLAG = 16;
	private static final int MINI_MERGE_TYPE_FLAG = 32;
	private static final int NEWS_MERGE_TYPE_FLAG = 64;
	private static final int REPLY_TYPE_FLAG = 128;
	private static final int XML_FLAG = 256;
	private static final int PTYPE_FLAG = 512;
	private static final int MINI_MERGE_FLAG = 1024;
	private static final int NEWS_MERGE_FLAG = 2048;
	
	private static final int ALL_FLAG = ACTOR_FLAG | EXTRA_PROPS_FLAG
			| PARENT_FLAG | SOURCE_FLAG | STYPE_FLAG | MINI_MERGE_TYPE_FLAG
			| NEWS_MERGE_TYPE_FLAG | REPLY_TYPE_FLAG | XML_FLAG | PTYPE_FLAG | MINI_MERGE_FLAG | NEWS_MERGE_FLAG;

	private FeedSeed _seed;
	private int _flag;

	public Feed() {
		_seed = new FeedSeed();
		_seed.actor = 0;
		_seed.extraProps = new HashMap<String, String>();
		_seed.feed = 0;
		_seed.psource = 0;
		_seed.source = 0;
		_seed.time = (int) (System.currentTimeMillis()/1000);
		_seed.type = 0;
		_seed.ptype = 0;
		_seed.miniMerge = 0;
		_seed.newsMerge = 0;
		_flag = 0;
	}

	public Feed(FeedSeed seed) {
		_seed = seed;
		_flag = ALL_FLAG;
	}

	public void setActor(int actor) {
		_seed.actor = actor;
		_flag |= ACTOR_FLAG;
	}

	public void setTimeAsSecond(int second){
		_seed.time = second;
	}
	
	
	public void setParent(long parent) {
		_seed.psource = parent;
		_flag |= PARENT_FLAG;
	}

	public void setSource(long source) {
		_seed.source = source;
		_flag |= SOURCE_FLAG;
	}

	public void setStype(int stype) {
		_seed.type = (_seed.type & 0xFF000000) | (stype & 0x00FFFFFF);
		_flag |= STYPE_FLAG;
	}

	public void setNewsMergeType(int newsMerge) {
		_seed.type |= (newsMerge & 3) << 30;
		_flag |= NEWS_MERGE_TYPE_FLAG;
	}

	public void setMiniMergeType(int miniMerge) {
		_seed.type |= (miniMerge & 3) << 28;
		_flag |= MINI_MERGE_TYPE_FLAG;
	}

	public void setReplyType(int reply) {
		_seed.type |= (reply & 3) << 26;
		_flag |= REPLY_TYPE_FLAG;
	}

	public void setPtype(int ptype) {
		_seed.ptype = ptype;
		_flag |= PTYPE_FLAG;
	}
	
	public void setMiniMergeId(long merge) {
		_seed.miniMerge = merge;
		_flag |= MINI_MERGE_FLAG;
	}
	
	public void setNewsMergeId(long merge) {
		_seed.newsMerge = merge;
		_flag |= NEWS_MERGE_FLAG;
	}
	
	public void setExtraProps(Map<String, String> props) {
		_seed.extraProps = props;
		_flag |= EXTRA_PROPS_FLAG;
	}

	public void setXml(String xml) throws Exception{
		StringReader sr = new StringReader(xml);
		InputSource is = new InputSource(sr);

		DocumentBuilderFactory factory = DocumentBuilderFactory.newInstance();
		factory.newDocumentBuilder().parse(is);
		_seed.xml = xml;
		_flag |= XML_FLAG;
	}

	public FeedSeed getFeedSeed() {
		//应康伟要求，暂注掉此log
		//System.out.println(_flag);
		if ((_flag & ALL_FLAG) == ALL_FLAG) {
			return _seed;
		}
		Oce.getLogger().info("feedseed fill wrong");
		return null;
	}
	
	public String toStr(){
		StringBuffer buf = new StringBuffer();
		/*
		 * _seed = new FeedSeed();
		_seed.actor = 0;
		_seed.extraProps = new HashMap<String, String>();
		_seed.feed = 0;
		_seed.psource = 0;
		_seed.source = 0;
		_seed.time = 0;
		_seed.type = 0;
		_seed.ptype = 0;
		_seed.miniMerge = 0;
		_seed.newsMerge = 0;
		 */
		
		buf.append("actor:").append(_seed.actor).append(",psource:").append(_seed.psource).append(",source:").append(_seed.source);
		buf.append(",time:").append(_seed.time).append(",type:").append(_seed.ptype).append(",ptype:").append(_seed.ptype);
		buf.append(",miniMerge:").append(_seed.miniMerge).append(",newsMerge:").append(_seed.newsMerge).append(",xml:").append(_seed.xml);
		buf.append(",propMap:").append(_seed.extraProps.toString()).append(",full_type:").append(_seed.type);
		return buf.toString();
		
	}
}
