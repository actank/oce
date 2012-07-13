package xce.distcache.test;

import java.io.UnsupportedEncodingException;

import xce.distcache.DistCacheAdapter;

public class CommonOperations {
  public static void main(String[] args) throws UnsupportedEncodingException {
    DistCacheAdapter adapter = DistCacheAdapter.connect("UserNames");

    adapter.set(202911262L, "NAMESNAMES".getBytes("UTF-8"));

    byte[] ret = adapter.get(202911262L);
    if (ret == null) {
      System.out.println("Got : null");
    } else {
      System.out.println("Got : " + new String(ret, "UTF-8") + " length: "
          + ret.length);
    }
  }
}
