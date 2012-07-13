package com.xiaonei.xce.test.dbcacherequestconfig;
import java.util.Arrays;
import java.util.Collection;

import org.junit.After;
import org.junit.AfterClass;
import org.junit.Assert;
import org.junit.Before;
import org.junit.BeforeClass;
import org.junit.Test;
import org.junit.runner.RunWith;
import org.junit.runners.Parameterized;
import org.junit.runners.Parameterized.Parameters;

import xce.generaldbcache.DbFields;
import xce.generaldbcache.DbRows;
import xce.generaldbcache.GeneralDbCacheManagerPrx;

import com.xiaonei.xce.dbcache.DbCacheRequestConfigAdapter;


@RunWith(Parameterized.class)
public class DbCacheRequestConfigTestCase{
	private static DbCacheRequestConfigAdapter _adapter;
	private int _userid;
	private int _fieldSize;
	private String _field1;
	private String _field2;
	private int _rowSize;
	public DbCacheRequestConfigTestCase(int userid, int fieldSize, String field1, String field2, int rowSize){
		this._userid = userid;
		this._fieldSize = fieldSize;
		this._field1 = field1;
		this._field2 = field2;
		this._rowSize = rowSize;
	}
	@Parameters
	public static Collection<Object[]> setParameters(){
		return Arrays.asList(new Object[][]{
		      	{700000001,2,"owner","app_id",3},		//测试第一组
		});
	}
	@BeforeClass
	public static void setUpBeforeClass() throws Exception {
		_adapter = DbCacheRequestConfigAdapter.getInstance();
	}
	@AfterClass
	public static void tearDownAfterClass() throws Exception {  
	}
	@Before
	public void setUpBefore() throws Exception {
	}
	@After
	public void tearDownAfter() throws Exception {  
	}
	@Test
	public void testFields(){
		System.out.println(_userid + " " + _fieldSize + " " + _field1 + " " + _field2 + " " + _rowSize);
		GeneralDbCacheManagerPrx manager = _adapter.getGeneralDbCacheManager(_userid);
		DbFields fields = manager.getDbFields();
		DbRows rows = manager.getDbRows(_userid);
		
		Assert.assertEquals(_fieldSize,fields.fields.length);
		Assert.assertEquals(_field1,fields.fields[0]);
		Assert.assertEquals(_field2,fields.fields[1]);
		Assert.assertEquals(_rowSize,rows.rows.length);
	}
}
