package com.sensei.search.req.protobuf;
import java.io.IOException;
import java.util.Arrays;

import org.apache.lucene.store.DataOutput;
import org.apache.lucene.util.BytesRef;

public class ByteDataOutput extends DataOutput {
	static final int BUFFER_SIZE = 16384;

	public byte[] bytes;
	public int pos;

	public ByteDataOutput() {
		pos = 0;
		bytes = new byte[BUFFER_SIZE];
	}

	public byte[] toBytes() {
		return Arrays.copyOfRange(bytes, 0, pos);
	}

	@Override
	public void writeByte(byte b) {
		if (pos < bytes.length) {
			bytes[pos++] = b;
		} else {
			byte[] newBuf = new byte[bytes.length << 1];
			System.arraycopy(bytes, 0, newBuf, 0, bytes.length);
			bytes = newBuf;
			bytes[pos++] = b;
		}
	}
	

	@Override
	public void writeBytes(byte[] b, int offset, int length) {
	}
}
