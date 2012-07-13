package mop.hi.oce.domain.model;

import xce.util.tools.DateFormatter;

public class Expenditure {

	protected mop.hi.svc.model.Expenditure _expenditure;
	
	protected Expenditure(mop.hi.svc.model.Expenditure exp)
	{
		_expenditure = exp;
	}
	
	public Expenditure()
	{
		_expenditure = new mop.hi.svc.model.Expenditure();
	}
	
	public static Expenditure parse(mop.hi.svc.model.Expenditure exp)
	{
		return new Expenditure(exp);
	}
	
	public mop.hi.svc.model.Expenditure get()
	{
		return _expenditure;
	}
	
	public int getId()
	{
		return _expenditure.id;
	}
	
	public int getUserId()
	{
		return _expenditure.uid;
	}
	public void setUserId(int uid)
	{
		_expenditure.uid = uid;
	}
	
	public int getXNB()
	{
		return _expenditure.xnb;
	}
	public void setXNB(int xnb)
	{
		_expenditure.xnb = xnb;
	}
	
	public java.util.Date getTimestamp()
	{
		return DateFormatter.parse(_expenditure.timestamp);
	}
	public void setTimestamp(java.util.Date time)
	{
		_expenditure.timestamp = DateFormatter.format(time);
	}
	
	public String getDescription()
	{
		return _expenditure.description;
	}
	public void setDescription(String desc)
	{
		_expenditure.description = desc;
	}
	
	public String getProductName()
	{
		return _expenditure.productName;
	}
	public void setProductName(String pname)
	{
		_expenditure.productName = pname;	
	}
	
	public String getProductType()
	{
		return _expenditure.productType;
	}
	public void setProductType(String type)
	{
		_expenditure.productType = type;
	}
}
