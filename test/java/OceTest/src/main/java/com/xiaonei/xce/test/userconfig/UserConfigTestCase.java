/**
 * 测试目的：测试UserConfig热备服务中oldwr/reader/writeragent的功能
 * 测试环境：在服务端oldwr/reader/writeragent服务各启动一份；
 * 通过标准：reader/oldwr/DB均和预期数据一致；
 * 
 * @author 梁小锋(xiaofeng.liang@renren-inc.com)
 *
 */
package com.xiaonei.xce.test.userconfig;

import mop.hi.oce.adapter.impl.UserConfigInfoAdapterImpl;
import mop.hi.oce.adapter.impl.UserConfigInfoReaderAdapterImpl;
import mop.hi.oce.adapter.impl.UserConfigInfoWriterAdapterImpl;
import mop.hi.oce.domain.user.UserConfigInfo;
import mop.hi.svc.model.UserException;

import com.xiaonei.xce.XceAdapter;

import org.junit.After;
import org.junit.Before;
import org.junit.Test;
import org.junit.runner.RunWith;
import org.junit.runners.Parameterized;
import org.junit.runners.Parameterized.Parameters;
import junit.framework.Assert;

import java.sql.Connection;
import java.sql.ResultSet;
import java.sql.Statement;

import java.util.Arrays;
import java.util.Collection;

@RunWith(Parameterized.class)
public class UserConfigTestCase {
	private static int testCount = 0;
	private final UserConfigInfo _info;
	private UserConfigInfoAdapterImpl _adapter;
	private UserConfigInfoReaderAdapterImpl _adapterOfReader;
	private UserConfigInfoWriterAdapterImpl _adapterOfWriter;
	
	

	public UserConfigTestCase(int id, long email_config, int basic_config, int poke_config, int request_friend_config,
			int photo_config, int message_config, int browse_config, int want_see_css,
			int status_config, String display_menu_list, String more_menu_list, 
			String send_feed_config, String recv_feed_config, String profile_privacy
			) {
		_info = new UserConfigInfo();
		_info.setId(id);
		_info.setBasicConfig(basic_config);
		_info.setBrowseConfig(browse_config);
		_info.setDisplayMenuList(display_menu_list);
		_info.setEmailConfig(email_config);
		_info.setMessageConfig(message_config);
		_info.setMoreMenuList(more_menu_list);
		_info.setPhotoConfig(photo_config);
		_info.setPokeConfig(poke_config);
		_info.setProfilePrivacy(profile_privacy);
		_info.setRecvFeedConfig(recv_feed_config);
		_info.setSendFeedConfig(send_feed_config);
		_info.setStatusConfig(status_config);
		_info.setWantSeeCss(want_see_css);
		_info.setRequestFriendConfig(request_friend_config);
	}

	/**
	 * 获取一份新的数据
	 * 
	 * @param idStep
	 * @return
	 */
	protected UserConfigInfo genInfoCopy(int idStep) {
		UserConfigInfo result = new UserConfigInfo();
		result.setId(_info.getId());
		result.setBasicConfig(_info.getBasicConfig() + idStep);
		result.setBrowseConfig(_info.getBrowseConfig() + idStep);
		result.setDisplayMenuList(_info.getDisplayMenuList() + idStep);
		result.setEmailConfig(_info.getEmailConfig() + idStep);
		result.setMessageConfig(_info.getMessageConfig() + idStep);
		result.setMoreMenuList(_info.getMoreMenuList() + idStep);
		result.setPhotoConfig(_info.getPhotoConfig() + idStep);
		result.setPokeConfig(_info.getPokeConfig() + idStep);
		result.setProfilePrivacy(_info.getProfilePrivacy() + idStep);
		result.setRecvFeedConfig(_info.getRecvFeedConfig() + idStep);
		result.setSendFeedConfig(_info.getSendFeedConfig() + idStep);
		result.setStatusConfig(_info.getStatusConfig() + idStep);
		result.setWantSeeCss(_info.getWantSeeCss() + idStep);
		result.setRequestFriendConfig(_info.getRequestFriendConfig() + idStep);

		return result;
	}

	@Parameters
	public static Collection<Object[]> setParameters() {
		return Arrays.asList(new Object[][] {
			//参数意义：前10个是int/long，后5个为String，前10个含义如下
			//在数据库中want_see_css是tinyint(4)
			//id, email_config, basic_config, poke_config, request_friend_configO,
			//photo_config, message_config, browse_config, want_see_css, status_configO
			{
				800004444,1,1,1,1,
				1,1,1,1,1,
				"display_menu_list",
				"more_menu_list",
				"send_feed_config",
				"recv_feed_config",
				"profile_privacy" 
			}, 
			{
				800004444,123456789012345L,0,0,0,
				0,0,0,0,0,
				"display_menu_list",
				"more_menu_list",
				"send_feed_config",
				"recv_feed_config",
				"profile_privacy" 
			}, 
//			//并没有对want_see_css的溢出做判断或处理，
//			//建议把UserConfigInfo::setWantSeeCss(int)改为UserConfigInfo::setWantSeeCss(byte)
//			{
//				800004444,1,1,1,1,
//				1,1,1,127,1,
//				"display_menu_list",
//				"more_menu_list",
//				"send_feed_config",
//				"recv_feed_config",
//				"profile_privacy" 
//			}			
		});
	}

	@Before
	public void setUpBefore() throws Exception {
		System.out.println(++testCount + " : setUpBefore() ");
		System.out.println("================================");
		_adapter = new UserConfigInfoAdapterImpl();
		_adapterOfReader = new UserConfigInfoReaderAdapterImpl();
		_adapterOfWriter = new UserConfigInfoWriterAdapterImpl();
	}

	@After
	public void tearDownAfter() throws Exception {

	}

	/**
	 * reload和get是最关键的方法
	 * 测试中对缓存的控制依赖于reload方法
	 * 测试中对缓存的判断依赖于get方法
	 */
	@Test
	public void testGetUserConfig() {
		init();
		checkAll(_info);
	}
	
	@Test
	public void testReloadUserConfig() {
		init();
		UserConfigInfo info = genInfoCopy(1);
		setDB(info.getId(), info);
		_adapter.reloadUserConfig(info.getId());
		checkAll(info);
		
		init();
		UserConfigInfo infoForWriter = genInfoCopy(1);
		setDB(infoForWriter.getId(), infoForWriter);
		_adapterOfWriter.reloadUserConfig(infoForWriter.getId());
		checkAll(infoForWriter);
	}
	
	@Test
	public void testSetUserConfig() {
		init();
		UserConfigInfo info = genInfoCopy(1);
		_adapter.setUserConfig(info.getId(), info);
		checkAll(info);
		
		init();
		UserConfigInfo infoForWriter = genInfoCopy(1);
		_adapterOfWriter.setUserConfig(infoForWriter.getId(), infoForWriter);
		checkAll(infoForWriter);		
	}

	/**
	 * 用info来和DB/Cache/ReaderCache中的内容对比，判断是否一致
	 * @param info
	 */
	protected void checkAll(UserConfigInfo info) {
		Assert.assertTrue(isDBCorrect(info));
		Assert.assertTrue(isCacheCorrect(info));
		Assert.assertTrue(isReaderCacheCorrect(info));
	}

	protected boolean isDBCorrect(UserConfigInfo info) {
		UserConfigInfo infosOfDB = readDBByUserId(info.getId());
		System.out.println("\ninfosOfDB:");
		System.out.println("--------------------------------");
		System.out.println(infosOfDB);
		return isEqual(infosOfDB, info);
	}

	protected boolean isCacheCorrect(UserConfigInfo info) {
		UserConfigInfo infosOfCache = null;
		try {
			infosOfCache = _adapter.getUserConfig(info.getId());
		} catch (UserException e) {
			e.printStackTrace();
		} catch (Exception e) {
			e.printStackTrace();
		}
		System.out.println("\ninfosOfCache:");
		System.out.println("--------------------------------");
		System.out.println(infosOfCache);
		return isEqual(infosOfCache, info);
	}

	protected boolean isReaderCacheCorrect(UserConfigInfo info) {
		UserConfigInfo infosOfReaderCache = null;
		try {
			infosOfReaderCache = _adapterOfReader.getUserConfig(info.getId());
		} catch (UserException e) {
			e.printStackTrace();
		} catch (Exception e) {
			e.printStackTrace();
		}
		System.out.println("\ninfosOfReaderCache:");
		System.out.println("--------------------------------");
		System.out.println(infosOfReaderCache);
		return isEqual(infosOfReaderCache, info);
	}
	
	protected void setDB(int userId, UserConfigInfo info) {
		String sql = "replace into user_config("
			+ "id,"
			+ "email_config,"
			+ "basic_config,"
			+ "poke_config,"
			+ "request_friend_config,"
			+ "photo_config,"
			+ "message_config,"
			+ "browse_config,"
			+ "display_menu_list,"
			+ "more_menu_list,"
			+ "want_see_css,"
			+ "send_feed_config,"
			+ "recv_feed_config,"
			+ "profile_privacy,"
			+ "status_config"
			+ ") values ("
			+ userId + ","
			+ info.getEmailConfig() + ","
			+ info.getBasicConfig() + ","
			+ info.getPokeConfig() + ","
			+ info.getRequestFriendConfig() + ","
			+ info.getPhotoConfig() + ","
			+ info.getMessageConfig() + ","
			+ info.getBrowseConfig() + ",'"
			+ info.getDisplayMenuList() + "','"
			+ info.getMoreMenuList() + "',"
			+ info.getWantSeeCss() + ",'"
			+ info.getSendFeedConfig() + "','"
			+ info.getRecvFeedConfig() + "','"
			+ info.getProfilePrivacy() + "',"
			+ info.getStatusConfig() + ")";
		writeDB(sql);
	}

	protected void cleanDBByUserId(int userId) {
		String sql = "delete from user_config where id=" + userId;
		writeDB(sql);
	}

	protected UserConfigInfo readDBByUserId(int userId) {
		String sql = "select * from user_config where id=" + userId;
		return readDB(sql);
	}

	protected UserConfigInfo readDB(String sql) {
		UserConfigInfo result = new UserConfigInfo();
		try {
			Connection c = XceAdapter.getInstance().getReadConnection("user_config");
			try {
				Statement s = c.createStatement();
				try {
					ResultSet rs = s.executeQuery(sql);
					while (rs.next()) {
						result.setId(rs.getInt("id"));
						result.setBasicConfig(rs.getInt("basic_config"));
						result.setBrowseConfig(rs.getInt("browse_config"));
						result.setDisplayMenuList(rs.getString("display_menu_list"));
						result.setEmailConfig(rs.getLong("email_config"));
						result.setMessageConfig(rs.getInt("message_config"));
						result.setMoreMenuList(rs.getString("more_menu_list"));
						result.setPhotoConfig(rs.getInt("photo_config"));
						result.setPokeConfig(rs.getInt("poke_config"));
						result.setProfilePrivacy(rs.getString("profile_privacy"));
						result.setRecvFeedConfig(rs.getString("recv_feed_config"));
						result.setSendFeedConfig(rs.getString("send_feed_config"));
						result.setStatusConfig(rs.getInt("status_config"));
						result.setWantSeeCss(rs.getInt("want_see_css"));
						result.setRequestFriendConfig(rs.getInt("request_friend_config"));

						break; //正常只有一个结果，如果有多个，只取其中的第一个
					}
					rs.close();
				} catch (Exception e) {
					e.printStackTrace();
					Assert.assertFalse(true);
				} finally {
					s.close();
				}
			} catch (Exception e) {
				e.printStackTrace();
				Assert.assertFalse(true);
			} finally {
				c.close();
			}
		} catch (Exception e) {
			e.printStackTrace();
			Assert.assertFalse(true);
		}
		return result;
	}

	protected void writeDB(String sql) {
		try {
			Connection c = XceAdapter.getInstance().getWriteConnection("user_config");
			try {
				Statement s = c.createStatement();
				try {
					s.execute(sql);
				} catch (Exception e) {
					e.printStackTrace();
					Assert.assertFalse(true);
				} finally {
					s.close();
				}
			} catch (Exception e) {
				e.printStackTrace();
				Assert.assertFalse(true);
			} finally {
				c.close();
			}
		} catch (Exception e) {
			e.printStackTrace();
			Assert.assertFalse(true);
		}
	}

	protected void cleanBbAndCache() {
		cleanDBByUserId(_info.getId());
		_adapter.reloadUserConfig(_info.getId());
	}
	
	protected void initBbAndCache(UserConfigInfo info) {
		setDB(info.getId(), info);
		_adapter.reloadUserConfig(info.getId());
	}

	/**
	 * 初始化过程： 1. 清理DB/Cache; 2. 用参数的值初始化DB/Cache.
	 */
	protected void init() {
		cleanBbAndCache();
		initBbAndCache(_info);
	}

	protected boolean isEqual(UserConfigInfo info1, UserConfigInfo info2) {
		return info1.toString().equals(info2.toString());
	}

}