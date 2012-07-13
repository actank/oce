package com.xiaonei.xce.passport;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.Map.Entry;

import junit.framework.TestCase;

import org.junit.Assert;

import com.xiaonei.xce.passport.Login.Type;

public class LoginLogicTestCase extends TestCase {
    private class Input {
	private List<Type> types_;
	private String account_;
	private String password_;
	private String domain_;

	public Input(List<Type> types, String account, String password,
		String domain) {
	    types_ = types;
	    account_ = account;
	    password_ = password;
	    domain_ = domain;
	}

	public List<Type> getTypes() {
	    return types_;
	}

	public String getAccount() {
	    return account_;
	}

	public String getPassword() {
	    return password_;
	}

	public String getDomain() {
	    return domain_;
	}
    }

    private class Output {
	private int id_;
	private String account_;
	private String domain_;
	private int success_;

	public Output(int id, String account, String domain, int success) {
	    id_ = id;
	    account_ = account;
	    domain_ = domain;
	    success_ = success;
	}

	public int getId() {
	    return id_;
	}

	public String getAccount() {
	    return account_;
	}

	public String getDomain() {
	    return domain_;
	}

	public int getSuccess() {
	    return success_;
	}

	public String toString() {
	    StringBuffer sb = new StringBuffer();
	    sb.append("id=" + id_ + ";account=" + account_ + ";success="
		    + success_ + ";domain=" + domain_);
	    return sb.toString();
	}
    }

    private Map<Input, Output> map;

    private void setUpPhone() {

	List<Type> list = new ArrayList<Type>();
	list.add(Type.Phone);

	map.put(new Input(list, "13488813147",
		"ec97eded1c9edb62917d9f3540661fe9", "renren.com"), new Output(
		-1, "", "", -1));
	map.put(new Input(list, "13488813147",
		"ec97eded1c9edb62917d9f3540661fe9", "kaixin.com"), new Output(
		-1, "", "", -1));
	map.put(new Input(list, "13488813147",
		"ec97eded1c9edb62917d9f3540661fe9", "nuomi.com"), new Output(
		-1, "", "", -1));
	map.put(new Input(list, "13488813147",
		"ec97eded1c9edb62917d9f3540661fe9", "imop.com"), new Output(-1,
		"", "", -1));

	map.put(new Input(list, "13488813146", "wrong", "renren.com"),
		new Output(368927241, "13488813146", "renren.com", -2));
	map.put(new Input(list, "13488813146", "wrong", "kaixin.com"),
		new Output(368927241, "13488813146", "renren.com", -2));
	map.put(new Input(list, "13488813146", "wrong", "nuomi.com"),
		new Output(368927241, "13488813146", "renren.com", -2));
	map.put(new Input(list, "13488813146", "wrong", "imop.com"),
		new Output(368927241, "13488813146", "renren.com", -2));

	map.put(new Input(list, "13488813146",
		"ec97eded1c9edb62917d9f3540661fe9", "renren.com"), new Output(
		368927241, "13488813146", "renren.com", 0));
	map.put(new Input(list, "13488813146",
		"ec97eded1c9edb62917d9f3540661fe9", "kaixin.com"), new Output(
		368927241, "13488813146", "renren.com", 0));
	map.put(new Input(list, "13488813146",
		"ec97eded1c9edb62917d9f3540661fe9", "nuomi.com"), new Output(
		368927241, "13488813146", "renren.com", 0));
	map.put(new Input(list, "13488813146",
		"ec97eded1c9edb62917d9f3540661fe9", "imop.com"), new Output(
		368927241, "13488813146", "renren.com", 0));
    }

    private void setUpNickName() {
	List<Type> list = new ArrayList<Type>();
	list.add(Type.BindedId);

	map.put(new Input(list, "xiaoneitest_1",
		"ec97eded1c9edb62917d9f3540661fe9", "renren.com"), new Output(
		-1, "", "", -1));
	map.put(new Input(list, "xiaoneitest_1",
		"ec97eded1c9edb62917d9f3540661fe9", "kaixin.com"), new Output(
		-1, "", "", -1));
	map.put(new Input(list, "xiaoneitest_1",
		"ec97eded1c9edb62917d9f3540661fe9", "nuomi.com"), new Output(
		-1, "", "", -1));
	map.put(new Input(list, "xiaoneitest_1",
		"ec97eded1c9edb62917d9f3540661fe9", "imop.com"), new Output(-1,
		"", "", -1));

	map.put(new Input(list, "xiaoneitest_0", "wrong", "renren.com"),
		new Output(257895598, "xiaoneitest_0", "renren.com", -1));
	map.put(new Input(list, "xiaoneitest_0", "wrong", "kaixin.com"),
		new Output(257895598, "xiaoneitest_0", "renren.com", -1));
	map.put(new Input(list, "xiaoneitest_0", "wrong", "nuomi.com"),
		new Output(257895598, "xiaoneitest_0", "renren.com", -1));
	map.put(new Input(list, "xiaoneitest_0", "wrong", "imop.com"),
		new Output(257895598, "xiaoneitest_0", "renren.com", -1));

	map.put(new Input(list, "xiaoneitest_0",
		"ec97eded1c9edb62917d9f3540661fe9", "renren.com"), new Output(
		257895598, "xiaoneitest_0", "renren.com", 0));
	map.put(new Input(list, "xiaoneitest_0",
		"ec97eded1c9edb62917d9f3540661fe9", "kaixin.com"), new Output(
		257895598, "xiaoneitest_0", "renren.com", 0));
	map.put(new Input(list, "xiaoneitest_0",
		"ec97eded1c9edb62917d9f3540661fe9", "nuomi.com"), new Output(
		257895598, "xiaoneitest_0", "renren.com", 0));
	map.put(new Input(list, "xiaoneitest_0",
		"ec97eded1c9edb62917d9f3540661fe9", "imop.com"), new Output(
		257895598, "xiaoneitest_0", "renren.com", 0));
    }

    private void setUpId() {
	List<Type> list = new ArrayList<Type>();
	list.add(Type.Id);
	map.put(new Input(list, "6812668126",
		"ec97eded1c9edb62917d9f3540661fe9", "renren.com"), new Output(
		-1, "", "", -1));
	map.put(new Input(list, "6812668126",
		"ec97eded1c9edb62917d9f3540661fe9", "kaixin.com"), new Output(
		-1, "", "", -1));
	map.put(new Input(list, "6812668126",
		"ec97eded1c9edb62917d9f3540661fe9", "nuomi.com"), new Output(
		-1, "", "", -1));
	map.put(new Input(list, "6812668126",
		"ec97eded1c9edb62917d9f3540661fe9", "imop.com"), new Output(-1,
		"", "", -1));

	map.put(new Input(list, "257895598", "wrong", "renren.com"),
		new Output(257895598, "xiaoneitest_0@126.com", "renren.com", -2));
	map.put(new Input(list, "257895598", "wrong", "kaixin.com"),
		new Output(257895598, "xiaoneitest_0@126.com", "renren.com", -2));
	map.put(new Input(list, "257895598", "wrong", "nuomi.com"), new Output(
		257895598, "xiaoneitest_0@126.com", "renren.com", -2));
	map.put(new Input(list, "257895598", "wrong", "imop.com"), new Output(
		257895598, "xiaoneitest_0@126.com", "renren.com", -2));

	map.put(new Input(list, "257895598",
		"ec97eded1c9edb62917d9f3540661fe9", "renren.com"), new Output(
		257895598, "xiaoneitest_0@126.com", "renren.com", 0));
	map.put(new Input(list, "257895598",
		"ec97eded1c9edb62917d9f3540661fe9", "kaixin.com"), new Output(
		257895598, "xiaoneitest_0@126.com", "renren.com", 0));
	map.put(new Input(list, "257895598",
		"ec97eded1c9edb62917d9f3540661fe9", "nuomi.com"), new Output(
		257895598, "xiaoneitest_0@126.com", "renren.com", 0));
	map.put(new Input(list, "257895598",
		"ec97eded1c9edb62917d9f3540661fe9", "imop.com"), new Output(
		257895598, "xiaoneitest_0@126.com", "renren.com", 0));

    }

    private void setUpAccount() {
	List<Type> list = new ArrayList<Type>();
	list.add(Type.Account);
	map.put(new Input(list, "xiaoneitest_00@126.com",
		"ec97eded1c9edb62917d9f3540661fe9", "renren.com"), new Output(
		-1, "", "", -1));
	map.put(new Input(list, "xiaoneitest_00@126.com",
		"ec97eded1c9edb62917d9f3540661fe9", "kaixin.com"), new Output(
		-1, "", "", -1));
	map.put(new Input(list, "xiaoneitest_00@126.com",
		"ec97eded1c9edb62917d9f3540661fe9", "nuomi.com"), new Output(
		-1, "", "", -1));
	map.put(new Input(list, "xiaoneitest_00@126.com",
		"ec97eded1c9edb62917d9f3540661fe9", "imop.com"), new Output(-1,
		"", "", -1));

	map.put(new Input(list, "xiaoneitest_0@126.com", "wrong", "renren.com"),
		new Output(257895598, "xiaoneitest_0@126.com", "renren.com", -2));
	map.put(new Input(list, "xiaoneitest_0@126.com", "wrong", "kaixin.com"),
		new Output(257895598, "xiaoneitest_0@126.com", "renren.com", -2));
	map.put(new Input(list, "xiaoneitest_0@126.com", "wrong", "nuomi.com"),
		new Output(257895598, "xiaoneitest_0@126.com", "renren.com", -2));
	map.put(new Input(list, "xiaoneitest_0@126.com", "wrong", "imop.com"),
		new Output(257895598, "xiaoneitest_0@126.com", "renren.com", -2));

	map.put(new Input(list, "xiaoneitest_0@126.com",
		"ec97eded1c9edb62917d9f3540661fe9", "renren.com"), new Output(
		257895598, "xiaoneitest_0@126.com", "renren.com", 0));
	map.put(new Input(list, "xiaoneitest_0@126.com",
		"ec97eded1c9edb62917d9f3540661fe9", "kaixin.com"), new Output(
		257895598, "xiaoneitest_0@126.com", "renren.com", 0));
	map.put(new Input(list, "xiaoneitest_0@126.com",
		"ec97eded1c9edb62917d9f3540661fe9", "nuomi.com"), new Output(
		257895598, "xiaoneitest_0@126.com", "renren.com", 0));
	map.put(new Input(list, "xiaoneitest_0@126.com",
		"ec97eded1c9edb62917d9f3540661fe9", "imop.com"), new Output(
		257895598, "xiaoneitest_0@126.com", "renren.com", 0));

	map.put(new Input(list, "xiaoneitest_1@126.com", "wrong", "renren.com"),
		new Output(252834512, "xiaoneitest_1@126.com", "renren.com", -2));
	map.put(new Input(list, "xiaoneitest_1@126.com", "wrong", "kaixin.com"),
		new Output(252834512, "xiaoneitest_1@126.com", "renren.com", -2));
	map.put(new Input(list, "xiaoneitest_1@126.com", "wrong", "nuomi.com"),
		new Output(252834512, "xiaoneitest_1@126.com", "renren.com", -2));
	map.put(new Input(list, "xiaoneitest_1@126.com", "wrong", "imop.com"),
		new Output(252834512, "xiaoneitest_1@126.com", "renren.com", -2));

	map.put(new Input(list, "xiaoneitest_1@126.com",
		"ec97eded1c9edb62917d9f3540661fe9", "renren.com"), new Output(
		252834512, "xiaoneitest_1@126.com", "renren.com", 0));
	map.put(new Input(list, "xiaoneitest_1@126.com",
		"ec97eded1c9edb62917d9f3540661fe9", "kaixin.com"), new Output(
		252834512, "xiaoneitest_1@126.com", "renren.com", 0));
	map.put(new Input(list, "xiaoneitest_1@126.com",
		"ec97eded1c9edb62917d9f3540661fe9", "nuomi.com"), new Output(
		252834512, "xiaoneitest_1@126.com", "renren.com", 0));
	map.put(new Input(list, "xiaoneitest_1@126.com",
		"ec97eded1c9edb62917d9f3540661fe9", "imop.com"), new Output(
		252834512, "xiaoneitest_1@126.com", "renren.com", 0));

	map.put(new Input(list, "xiaoneitest_1@126.com",
		"afc87889b38855023d0a1ff991c758d3", "renren.com"), new Output(
		707531437, "xiaoneitest_1@126.com", "kaixin.com", 0));
	map.put(new Input(list, "xiaoneitest_1@126.com",
		"afc87889b38855023d0a1ff991c758d3", "kaixin.com"), new Output(
		707531437, "xiaoneitest_1@126.com", "kaixin.com", 0));
	map.put(new Input(list, "xiaoneitest_1@126.com",
		"afc87889b38855023d0a1ff991c758d3", "nuomi.com"), new Output(
		707531437, "xiaoneitest_1@126.com", "kaixin.com", 0));
	map.put(new Input(list, "xiaoneitest_1@126.com",
		"afc87889b38855023d0a1ff991c758d3", "imop.com"), new Output(
		707531437, "xiaoneitest_1@126.com", "kaixin.com", 0));

	map.put(new Input(list, "xiaoneitest_2@126.com", "wrong", "renren.com"),
		new Output(252891891, "xiaoneitest_2@126.com", "renren.com", -2));
	map.put(new Input(list, "xiaoneitest_2@126.com", "wrong", "kaixin.com"),
		new Output(252891891, "xiaoneitest_2@126.com", "renren.com", -2));
	map.put(new Input(list, "xiaoneitest_2@126.com", "wrong", "nuomi.com"),
		new Output(252891891, "xiaoneitest_2@126.com", "renren.com", -2));
	map.put(new Input(list, "xiaoneitest_2@126.com", "wrong", "imop.com"),
		new Output(252891891, "xiaoneitest_2@126.com", "renren.com", -2));

	map.put(new Input(list, "xiaoneitest_2@126.com",
		"ec97eded1c9edb62917d9f3540661fe9", "renren.com"), new Output(
		707560675, "xiaoneitest_2@126.com", "kaixin.com", 0));
	map.put(new Input(list, "xiaoneitest_2@126.com",
		"ec97eded1c9edb62917d9f3540661fe9", "kaixin.com"), new Output(
		707560675, "xiaoneitest_2@126.com", "kaixin.com", 0));
	map.put(new Input(list, "xiaoneitest_2@126.com",
		"ec97eded1c9edb62917d9f3540661fe9", "nuomi.com"), new Output(
		707560675, "xiaoneitest_2@126.com", "kaixin.com", 0));
	map.put(new Input(list, "xiaoneitest_2@126.com",
		"ec97eded1c9edb62917d9f3540661fe9", "imop.com"), new Output(
		707560675, "xiaoneitest_2@126.com", "kaixin.com", 0));

	map.put(new Input(list, "xiaoneitest_4@126.com",
		"ec97eded1c9edb62917d9f3540661fe9", "renren.com"), new Output(
		253071631, "xiaoneitest_4@126.com", "renren.com", 0));
	map.put(new Input(list, "xiaoneitest_4@126.com",
		"ec97eded1c9edb62917d9f3540661fe9", "kaixin.com"), new Output(
		253071631, "xiaoneitest_4@126.com", "renren.com", 0));
	map.put(new Input(list, "xiaoneitest_4@126.com",
		"ec97eded1c9edb62917d9f3540661fe9", "nuomi.com"), new Output(
		253071631, "xiaoneitest_4@126.com", "renren.com", 0));
	map.put(new Input(list, "xiaoneitest_4@126.com",
		"ec97eded1c9edb62917d9f3540661fe9", "imop.com"), new Output(
		253071631, "xiaoneitest_4@126.com", "renren.com", 0));

	map.put(new Input(list, "xiaoneitest_5@126.com",
		"ec97eded1c9edb62917d9f3540661fe9", "renren.com"), new Output(
		708582319, "xiaoneitest_5@126.com", "kaixin.com", 0));
	map.put(new Input(list, "xiaoneitest_5@126.com",
		"ec97eded1c9edb62917d9f3540661fe9", "kaixin.com"), new Output(
		708582319, "xiaoneitest_5@126.com", "kaixin.com", 0));
	map.put(new Input(list, "xiaoneitest_5@126.com",
		"ec97eded1c9edb62917d9f3540661fe9", "nuomi.com"), new Output(
		708582319, "xiaoneitest_5@126.com", "kaixin.com", 0));
	map.put(new Input(list, "xiaoneitest_5@126.com",
		"ec97eded1c9edb62917d9f3540661fe9", "imop.com"), new Output(
		708582319, "xiaoneitest_5@126.com", "kaixin.com", 0));

	map.put(new Input(list, "xiaoneitest_6@126.com",
		"ec97eded1c9edb62917d9f3540661fe9", "renren.com"), new Output(
		253563668, "xiaoneitest_6@126.com", "renren.com", 0));
	map.put(new Input(list, "xiaoneitest_6@126.com",
		"ec97eded1c9edb62917d9f3540661fe9", "kaixin.com"), new Output(
		253563668, "xiaoneitest_6@126.com", "renren.com", 0));
	map.put(new Input(list, "xiaoneitest_6@126.com",
		"ec97eded1c9edb62917d9f3540661fe9", "nuomi.com"), new Output(
		253563668, "xiaoneitest_6@126.com", "renren.com", 0));
	map.put(new Input(list, "xiaoneitest_6@126.com",
		"ec97eded1c9edb62917d9f3540661fe9", "imop.com"), new Output(
		253563668, "xiaoneitest_6@126.com", "renren.com", 0));

	map.put(new Input(list, "xiaoneitest_3@126.com",
		"ec97eded1c9edb62917d9f3540661fe9", "renren.com"), new Output(
			252986885, "xiaoneitest_3@126.com", "renren.com", 0));
	map.put(new Input(list, "xiaoneitest_3@126.com",
		"ec97eded1c9edb62917d9f3540661fe9", "kaixin.com"), new Output(
			252986885, "xiaoneitest_3@126.com", "renren.com", 0));
	map.put(new Input(list, "xiaoneitest_3@126.com",
		"ec97eded1c9edb62917d9f3540661fe9", "nuomi.com"), new Output(
			252986885, "xiaoneitest_3@126.com", "renren.com", 0));
	map.put(new Input(list, "xiaoneitest_3@126.com",
		"ec97eded1c9edb62917d9f3540661fe9", "imop.com"), new Output(
			252986885, "xiaoneitest_3@126.com", "renren.com", 0));
    }

    private void setUpAccountAndId() {
	List<Type> list = new ArrayList<Type>();
	list.add(Type.Account);
	list.add(Type.Id);
	map.put(new Input(list, "257895598",
		"afc87889b38855023d0a1ff991c758d3", "renren.com"), new Output(
		368923544, "257895598", "renren.com", 0));
	map.put(new Input(list, "257895598",
		"ec97eded1c9edb62917d9f3540661fe9", "renren.com"), new Output(
		368923544, "257895598", "renren.com", -2));

	list = new ArrayList<Type>();
	list.add(Type.Id);
	list.add(Type.Account);
	map.put(new Input(list, "257895598",
		"ec97eded1c9edb62917d9f3540661fe9", "renren.com"), new Output(
		257895598, "xiaoneitest_0@126.com", "renren.com", 0));
	map.put(new Input(list, "257895598",
		"afc87889b38855023d0a1ff991c758d3", "renren.com"), new Output(
		368923544, "257895598", "renren.com", 0));
    }

    public void setUp() {
	System.setProperty("TestLoginLogic", "ControllerTestLoginLogic");
	map = new HashMap<Input, Output>();
	setUpId();
	setUpNickName();
	setUpPhone();
	setUpAccount();
	setUpAccountAndId();
    }

    public void test() {
	for (Entry<Input, Output> e : map.entrySet()) {
	    Account a = PassportAdapterFactory.getWebLogin()
		    .login(e.getKey().getTypes(), e.getKey().getAccount(),
			    e.getKey().getPassword(), e.getKey().getDomain(),
			    -1, false);
	    if (a.getSuccess() == 0) {
		check(e.getValue(), a);
		String ticket = a.getTicket();
		try {
		    Thread.sleep(1000);
		} catch (Exception e1) {
		    e1.printStackTrace();
		}

		a = PassportAdapterFactory.getWebLogin().login(
			e.getKey().getTypes(), e.getKey().getAccount(),
			e.getKey().getPassword(), e.getKey().getDomain(), -1,
			false);
		check(e.getValue(), a);
		Assert.assertTrue(!a.getTicket().equals(ticket));
		ticket = a.getTicket();
		try {
		    Thread.sleep(1000);
		} catch (Exception e1) {
		    e1.printStackTrace();
		}
		a = PassportAdapterFactory.getWebLogin().login(
			e.getKey().getTypes(), e.getKey().getAccount(),
			e.getKey().getPassword(), e.getKey().getDomain(), -1,
			true);
		check(e.getValue(), a);
		Assert.assertEquals(ticket, a.getTicket());

	    } else {
		check(e.getValue(), a);
	    }
	}
    }

    private void check(Output o, Account a) {
	Assert.assertTrue(
		"target: " + o.toString() + " result" + a.toString(),
		o.getAccount().equals(a.getAccount()) && o.getId() == a.getId()
			&& o.getDomain().equals(a.getDomain())
			&& o.getSuccess() == a.getSuccess());
    }
}
