package mop.hi.oce.adapter;


public interface BatchUpdaterSelfAdapter {

	public void incUgcUserScore(int userId, int type, int score);

	public void decUgcUserScore(int userId, int type, int score);

	public void incUserScore(int userId, int score);

	public void decUserScore(int userId, int score);

	void incMessageInCount(int id);

	void decMessageInCount(int id);

	void incMessageInCount(int id, int value);

	void decMessageInCount(int id, int value);

	void incMessageOutCount(int id);

	void decMessageOutCount(int id);

	void incMessageOutCount(int id, int value);

	void decMessageOutCount(int id, int value);

	void incBlogViewCount(int id, int userId, int count);

	void decBlogViewCount(int id, int userId, int count);

	@Deprecated
	void incPhotoViewCount(int id, int userId, int count);

	@Deprecated
	void decPhotoViewCount(int id, int userId, int count);

	void incMessageInCount(long id);

	void decMessageInCount(long id);

	void incMessageInCount(long id, int value);

	void decMessageInCount(long id, int value);

	void incMessageOutCount(long id);

	void decMessageOutCount(long id);

	void incMessageOutCount(long id, int value);

	void decMessageOutCount(long id, int value);

	void incBlogViewCount(long id, int userId, int count);

	void decBlogViewCount(long id, int userId, int count);

	@Deprecated
	void incPhotoViewCount(long id, int userId, int count);

	@Deprecated
	void decPhotoViewCount(long id, int userId, int count);
	
	void incPhotoViewCount(long photoId, long albumId, long ownerId, int increment);

}
