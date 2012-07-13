<div id="friendPhoto">
  <div class="side-item friend-photo">
    <div class="side-item-header clearfix">
      <h4>朋友的照片</h4>
      <div class="option">
        <a title="上一页" onclick="homeFridPhoto.prePhoto();" class="{{VIEW_PREV_CLASS}}" href="#nogo"></a>
        <a title="下一页" onclick="homeFridPhoto.nextPhoto();" class="{{VIEW_NEXT_CLASS}}" href="#nogo"></a>
      </div>
    </div>
    <div class="side-item-body clearfix">
      {{>INC_PHOTO_LIST}}
    </div>
  </div>
</div>

