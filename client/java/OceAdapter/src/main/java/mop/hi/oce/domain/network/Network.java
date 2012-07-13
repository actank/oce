package mop.hi.oce.domain.network;

import java.util.Date;

import mop.hi.oce.domain.Updatable;
import mop.hi.svc.model.NetColID;
import mop.hi.svc.model.NetColINFOID;
import mop.hi.svc.model.NetColJOINTIME;
import mop.hi.svc.model.NetColNetworkId;
import mop.hi.svc.model.NetColNetworkName;
import mop.hi.svc.model.NetColSTAGE;
import mop.hi.svc.model.NetColSTATUS;
import mop.hi.svc.model.NetColUSERID;
import xce.util.tools.DateFormatter;


public class Network extends Updatable {
	public static final String ID = NetColID.value;

	public static final String USERID = NetColUSERID.value;

	public static final String STAGE = NetColSTAGE.value;

	public static final String INFO_ID = NetColINFOID.value;

	public static final String JOIN_TIME = NetColJOINTIME.value;

	public static final String STATUS = NetColSTATUS.value;

	public static final String NETWORK_NAME = NetColNetworkName.value;

	public static final String NETWORK_ID = NetColNetworkId.value;

	private mop.hi.svc.model.Network network = new mop.hi.svc.model.Network();

	public String toString()
	{
		StringBuffer str = new StringBuffer();
		
		str.append(this.ID).append("=").append(String.valueOf(this.getId())).append("\n");
		str.append(this.USERID).append("=").append(String.valueOf(this.getUserId())).append("\n");
		str.append(this.STAGE).append("=").append(String.valueOf(this.getStage())).append("\n");
		str.append(this.STATUS).append("=").append(String.valueOf(this.getStatus())).append("\n");
		str.append(this.INFO_ID).append("=").append(String.valueOf(this.getInfoId())).append("\n");
		str.append(this.JOIN_TIME).append("=").append(String.valueOf(this.getJoinTime())).append("\n");
		str.append(this.NETWORK_ID).append("=").append(String.valueOf(this.getNetworkId())).append("\n");
		str.append(this.NETWORK_NAME).append("=").append(String.valueOf(this.getNetworkName())).append("\n");
		
		return str.toString();
		
	}
	
	public mop.hi.svc.model.Network get() {
		return network;
	}

	public void set(mop.hi.svc.model.Network network) {
		this.network = network;
	}

	public int getId() {
		return network.id;
	}

	public void setId(int id) {
//		this.getUpdates().put(Network.ID, String.valueOf(id));
		network.id = id;
	}

	public int getUserId() {
		return network.userId;
	}

	public void setUserId(int userId) {
		this.getUpdates().put(Network.USERID, String.valueOf(userId));
		network.userId = userId;
	}

	public int getStage() {
		return network.stage;
	}

	public void setStage(int stage) {
		this.getUpdates().put(Network.STAGE, String.valueOf(stage));
		network.stage = stage;
	}

	public int getInfoId() {
		return network.infoId;
	}

	public void setInfoId(int infoId) {
		this.getUpdates().put(Network.INFO_ID, String.valueOf(infoId));
		network.infoId = infoId;
	}

	public Date getJoinTime() {
		return DateFormatter.parse(network.joinTime);
	}

	public void setJoinTime(Date joinTime) {
		String date = DateFormatter.format(joinTime);
		this.getUpdates().put(Network.JOIN_TIME, date);
		network.joinTime = date;

	}

	public int getStatus() {
		return network.status;
	}

	public void setStatus(int status) {
		this.getUpdates().put(Network.STATUS, String.valueOf(status));
		network.status = status;
	}

	public int getNetworkId() {
		return network.networkId;
	}

	public void setNetworkId(int networkId) {
		this.getUpdates().put(Network.NETWORK_ID, String.valueOf(networkId));
		network.networkId = networkId;
	}

	public String getNetworkName() {
		return network.networkName;
	}

	public void setNetworkName(String name) {
		this.getUpdates().put(Network.NETWORK_NAME, name);
		network.networkName = name;
	}
}
