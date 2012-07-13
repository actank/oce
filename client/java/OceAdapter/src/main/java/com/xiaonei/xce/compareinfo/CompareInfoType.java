package com.xiaonei.xce.compareinfo;
import xce.compareinfo.NETWORK;
import xce.compareinfo.HIGHTSCHOOL;
import xce.compareinfo.UNIVERSITY;
import xce.compareinfo.COLLEGE;
import xce.compareinfo.WORKPLACE;
import xce.compareinfo.ELEMENTARYSCHOOL;
import xce.compareinfo.JUNIORHIGHTSCHOOL;

public class CompareInfoType {
	   
	private  boolean _network ;
	private  boolean _highschool;
	private  boolean _university ;
	private  boolean _college ;
	private  boolean _workplace ;
	private  boolean _elmentayschool;
	private  boolean _juniorhighschool;

	private byte _typelist[];
	
	public  CompareInfoType()
	{}
	
	public byte[] getTypelist()
	{		
		int i =0 ;
		if(_network==true) ++i;
		if(_highschool==true)++i;
		if(_university==true) ++i;
		if(_college==true) ++i;
		if(_workplace==true) ++i;
		if(_elmentayschool==true) ++i;
		if(_juniorhighschool==true) ++i;
		
		_typelist = new byte[i];
		
		i=0;
		if(_network==true) _typelist[i]= NETWORK.value ;
		if(_highschool==true) _typelist[i]= HIGHTSCHOOL.value;
		if(_university==true) _typelist[i]= UNIVERSITY.value;
		if(_college==true) _typelist[i]= COLLEGE.value;
		if(_workplace==true) _typelist[i]= WORKPLACE.value;
		if(_elmentayschool==true) _typelist[i]= ELEMENTARYSCHOOL.value;
		if(_juniorhighschool==true) _typelist[i]= JUNIORHIGHTSCHOOL.value;
		
		return _typelist;
	}
	
	public void setNetWork(boolean flag) 
	{
		 _network=flag;
	}     
    public void setHighSchool(boolean flag) 
 	{
 		 _highschool=flag;
 	}     
    public void setUniversity(boolean flag) 
  	{
  		 _university=flag;
  	}
    public void setCollege(boolean flag) 
  	{
  		 _college=flag;
  	}
    public void setWorkPlace(boolean flag) 
  	{
  		 _workplace=flag;
  	}
    public void setElmentaySchool(boolean flag) 
  	{
  		 _elmentayschool=flag;
  	}
    public void setJuniorHighSchool(boolean flag) 
  	{          
  		 _juniorhighschool=flag;
  	}
	
		
    public boolean getNetWork() 
	{
		return _network;
	}     
    public boolean getHightSchool() 
 	{
 		return _highschool;
 	}     
    public boolean getUniversity() 
  	{
  		return _university;
  	}
    public boolean getCollege() 
  	{
  		return _college;
  	}
    public boolean getWorkPlace() 
  	{
  		return _workplace;
  	}
    public boolean getElmentaySchool() 
  	{
  		return _elmentayschool;
  	}
    public boolean getJuniorHighSchool() 
  	{
  		return _juniorhighschool;
  	}
     
}
