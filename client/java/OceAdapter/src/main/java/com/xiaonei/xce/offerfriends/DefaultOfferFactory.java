package com.xiaonei.xce.offerfriends;

public class DefaultOfferFactory implements OfferFactory<DefaultOffer> {

	@Override
	public DefaultOffer create() {
		return new DefaultOffer();
	}

}
