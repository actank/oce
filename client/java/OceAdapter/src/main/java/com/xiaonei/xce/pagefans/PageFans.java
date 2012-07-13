package com.xiaonei.xce.pagefans;

import java.util.Date;

import xce.buddy.PageEntry;

public class PageFans {

	private    int id;
	private   int pageId;
	private   int userId;
	private   int state;
	private   Date addTime;
	private   int operatorId;


  public
	int getId()
	{
		return id;
	}
  public int getPageId()
    {
    	return pageId;
    }
  public  int getUserId () 
    {
    	return userId;
    }
  public  int getState()
    {
    	return state;
    }
  public  Date getAddTime()
    {
    	return addTime;
    }
  public   int getOperatorId()
    {
    	 return  operatorId;
    }
    
    
  public  void setId(int data)
	{
		 id = data;
	}
  public  void setPageId(int data)
    {
    	 pageId= data;
    }
  public void setUserId (int data) 
    {
    	 userId= data;
    }
  public void setState(int data)
    {
    	 state= data;
    }
  public  void setAddTime(Date data)
    {
    	 addTime= data;
    }
  public void setOperatorId(int data)
    {
    	   operatorId= data;
    }
    
  public PageFans()
    {	
    }
    
  public  void Parse(PageEntry data)
    {	
    	 id = data.id;
         pageId = data.pageId;
         userId = data.userId;
         state = data.state;
         addTime = new Date((long)(data.addTime)*1000L); 
         operatorId = data.operatorId;
    }
    
}
