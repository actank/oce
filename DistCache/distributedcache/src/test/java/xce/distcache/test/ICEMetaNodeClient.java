package xce.distcache.test;

import java.util.List;

import xce.distcache.DistributedCacheSchema.NodeTypeEnum;
import xce.distcache.node.MetaNode;
import xce.distcache.node.NodeFactory;
import xce.distcache.node.Node.Item;

public class ICEMetaNodeClient {

  /**
   * @param args
   */
  public static void main(String[] args) {
    MetaNode x = NodeFactory.createMetaNode(NodeTypeEnum.ICE, "DUMMY",
        "M:tcp -h localhost -p 12345");
    final int cluster = 9997;
    for (long u = 0; u < 10000; ++u) {
      x.alloc(new Item((int) (u % cluster), u));
    }

    for (int u = 0; u < 100; ++u) {
      List<Long> ret = x.get(u);
      System.out.print(u + "\t");
      for (Long uu : ret) {
        System.out.print(uu + " ");
      }
      System.out.println();
    }
  }
}
