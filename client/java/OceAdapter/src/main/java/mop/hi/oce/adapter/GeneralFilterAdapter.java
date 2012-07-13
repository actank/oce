package mop.hi.oce.adapter;

import java.util.Map;

import mop.hi.oce.domain.model.GeneralVisitInformation;

public interface GeneralFilterAdapter {
	/**
	 * 
	 * @param fpt
	 *            footprint stuct
	 * 
	 */
	public int passFilter(GeneralVisitInformation info);
	public int filterControl(Map cmd, int type);
}
