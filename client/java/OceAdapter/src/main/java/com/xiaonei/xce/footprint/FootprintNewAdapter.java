package com.xiaonei.xce.footprint;

import java.util.List;
import mop.hi.oce.domain.model.FootprintNewView;
import mop.hi.oce.domain.model.FootprintResult;

public interface FootprintNewAdapter {
	
	@Deprecated
	public void  visitHomepage(FootprintNewView fpt);

	@Deprecated
	public void  visitHomepage(FootprintNewView fpt, boolean friendflag);
	
	public void visitHomepage(FootprintNewView fpt, boolean friendflag, String ref);
	
	public void visitHomepage(FootprintNewView fpt, boolean friendflag, String ref, boolean nocount);
	
	public FootprintResult visitHomepageAndGet(FootprintNewView fpt, boolean friendflag, String ref);
	
	public FootprintResult visitHomepageAndGet(FootprintNewView fpt, boolean friendflag, String ref, boolean nocount);
	
	/**
	 * 
	 * @param hostId
	 * @param count
	 * @return change the view count of hostId to count
	 */
	public void setUserVisitCount(int hostId, int count);
	
	/**
	 * 
	 * @param hostId
	 * @return return the user view count
	 */
	public int getUserVisitCount(int hostId);

	/**
	 * 
	 * @param hostId
	 * @return return the footprint list
	 */
	public FootprintResult getAll(int hostId);

	/**
	 * 
	 * @param hostId
	 * @param begin
	 *            initialize is 0
	 * @param limit
	 *            -1:get all 0~n: return n footprints
	 * @return
	 */
	public FootprintResult getAll(int hostId, int begin, int limit);

	public int getSize(int hostId);	
}