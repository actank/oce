{{#SEC_PHOTO_ITEM}}
<div class="friend-photo-item" id="pm-{{PHOTO_ID}}" style="display:{{CSS_DISPLAY}};">
    {{#PHOTO_DESC_EMPTY}} <h5><a target="_blank" href="http://www.renren.com/profile.do?id={{OWNER_ID}}">{{OWNER_NAME}}</a>在<a href="http://photo.renren.com/photo/{{OWNER_ID}}/photo-{{PHOTO_ID}}" title="{{ALBUM_NAME:html_escape}}" target="_blank">{{ALBUM_NAME:x-maxlen=32:html_escape}}</a>的照片</h5>
    {{/PHOTO_DESC_EMPTY}}
    {{#NOT_PHOTO_DESC_EMPTY}}
    <h5><a target="_blank" href="http://www.renren.com/profile.do?id={{OWNER_ID}}">{{OWNER_NAME}}</a>:{{PHOTO_DESC:x-maxlen=60:html_escape}}</h5>
    {{/NOT_PHOTO_DESC_EMPTY}}
    <a href="http://photo.renren.com/photo/{{OWNER_ID}}/photo-{{PHOTO_ID}}" target="_blank" class="photo"><img src="{{PHOTO_SRC_URL}}"></a>
    {{#SEC_TAG_USER}}
    <p class="contain-friend">
        照片中有：{{TAG_USER_SEP}}<a target="_blank" href="http://www.renren.com/profile.do?id={{TAG_USER_ID}}">{{TAG_USER_NAME}}</a>
    </p>
    {{/SEC_TAG_USER}}
</div>
{{/SEC_PHOTO_ITEM}}
