package mop.hi.oce.adapter;

import java.util.Map;

import mop.hi.oce.domain.model.importer.ImporterBuddyList;
import mop.hi.oce.domain.model.importer.ImporterOwner;

public interface ImporterNAdapter {
	public void sendRequest(ImporterOwner owner);

	public ImporterBuddyList getResponse(ImporterOwner owner);
	
	public ImporterBuddyList getGMailResponse(ImporterOwner owner);

	public void sendMSNRequest(ImporterOwner owner);
	
	public void sendGMailRequest(ImporterOwner owner);
	
	public void sendNeteaseRequest(ImporterOwner owner);
	
	public void send126Request(ImporterOwner owner);
	
	public void sendYahooRequest(ImporterOwner owner);
	
	public void sendSinaRequest(ImporterOwner owner);

	public ImporterBuddyList getMSNResponse(ImporterOwner owner);

	public void sendCSVRequest(ImporterOwner owner, Map<String, String> contacts);

	public ImporterBuddyList getCSVResponse(ImporterOwner owner);
	
	public ImporterBuddyList getNeteaseResponse(ImporterOwner owner);
	
	public ImporterBuddyList get126Response(ImporterOwner owner);
	
	public ImporterBuddyList getYahooResponse(ImporterOwner owner);
	
	public ImporterBuddyList getSinaResponse(ImporterOwner owner);
	
	public boolean verifyMSNAccount(String liveId, String livePassword);
}
