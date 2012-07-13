package com.xiaonei.xce.cardfacade;

import xce.cardcache.ImportInfo;
import xce.cardcache.ImportInfoNew;

public class ImportDetailInfo {

	public ImportDetailInfo() {

	}

	public ImportDetailInfo(ImportInfo info) {
		this.userId = info.userId;
		this.importWay = info.importWay;
		this.importDetail = info.importDetail;
		this.msn = info.msn;
		this.email = info.email;
		this.name = info.name;
		this.mobile = info.mobile;
		this.qq = info.qq;
		this.address = info.address;
		this.extra = info.extra;
		this.requestCount = info.requestCount;
	}

	public ImportDetailInfo(ImportInfoNew info) {
		this.id = info.id;
		this.userId = info.userId;
		this.importWay = info.importWay;
		this.importDetail = info.importDetail;
		this.msn = info.msn;
		this.email = info.email;
		this.name = info.name;
		this.mobile = info.mobile;
		this.qq = info.qq;
		this.address = info.address;
		this.extra = info.extra;
		this.requestCount = info.requestCount;
		this.subscribeFlag = info.subscribeFlag;
	}

	public ImportInfo getImportInfo() {
		ImportInfo info = new ImportInfo();
		info.userId = this.userId;
		info.importWay = this.importWay;
		info.importDetail = this.importDetail;
		info.msn = this.msn;
		info.email = this.email;
		info.name = this.name;
		info.mobile = this.mobile;
		info.qq = this.qq;
		info.address = this.address;
		info.extra = this.extra;
		info.requestCount = this.requestCount;
		return info;
	}

	public ImportInfoNew getImportInfoNew() {
		ImportInfoNew info = new ImportInfoNew();
		info.id = this.id;
		info.userId = this.userId;
		info.importWay = this.importWay;
		info.importDetail = this.importDetail;
		info.msn = this.msn;
		info.email = this.email;
		info.name = this.name;
		info.mobile = this.mobile;
		info.qq = this.qq;
		info.address = this.address;
		info.extra = this.extra;
		info.requestCount = this.requestCount;
		info.subscribeFlag = this.subscribeFlag;
		return info;
	}

	public int getUserId() {
		return userId;
	}

	public void setUserId(int userId) {
		this.userId = userId;
	}

	public String getMobile() {
		return mobile;
	}

	public void setMobile(String mobile) {
		this.mobile = mobile;
	}

	public String getName() {
		return name;
	}

	public void setName(String name) {
		this.name = name;
	}

	public String getQq() {
		return qq;
	}

	public void setQq(String qq) {
		this.qq = qq;
	}

	public String getAddress() {
		return address;
	}

	public void setAddress(String address) {
		this.address = address;
	}

	public String getExtra() {
		return extra;
	}

	public void setExtra(String extra) {
		this.extra = extra;
	}

	public void setRequestCount(int requestCount) {
		this.requestCount = requestCount;
	}

	public int getRequestCount() {
		return requestCount;
	}

	public void setImportWay(byte importWay) {
		this.importWay = importWay;
	}

	public byte getImportWay() {
		return importWay;
	}

	public void setImportDetail(String importDetail) {
		this.importDetail = importDetail;
	}

	public String getImportDetail() {
		return importDetail;
	}

	public void setMsn(String msn) {
		this.msn = msn;
	}

	public String getMsn() {
		return msn;
	}

	public void setEmail(String email) {
		this.email = email;
	}

	public String getEmail() {
		return email;
	}

	private int id;
	private int userId;
	private byte importWay;
	private String importDetail;
	private String msn;
	private String email;
	private String name;
	private String mobile;
	private String qq;
	private String address;
	private String extra;
	private int requestCount;
	private byte subscribeFlag;

	@Override
	public String toString() {
		return "ImportDetailInfo [userId=" + userId + ", importWay="
				+ importWay + ", importDetail=" + importDetail + ", msn=" + msn
				+ ", email=" + email + ", name=" + name + ", mobile=" + mobile
				+ ", qq=" + qq + ", address=" + address + ", extra=" + extra
				+ ", requestCount=" + requestCount + "]";
	}

	public String toStringNew() {
		return "ImportDetailInfo [id=" + id + ", userId=" + userId
				+ ", importWay=" + importWay + ", importDetail=" + importDetail
				+ ", msn=" + msn + ", email=" + email + ", name=" + name
				+ ", mobile=" + mobile + ", qq=" + qq + ", address=" + address
				+ ", extra=" + extra + ", requestCount=" + requestCount
				+ ",subscribeFlag" + subscribeFlag + "]";
	}

	public void setId(int id) {
		this.id = id;
	}

	public int getId() {
		return id;
	}

	public void setSubscribeFlag(byte subscribeFlag) {
		this.subscribeFlag = subscribeFlag;
	}

	public byte getSubscribeFlag() {
		return subscribeFlag;
	}

}
