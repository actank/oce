package com.xiaonei.xce.test.userconfig;

import java.sql.Connection;
import java.sql.ResultSet;
import java.sql.Statement;
import java.util.Arrays;
import java.util.Collection;

import junit.framework.Assert;

import org.junit.After;
import org.junit.AfterClass;
import org.junit.Before;
import org.junit.BeforeClass;
import org.junit.Test;
import org.junit.runner.RunWith;
import org.junit.runners.Parameterized;
import org.junit.runners.Parameterized.Parameters;

import com.xiaonei.xce.XceAdapter;

import mop.hi.oce.adapter.impl.UserConfigInfoAdapterImpl;
import mop.hi.oce.adapter.impl.UserConfigInfoReplicaAdapterImpl;
import mop.hi.oce.domain.user.UserConfigInfo;
import mop.hi.oce.domain.user.UserConfigInfoFactory;
import mop.hi.svc.model.UserException;

@RunWith(Parameterized.class)
public class UserConfigTestCase {
	private int _id;
          
	private int _email_configO;
	private int _basic_configO;
	private int _poke_configO;
	private int _request_friend_configO;
	private int _photo_configO;
	private int _message_configO;
	private int _browse_configO;
	private int _want_see_cssO;
	private int _status_configO;	
	private String _display_menu_listO;
	private String _more_menu_listO;
	private String _send_feed_configO;
	private String _recv_feed_configO;
	private String _profile_privacyO;
	
	private int _email_configN;
	private int _basic_configN;
	private int _poke_configN;
	private int _request_friend_configN;
	private int _photo_configN;
	private int _message_configN;
	private int _browse_configN;
	private int _want_see_cssN;
	private int _status_configN;	
	private String _display_menu_listN;
	private String _more_menu_listN;
	private String _send_feed_configN;
	private String _recv_feed_configN;
	private String _profile_privacyN;
	
	@Parameters
	public static Collection<Object[]> setParameters() {
		return Arrays.asList(new Object[][] {
					{6813,1,1,1,1,1,1,1,1,1,"display_menu_listO","more_menu_listO","send_feed_configO","recv_feed_configO","profile_privacyO",
						2,2,2,2,2,2,2,2,2,"display_menu_listN","more_menu_listN","send_feed_configN","recv_feed_configN","profile_privacyN"},
				});
	}
	
	public UserConfigTestCase(int id, int email_configO, int basic_configO, int poke_configO, int request_friend_configO,
			int photo_configO, int message_configO, int browse_configO, int want_see_cssO,
			int status_configO, String display_menu_listO, String more_menu_listO, 
			String send_feed_configO,String recv_feed_configO, String profile_privacyO,
			int email_configN, int basic_configN, int poke_configN, int request_friend_configN,
			int photo_configN, int message_configN, int browse_configN, int want_see_cssN,
			int status_configN, String display_menu_listN, String more_menu_listN, 
			String send_feed_configN,String recv_feed_configN, String profile_privacyN
			){
		_id=id;
		
		_email_configO=email_configO;
		_basic_configO=basic_configO;
		_poke_configO=poke_configO;
		_request_friend_configO=request_friend_configO;
		_photo_configO=photo_configO;
		_message_configO=message_configO;
		_browse_configO=browse_configO;
		_want_see_cssO=want_see_cssO;
		_status_configO=status_configO;
		_display_menu_listO=display_menu_listO;
		_more_menu_listO=more_menu_listO;
		_send_feed_configO=send_feed_configO;
		_recv_feed_configO=recv_feed_configO;
		_profile_privacyO=profile_privacyO;
		
		_email_configN=email_configN;
		_basic_configN=basic_configN;
		_poke_configN=poke_configN;
		_request_friend_configN=request_friend_configN;
		_photo_configN=photo_configN;
		_message_configN=message_configN;
		_browse_configN=browse_configN;
		_want_see_cssN=want_see_cssN;
		_status_configN=status_configN;
		_display_menu_listN=display_menu_listN;
		_more_menu_listN=more_menu_listN;
		_send_feed_configN=send_feed_configN;
		_recv_feed_configN=recv_feed_configN;
		_profile_privacyN=profile_privacyN;
	}
	

	@BeforeClass
	public static void setUpBeforeClass() throws Exception {
		
	}
	@AfterClass
	public static void tearDownAfterClass() throws Exception {
		
	}
	
	@Before
	public void setUpBefore() throws Exception {
		try{
			Connection c=XceAdapter.getInstance().getWriteConnection("user_config");
			try{
				Statement s=c.createStatement();
				try{
					String sql="delete from user_config where id="+_id;
					s.execute(sql);
				}catch(Exception e){
					e.printStackTrace();
					Assert.assertFalse(true);
				}finally{
					s.close();
				}
			}catch(Exception e){
				e.printStackTrace();
				Assert.assertFalse(true);
			}finally{
				c.close();
			}			
		}catch(Exception e){
			e.printStackTrace();
			Assert.assertFalse(true);
		}
	}
	@After
	public void tearDownAfter() throws Exception {  

	}
	
	@Test
	public void test() {
		UserConfigInfoAdapterImpl uciai=new UserConfigInfoAdapterImpl();
		UserConfigInfo uciw1=new UserConfigInfo();
		uciw1.setId(_id);
		uciw1.setBasicConfig(_basic_configO);
		uciw1.setBrowseConfig(_browse_configO);
		uciw1.setDisplayMenuList(_display_menu_listO);
		uciw1.setEmailConfig(_email_configO);
		uciw1.setMessageConfig(_message_configO);
		uciw1.setMoreMenuList(_more_menu_listO);
		uciw1.setPhotoConfig(_photo_configO);
		uciw1.setPokeConfig(_poke_configO);
		uciw1.setProfilePrivacy(_profile_privacyO);
		uciw1.setRecvFeedConfig(_recv_feed_configO);
		uciw1.setSendFeedConfig(_send_feed_configO);
		uciw1.setStatusConfig(_status_configO);
		uciw1.setWantSeeCss(_want_see_cssO);
		uciw1.setRequestFriendConfig(_request_friend_configO);
		uciai.createUserConfig(_id, uciw1);
		try{
			Connection c=XceAdapter.getInstance().getReadConnection("user_config");
			try{
				Statement s=c.createStatement();
				try{
					String sql="SELECT * FROM user_config WHERE id="+_id;
					ResultSet rs=s.executeQuery(sql);
					while(rs.next()){
						Assert.assertEquals(rs.getString("send_feed_config"), _send_feed_configO);
						Assert.assertEquals(rs.getString("recv_feed_config"), _recv_feed_configO);
						Assert.assertEquals(rs.getString("profile_privacy"), _profile_privacyO);
						Assert.assertEquals(rs.getString("display_menu_list"), _display_menu_listO);
						Assert.assertEquals(rs.getString("more_menu_list"), _more_menu_listO);
						
						Assert.assertEquals(rs.getInt("email_config"),_email_configO);
						Assert.assertEquals(rs.getInt("basic_config"),_basic_configO);
						Assert.assertEquals(rs.getInt("poke_config"),_poke_configO);
						Assert.assertEquals(rs.getInt("request_friend_config"),_request_friend_configO);
						Assert.assertEquals(rs.getInt("photo_config"),_photo_configO);
						Assert.assertEquals(rs.getInt("message_config"),_message_configO);
						Assert.assertEquals(rs.getInt("browse_config"),_browse_configO);
						Assert.assertEquals(rs.getInt("want_see_css"),_want_see_cssO);
						Assert.assertEquals(rs.getInt("status_config"),_status_configO);
					}
					rs.close();
				}catch(Exception e){
					e.printStackTrace();
					Assert.assertFalse(true);
				}finally{
					s.close();
				}
			}catch(Exception e){
				e.printStackTrace();
				Assert.assertFalse(true);
			}finally{
				c.close();
			}
		}catch(Exception e){
			e.printStackTrace();
			Assert.assertFalse(true);
		}
		try{
			UserConfigInfo ucir1=uciai.getUserConfig(_id);
			Assert.assertEquals(_id,ucir1.getId());
			Assert.assertEquals(ucir1.getBasicConfig(),_basic_configO);
			Assert.assertEquals(ucir1.getBrowseConfig(),_browse_configO);
			Assert.assertEquals(ucir1.getDisplayMenuList(),_display_menu_listO);
			Assert.assertEquals(ucir1.getEmailConfig(),_email_configO);
			Assert.assertEquals(ucir1.getMessageConfig(),_message_configO);
			Assert.assertEquals(ucir1.getMoreMenuList(),_more_menu_listO);
			Assert.assertEquals(ucir1.getPhotoConfig(),_photo_configO);
			Assert.assertEquals(ucir1.getPokeConfig(),_poke_configO);
			Assert.assertEquals(ucir1.getProfilePrivacy(),_profile_privacyO);
			Assert.assertEquals(ucir1.getRecvFeedConfig(),_recv_feed_configO);
			Assert.assertEquals(ucir1.getRequestFriendConfig(),_request_friend_configO);
			Assert.assertEquals(ucir1.getSendFeedConfig(),_send_feed_configO);
			Assert.assertEquals(ucir1.getStatusConfig(),_status_configO);
			Assert.assertEquals(ucir1.getWantSeeCss(),_want_see_cssO);
		}catch(Exception e){
			e.printStackTrace();
			Assert.assertFalse(true);
		}
		UserConfigInfo uciw2=new UserConfigInfo();
		uciw2.setBasicConfig(_basic_configN);
		uciw2.setBrowseConfig(_browse_configN);
		uciw2.setDisplayMenuList(_display_menu_listN);
		uciw2.setEmailConfig(_email_configN);
		uciw2.setMessageConfig(_message_configN);
		uciw2.setMoreMenuList(_more_menu_listN);
		uciw2.setPhotoConfig(_photo_configN);
		uciw2.setPokeConfig(_poke_configN);
		uciw2.setProfilePrivacy(_profile_privacyN);
		uciw2.setRecvFeedConfig(_recv_feed_configN);
		uciw2.setSendFeedConfig(_send_feed_configN);
		uciw2.setStatusConfig(_status_configN);
		uciw2.setWantSeeCss(_want_see_cssN);
		uciw2.setRequestFriendConfig(_request_friend_configN);
		uciai.setUserConfig(_id, uciw2);
		try{
			Connection c=XceAdapter.getInstance().getReadConnection("user_config");
			try{
				Statement s=c.createStatement();
				try{
					String sql="SELECT * FROM user_config WHERE id="+_id;
					ResultSet rs=s.executeQuery(sql);
					while(rs.next()){
						Assert.assertEquals(rs.getString("send_feed_config"), _send_feed_configN);
						Assert.assertEquals(rs.getString("recv_feed_config"), _recv_feed_configN);
						Assert.assertEquals(rs.getString("profile_privacy"), _profile_privacyN);
						Assert.assertEquals(rs.getString("display_menu_list"), _display_menu_listN);
						Assert.assertEquals(rs.getString("more_menu_list"), _more_menu_listN);
						
						Assert.assertEquals(rs.getInt("email_config"),_email_configN);
						Assert.assertEquals(rs.getInt("basic_config"),_basic_configN);
						Assert.assertEquals(rs.getInt("poke_config"),_poke_configN);
						Assert.assertEquals(rs.getInt("request_friend_config"),_request_friend_configN);
						Assert.assertEquals(rs.getInt("photo_config"),_photo_configN);
						Assert.assertEquals(rs.getInt("message_config"),_message_configN);
						Assert.assertEquals(rs.getInt("browse_config"),_browse_configN);
						Assert.assertEquals(rs.getInt("want_see_css"),_want_see_cssN);
						Assert.assertEquals(rs.getInt("status_config"),_status_configN);
					}
					rs.close();
				}catch(Exception e){
					e.printStackTrace();
					Assert.assertFalse(true);
				}finally{
					s.close();
				}
			}catch(Exception e){
				e.printStackTrace();
				Assert.assertFalse(true);
			}finally{
				c.close();
			}
		}catch(Exception e){
			e.printStackTrace();
			Assert.assertFalse(true);
		}
		try{
			UserConfigInfo ucir2=uciai.getUserConfig(_id);
			Assert.assertEquals(_id,ucir2.getId());
			Assert.assertEquals(ucir2.getBasicConfig(),_basic_configN);
			Assert.assertEquals(ucir2.getBrowseConfig(),_browse_configN);
			Assert.assertEquals(ucir2.getDisplayMenuList(),_display_menu_listN);
			Assert.assertEquals(ucir2.getEmailConfig(),_email_configN);
			Assert.assertEquals(ucir2.getMessageConfig(),_message_configN);
			Assert.assertEquals(ucir2.getMoreMenuList(),_more_menu_listN);
			Assert.assertEquals(ucir2.getPhotoConfig(),_photo_configN);
			Assert.assertEquals(ucir2.getPokeConfig(),_poke_configN);
			Assert.assertEquals(ucir2.getProfilePrivacy(),_profile_privacyN);
			Assert.assertEquals(ucir2.getRecvFeedConfig(),_recv_feed_configN);
			Assert.assertEquals(ucir2.getRequestFriendConfig(),_request_friend_configN);
			Assert.assertEquals(ucir2.getSendFeedConfig(),_send_feed_configN);
			Assert.assertEquals(ucir2.getStatusConfig(),_status_configN);
			Assert.assertEquals(ucir2.getWantSeeCss(),_want_see_cssN);
		}catch(Exception e){
			e.printStackTrace();
			Assert.assertFalse(true);
		}
	}

}

