var jebeList = {
  "cupid_js_version":"14087",
  "cupid_css_version":"2",
  "list" : [ {{#SEC_AD_ZONES}}
    {{AD_ZONE_SEP}}
    { 
      "next_load_time":600000,
      "flush_type":0,
      "adzone_id":"{{AD_ZONE_ID}}",
      "ads" : 
        [
          {{#SEC_AD_ITEMS}}
          {{AD_ITEM_SEP}}
          {
            "widget_id" : "{{AD_WIDGET_ID}}",
            "widget_version" : "{{AD_WIDGET_VERSION}}",
            "widget" : "{{AD_WIDGET}}",

            "ad_param": {
              "creative_id": "{{AD_CREATIVE_ID}}",
              "title":"{{AD_TITLE}}",
              "content":"{{AD_CONTENT}}",
              "click_url":"http://ebp.renren.com/ebp/click.html?aid={{AD_CREATIVE_ID}}&url={{AD_DEST_URL}}&mc={{AD_MATCHER_COOKIE}}",
              "media_uri":"{{AD_MEDIA_URI}}"
            }
          }
          {{/SEC_AD_ITEMS}}
        ]
      } {{/SEC_AD_ZONES}} 
  ]
};

