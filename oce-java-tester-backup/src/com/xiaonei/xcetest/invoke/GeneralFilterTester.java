package com.xiaonei.xcetest.invoke;


import java.util.HashMap;

import junit.framework.TestCase;
import mop.hi.oce.adapter.AdapterFactory;
import mop.hi.oce.domain.model.GeneralVisitInformation;
public class GeneralFilterTester extends TestCase {
	public void testFilterControl()
	{
		int typecount = 8;
		HashMap cmd=new HashMap();
		/*
		cmd.put(new String("OVER_COUNT_MESSAGE"),15);
		cmd.put(new String("MAX_MARK_MESSAGE"),8000);
		int type = GeneralVisitInformation.message;
		AdapterFactory.getGeneralFilterAdapter().filterControl(cmd, type);
		for(int i=0;i<40;i++){
			AdapterFactory.getGeneralFilterAdapter().passFilter(gvi);
		}
		*/
		/*
		cmd.put(new String("OVER_COUNT_FOOTPRINT"),15);
		cmd.put(new String("MAX_MARK_FOOTPRINT"),8000);
		int type = GeneralVisitInformation.footprint;	
		GeneralVisitInformation gvi= new GeneralVisitInformation();
		gvi.setType(type);
		gvi.setGuest(2);
		gvi.setHost(4);
		AdapterFactory.getGeneralFilterAdapter().filterControl(cmd, type);
		AdapterFactory.getGeneralFilterAdapter().passFilter(gvi);
		for(int i=0;i<40;i++){
			AdapterFactory.getGeneralFilterAdapter().passFilter(gvi);
		}
		*/
		/*
		cmd.put(new String("OVER_COUNT_LETTER"),15);
		cmd.put(new String("MAX_MARK_LETTER"),8000);
		int type = GeneralVisitInformation.letter;	
		GeneralVisitInformation gvi= new GeneralVisitInformation();
		gvi.setType(type);
		gvi.setGuest(2);
		gvi.setHost(4);
		AdapterFactory.getGeneralFilterAdapter().filterControl(cmd, type);
		AdapterFactory.getGeneralFilterAdapter().passFilter(gvi);
		for(int i=0;i<40;i++){
		AdapterFactory.getGeneralFilterAdapter().passFilter(gvi);
		}
	*/
/*		cmd.put(new String("OVER_COUNT_DIARY_AND_REPLY"),15);
		cmd.put(new String("MAX_MARK_DIARY_AND_REPLY"),8000);
		int type = GeneralVisitInformation.diaryAndreply;	
		GeneralVisitInformation gvi= new GeneralVisitInformation();
		gvi.setType(type);
		gvi.setGuest(2);
		gvi.setHost(4);
		AdapterFactory.getGeneralFilterAdapter().filterControl(cmd, type);
		AdapterFactory.getGeneralFilterAdapter().passFilter(gvi);
		for(int i=0;i<40;i++){
			AdapterFactory.getGeneralFilterAdapter().passFilter(gvi);
		}
	*/	
		/*
		cmd.put(new String("OVER_COUNT_INVITATION"),15);
		cmd.put(new String("MAX_MARK_INVITATION"),8000);
		int type = GeneralVisitInformation.invitation;	
		GeneralVisitInformation gvi= new GeneralVisitInformation();
		gvi.setType(type);
		gvi.setGuest(2);
		gvi.setHost(4);
		AdapterFactory.getGeneralFilterAdapter().filterControl(cmd, type);
		AdapterFactory.getGeneralFilterAdapter().passFilter(gvi);
		for(int i=0;i<40;i++){
			AdapterFactory.getGeneralFilterAdapter().passFilter(gvi);
		} 
		*/
		GeneralVisitInformation gvi= new GeneralVisitInformation();
		gvi.setGuest(2);
		gvi.setHost(4);
		for(int i=1;i<9;i++){
			gvi.setType(i);
			AdapterFactory.getGeneralFilterAdapter().passFilter(gvi);
		}

	}
}
