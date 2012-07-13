package engine.search;

import java.io.File;
import java.io.IOException;
import java.io.RandomAccessFile;
import java.lang.reflect.Method;
import java.nio.BufferUnderflowException;
import java.nio.ByteBuffer;
import java.nio.channels.FileChannel.MapMode;
import java.security.AccessController;
import java.security.PrivilegedActionException;
import java.security.PrivilegedExceptionAction;

import org.apache.lucene.store.IndexInput;

public class MMapIndexInput extends IndexInput {

	private ByteBuffer buffer;
	private long length;
	RandomAccessFile file = null;
	private volatile boolean isOpen;
	private boolean isClone = false;

	public MMapIndexInput(File path) throws IOException {
		file = new RandomAccessFile(path, "r");
		this.length = file.length();
		this.buffer = file.getChannel().map(MapMode.READ_ONLY, 0, length)
				.load();
		isOpen = true;
	}

	@Override
	public byte readByte() throws IOException {
		try {
			return buffer.get();
		} catch (BufferUnderflowException e) {
			throw new IOException("read past EOF");
		}
	}

	@Override
	public void readBytes(byte[] b, int offset, int len) throws IOException {
		try {
			buffer.get(b, offset, len);
		} catch (BufferUnderflowException e) {
			throw new IOException("read past EOF");
		}
	}

	@Override
	public long getFilePointer() {
		return buffer.position();
	}

	@Override
	public void seek(long pos) throws IOException {
		buffer.position((int) pos);
	}

	@Override
	public long length() {
		return length;
	}

	final void cleanMapping(final ByteBuffer buffer) throws IOException {
		try {
			AccessController
					.doPrivileged(new PrivilegedExceptionAction<Object>() {
						public Object run() throws Exception {
							final Method getCleanerMethod = buffer.getClass()
									.getMethod("cleaner");
							getCleanerMethod.setAccessible(true);
							final Object cleaner = getCleanerMethod
									.invoke(buffer);
							if (cleaner != null) {
								cleaner.getClass().getMethod("clean")
										.invoke(cleaner);
							}
							return null;
						}
					});
		} catch (PrivilegedActionException e) {
			final IOException ioe = new IOException(
					"unable to unmap the mapped buffer");
			ioe.initCause(e.getCause());
			throw ioe;
		}
	}

	@Override
	public Object clone() {
		if (buffer == null)
			try {
				throw new Exception("MMapIndexInput already closed");
			} catch (Exception e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
		MMapIndexInput clone = (MMapIndexInput) super.clone();
		clone.isClone = true;
		clone.buffer = buffer.duplicate();
		return clone;
	}

	@Override
	public void close() throws IOException {
		try {
			if (isClone || buffer == null)
				return;
			cleanMapping(buffer);
		} finally {
			buffer = null;
		}
	}
}
