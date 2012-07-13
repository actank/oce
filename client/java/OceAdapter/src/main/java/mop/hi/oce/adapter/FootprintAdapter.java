package mop.hi.oce.adapter;

import java.util.List;

import mop.hi.oce.domain.model.FootprintView;
import mop.hi.svc.fpt.FootprintException;

public interface FootprintAdapter {
	
	@Deprecated
	public void visitHomepage(FootprintView fpt);
	

	@Deprecated
	public void visitHomepage(FootprintView fpt, boolean friendFlag);
	
	public void visitHomepage(FootprintView fpt, boolean friendFlag, String ref);
	
	public void visitHomepage(FootprintView fpt, boolean friendFlag, String ref, boolean ignoreLimitFlag);
	/**
	 * 
	 * @param hostId
	 * @return return the footprint list
	 */
	public List<FootprintView> getAll(long hostId) throws FootprintException;
	
	/**
	 * @brief getAll (int hostid)
	 */
	@Deprecated
	public List<FootprintView> getAll(int hostId) throws FootprintException;

	/**
	 * 
	 * @param hostId
	 * @param begin
	 *            initialize is 0
	 * @param limit
	 *            -1:get all 0~n: return n footprints
	 * @return
	 */
	public List<FootprintView> getAll(long hostId, int begin, int limit)
			throws FootprintException;
	
	/**
	 * @beirf getAll (int type hostId)
	 */
	@Deprecated
	public List<FootprintView> getAll(int hostId, int begin, int limit)
	throws FootprintException;
	
	/** 
	 * @param hostId
	 * @return size of footprints of user
	 * @throws FootprintException
	 */
	public int getSize(long hostId) throws FootprintException;

	@Deprecated
	public int getSize(int hostId) throws FootprintException;
}
