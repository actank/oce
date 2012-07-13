package com.xiaonei.xce.search.suggest;

import java.util.HashMap;
import java.util.Vector;

import com.xiaonei.search.suggest.RecordItem;
import com.xiaonei.search.suggest.SuggestCondition;
import com.xiaonei.search.suggest.SuggestManagerPrx;
import com.xiaonei.search.suggest.SuggestManagerPrxHelper;
import com.xiaonei.services.Suggestion.SuggestionManagerPrxHelper;

import xce.util.channel.Channel;
import xce.util.service.ServiceAdapter;

public class SearchSuggestAdapter extends ServiceAdapter {

  private Vector<SuggestManagerPrx> managers = new Vector<SuggestManagerPrx>();
  private static SearchSuggestAdapter instance = new SearchSuggestAdapter();
  private static final String endpoints = "@SearchSuggestion.SearchSuggestion.SearchSuggestion";

  protected SearchSuggestAdapter() {
    super(endpoints, 1, Channel.newOceChannel(endpoints));
    // TODO Auto-generated constructor stub
  }

  public static SearchSuggestAdapter getInstance() {
    return instance;
  }

  protected SuggestManagerPrx getSuggestManager() {
    System.out.println("proxy is " + locateProxy("SearchSuggestion", Channel.Invoke.Twoway,
        SuggestionManagerPrxHelper.class, 300).toString());
    return SuggestManagerPrxHelper.uncheckedCast(locateProxy("SearchSuggestion", Channel.Invoke.Twoway,
        SuggestionManagerPrxHelper.class, 300));
    
  }

  public String[] getSuggest(SuggestCondition condition, int limit) {
    return getSuggestManager().getSuggest(condition, limit);
  }

  public void reload(String suggestType) {
    getSuggestManager().reload(suggestType);
  }

  public boolean addRecord(String suggestType, RecordItem[] records) {
    return getSuggestManager().addRecord(suggestType, records);
  }

  public static void main(String[] args) {
    SuggestCondition condition = new SuggestCondition();
    condition.userid = 25701953;
    condition.queryCondition = new HashMap<String, String>();
    condition.queryCondition.put("query", "zh");
    condition.queryCondition.put("type", "Universities");
    int limit = 10;
    for(String suggest : SearchSuggestAdapter.getInstance().getSuggest(condition, limit))  {
        System.out.println(suggest);
    }
  }

}
