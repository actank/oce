package engine.index;

import java.io.File;
import java.io.IOException;
import java.io.RandomAccessFile;

public class SimpleFSIndexOutput extends BufferedIndexOutput {
	RandomAccessFile file = null;

	// remember if the file is open, so that we don't try to close it
	// more than once
	private volatile boolean isOpen;

	public SimpleFSIndexOutput(File path) throws IOException {
		file = new RandomAccessFile(path, "rw");
		isOpen = true;
	}

	/** output methods: */
	@Override
	public void flushBuffer(byte[] b, int offset, int size) throws IOException {
		file.write(b, offset, size);
	}

	@Override
	public void close() throws IOException {
		// only close the file if it has not been closed yet
		if (isOpen) {
			boolean success = false;
			try {
				super.close();
				success = true;
			} finally {
				isOpen = false;
				if (!success) {
					try {
						file.close();
					} catch (Throwable t) {
						// Suppress so we don't mask original exception
					}
				} else
					file.close();
			}
		}
	}

	/** Random-access methods */
	@Override
	public void seek(long pos) throws IOException {
		super.seek(pos);
		file.seek(pos);
	}

	@Override
	public long length() throws IOException {
		return file.length();
	}

	@Override
	public void setLength(long length) throws IOException {
		file.setLength(length);
	}
}