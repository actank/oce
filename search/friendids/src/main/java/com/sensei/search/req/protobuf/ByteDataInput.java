package com.sensei.search.req.protobuf;

import java.io.IOException;

import org.apache.lucene.store.DataInput;

public class ByteDataInput extends DataInput {

	private byte[] bytes;

	private int pos;

	public ByteDataInput(byte[] bytes) {
		pos = 0;
		this.bytes = bytes;
	}

	@Override
	public byte readByte() throws IOException {
		assert (pos < bytes.length);
		return bytes[pos++];
	}

	@Override
	public void readBytes(byte[] b, int offset, int len) throws IOException {
	}

}
