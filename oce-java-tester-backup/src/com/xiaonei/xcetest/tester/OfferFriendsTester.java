package com.xiaonei.xcetest.tester;

import java.util.List;

import com.xiaonei.xce.offerfriends.Offer;
import com.xiaonei.xce.offerfriends.OfferFactory;
import com.xiaonei.xce.offerfriends.OfferFriendsAdapter;

import junit.framework.TestCase;

class MyOffer extends Offer {
	
}

class MyOfferFactory implements OfferFactory<MyOffer> {
	public MyOffer create(){
		return new MyOffer();
	}
}

public class OfferFriendsTester extends TestCase {

	private OfferFriendsAdapter _ofa = new OfferFriendsAdapter(
			new MyOfferFactory());

	public void testGetOffer( ) {
		int userId = 1124;
		System.out.println("here in testGetOffer(" + userId + ")");
		List<Offer> result = _ofa.getOffer(userId);
		System.out.println(result.size());
	}

}
