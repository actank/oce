package com.xiaonei.xce.pagefans;

import java.util.Date;

import xce.buddy.PageEntry;

public class BlacklistItem {

	
	private	int id;
	private  int pageId;
	private int userId;
   
	private Date timestamp;
	private int executor;


	public int getId()
	{
		return id;
	}
	public int getPageId()
    {
    	return pageId;
    }
	public int getUserId () 
    {
    	return userId;
    }
  
	public Date getTimestamp()
    {
    	 return timestamp;
    }
	public int getExecutor()
    {
    	 return  executor;
    }
    
    
	public void setId(int data)
	{
		 id = data;
	}
	public void setPageId(int data)
    {
    	 pageId= data;
    }
	public void setUserId (int data) 
    {
    	 userId= data;
    }
  
	public void setTimestamp(Date data)
    {
    	timestamp = data;
    }
	public void setExecutor(int data)
    {
    	executor= data;
    }
    
	public BlacklistItem()
    {	
    }
    
	public void Parse(PageEntry data)
    {	
    	 id = data.id;
         pageId = data.pageId;
         userId = data.userId;
         timestamp = new Date((long)(data.addTime)*1000L); ;
         executor = data.operatorId;
     
    }
	
}
