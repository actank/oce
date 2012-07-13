package com.xiaonei.xce.offerfriends;

public interface OfferFactory<T extends Offer> {
	public T create();
}
