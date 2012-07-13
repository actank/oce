package com.renren.search2.logger.service;

import org.apache.log4j.xml.DOMConfigurator;

import com.renren.search2.logger.impl.SearchLoggerI;

import xce.util.service.ServiceI;

public class SearchLoggerService extends ServiceI {

	@Override
	public void finishup() {
	}

	@Override
	public void initialize() {
		DOMConfigurator.configure("/data/xce/XiaoNeiSearch/lib64/log4j.xml");// 加载.xml文件
		_adapter.add(new SearchLoggerI(), Ice.Util.stringToIdentity("M"));
	}
}
