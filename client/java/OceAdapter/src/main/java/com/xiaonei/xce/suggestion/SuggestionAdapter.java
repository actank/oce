package com.xiaonei.xce.suggestion;

import java.util.Map;
import java.util.Vector;
import java.util.Map.Entry;

import xce.util.channel.Channel;
import xce.util.service.ServiceAdapter;

import com.xiaonei.services.Suggestion.IndexType;
import com.xiaonei.services.Suggestion.SuggestionManagerPrx;
import com.xiaonei.services.Suggestion.SuggestionManagerPrxHelper;

public class SuggestionAdapter extends ServiceAdapter {
	private static SuggestionAdapter _instance = new SuggestionAdapter();
	private static final String endpoints = "@AppSuggestion";
	private static final int serverSize = 10;
	private Vector<SuggestionManagerPrx> _managers = new Vector<SuggestionManagerPrx>();
	private Vector<SuggestionManagerPrx> _managersOneway = new Vector<SuggestionManagerPrx>();

	protected SuggestionAdapter() {
		super(endpoints, 10, Channel.newGadgetsChannel(endpoints));

	}

	public static SuggestionAdapter getInstance() {
		return _instance;
	}

	protected SuggestionManagerPrx getSuggestionManager(int num) {
		return SuggestionManagerPrxHelper.uncheckedCast(locateProxy(_managers,
				"Manager", num, Channel.Invoke.Twoway,
				SuggestionManagerPrxHelper.class, 300));
	}

	protected SuggestionManagerPrx getSuggestionManagerOneway(int num) {
		return SuggestionManagerPrxHelper.uncheckedCast(locateProxy(
				_managersOneway, "Manager", num, Channel.Invoke.Oneway,
				SuggestionManagerPrxHelper.class, 300));
	}

	public void buildSuggestion(IndexType type) {
		for (int i = 0; i < serverSize; ++i)
			getSuggestionManager(i).buildIndex(IndexType.App);
	}

	public Map<Integer, String> searchSuggestion(IndexType type,
			String condition, int limit) {
		if (condition == null || limit == 0 || condition.trim().length() == 0)
			return null;
		return getSuggestionManager((int) (Math.random() * serverSize)).search(
				IndexType.App, condition, limit);
	}

	public static void main(String[] args) {
		SuggestionAdapter.getInstance().buildSuggestion(IndexType.App);
		try {
			Map<Integer, String> res = SuggestionAdapter.getInstance()
					.searchSuggestion(IndexType.App, "kaixinno", 10);
			System.out.println(res.size());
			for (Entry<Integer, String> entry : res.entrySet()) {
				System.out.println(entry.getKey() + " --> " + entry.getValue());
			}

		} catch (Exception e) {
			System.out.println(e);
		}

		System.exit(0);
	}
}
