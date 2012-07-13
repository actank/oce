package com.xiaonei.search;

import java.io.IOException;

import org.apache.lucene.store.DataOutput;

public class ByteOutput extends DataOutput {

	private long size = 0;

	@Override
	public void writeByte(byte b) throws IOException {
		++size;
	}

	@Override
	public void writeBytes(byte[] b, int offset, int limit) throws IOException {
		if (limit > 0 && offset < b.length) {
			size += offset + limit > b.length ? b.length - offset : limit;
		}

	}

	public long getByteSize() {
		return size;
	}

	public static void main(String[] args) {
		int x = Integer.MAX_VALUE + 1;
		System.out.println(x%10);
	}
}
