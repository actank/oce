package com.xiaonei.xce.test.notifyrpl;

import java.util.Arrays;
import java.util.Collection;
import java.util.Vector;

import mop.hi.svc.dispatch.DispatchException;
import mop.hi.svc.dispatch.Note;
import mop.hi.svc.dispatch.NotifierPrx;
import mop.hi.svc.dispatch.NotifierPrxHelper;

import org.junit.After;
import org.junit.AfterClass;
import org.junit.Assert;
import org.junit.Before;
import org.junit.BeforeClass;
import org.junit.Test;
import org.junit.runner.RunWith;
import org.junit.runners.Parameterized;
import org.junit.runners.Parameterized.Parameters;

import xce.util.channel.Channel;
import xce.util.service.ServiceAdapter;

//import org.junit.internal.runners.TestClassRunner;

@RunWith(Parameterized.class)
public class NotifyRplTestCase {
	private static NotifyRplAdapter _adapter;
	private long _noteId;
	private int _quoteeId;
	private int _resource;
	private int _type;
	private boolean _unread;
	private String _fromName;
	private int _fromId;
	private int _toId;
	private String _ownerName;
	private int _ownerId;
	private String _title;
	private String _link;
	private String _imageLink;
	private int _timestamp;
	private int _security;
	private int[] _userIds;

	public NotifyRplTestCase(long noteId, int quoteeId, int resource, int type,
			boolean unread, String fromName, int fromId, int toId,
			String ownerName, int ownerId, String title, String link,
			String imageLink, int timestamp, int security, int[] userIds) {
		this._noteId = noteId;
		this._quoteeId = quoteeId;
		this._resource = resource;
		this._type = type;
		this._unread = unread;
		this._fromName = fromName;
		this._fromId = fromId;
		this._toId = toId;
		this._ownerName = ownerName;
		this._ownerId = ownerId;
		this._title = title;
		this._link = link;
		this._imageLink = imageLink;
		this._timestamp = timestamp;
		this._security = security;
		this._userIds = userIds;
	}

	@Parameters
	public static Collection<Object[]> setParameters() {
		return Arrays
				.asList(new Object[][] {
						{
								16365,
								0,
								287603079,
								65549,
								true,
								"叶湘",
								700000001,
								700000002,
								"张函",
								700000002,
								"ttttitle",
								"http://xiaonei.com/getdoing.do?doingId=287603079#287603079",
								"", 0, 1, new int[] { 700000002 } }, // 测试第一组
						{
								12181,
								0,
								287603079,
								65549,
								true,
								"叶湘",
								700000001,
								700000003,
								"赵宁",
								700000003,
								"tttttttttitle",
								"http://xiaonei.com/getdoing.do?doingId=287603079#287603079",
								"", 0, 1, new int[] { 700000003 } }, // 测试第二组
				});
	}

	@BeforeClass
	public static void setUpBeforeClass() throws Exception {
		_adapter = NotifyRplAdapter.getInstance();
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
	public void test() {
		Note theNote = new Note(_noteId, _quoteeId, _resource, _type, _unread,
				_fromName, _fromId, _toId, _ownerName, _ownerId, _title, _link,
				_imageLink, _timestamp, _security);
		_adapter.deliver(theNote, _userIds);
		System.out.println("theNote delivered");

		for (int userId : _userIds) {
			Note[] notes = _adapter.getAll(userId, _type, false, 0, -1);
			boolean delivered = false;
			Note theOne = null;
			for (Note note : notes) {
				if (_noteId == note.noteId) {
					delivered = true;
					theOne = note;
					break;
				}
			}
			Assert.assertTrue(delivered);
			Assert.assertEquals(_quoteeId, theOne.quoteeId);
			Assert.assertEquals(_resource, theOne.resource);
			Assert.assertEquals(_type, theOne.type);
			Assert.assertEquals(_unread, theOne.unread);
			Assert.assertEquals(_fromName, theOne.fromName);
			Assert.assertEquals(_fromId, theOne.fromId);
			Assert.assertEquals(_toId, theOne.toId);
			Assert.assertEquals(_ownerName, theOne.ownerName);
			Assert.assertEquals(_ownerId, theOne.ownerId);
			Assert.assertEquals(_title, theOne.title);
			Assert.assertEquals(_link, theOne.link);
			Assert.assertEquals(_imageLink, theOne.imageLink);
			Assert.assertEquals(_timestamp, theOne.timestamp);
			Assert.assertEquals(_security, theOne.security);
		}
	}
}

class NotifyRplAdapter extends ServiceAdapter {
	private static NotifyRplAdapter _adapter = new NotifyRplAdapter();
	private static final String endpoints = "@NotifyRpl";

	private Vector<NotifierPrx> managers = new Vector<NotifierPrx>();
	private Vector<NotifierPrx> managersOneway = new Vector<NotifierPrx>();

	protected NotifyRplAdapter() {
		super(endpoints, 10, Channel.newOceChannel(endpoints));
	}

	public static NotifyRplAdapter getInstance() {
		return _adapter;
	}

	protected NotifierPrx getNotifyRplManager(int userId) {
		return NotifierPrxHelper.uncheckedCast(locateProxy(managers, "N",
				userId, Channel.Invoke.Twoway, NotifierPrxHelper.class, 300));
	}

	protected NotifierPrx getNotifyRplManagerOneway(int userId) {
		return NotifierPrxHelper.uncheckedCast(locateProxy(managersOneway, "N",
				userId, Channel.Invoke.Oneway, NotifierPrxHelper.class, 300));
	}

	public Note[] getAll(int userId, int type, boolean unreadOnly, int begin,
			int limit) {
		try {
			return getNotifyRplManager(userId).getAll(userId, type, unreadOnly,
					begin, limit);
		} catch (DispatchException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		return null;
	}

	public int getCount(int userId, int type, boolean unreadOnly) {
		try {
			return getNotifyRplManager(userId).getCount(userId, type,
					unreadOnly);
		} catch (DispatchException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		return -1;
	}

	public void deliver(Note theNote, int[] userIds) {
		for (int it : userIds) {
			getNotifyRplManager(it)._notify(theNote, userIds);
		}
	}

}
