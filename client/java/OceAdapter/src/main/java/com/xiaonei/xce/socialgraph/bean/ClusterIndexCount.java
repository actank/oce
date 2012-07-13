package com.xiaonei.xce.socialgraph.bean;

public class ClusterIndexCount implements Comparable<ClusterIndexCount> {

    private int clusterIndex;

    private int clusterCount;

    public ClusterIndexCount(int clusterIndex) {
        this.clusterIndex = clusterIndex;
        this.clusterCount = 1;
    }

    public void increase() {
        ++clusterCount;
    }

    public int getClusterIndex() {
        return clusterIndex;
    }

    public int getClusterCount() {
        return clusterCount;
    }

    /**
     * 降序!
     */
    @Override
    public int compareTo(ClusterIndexCount o) {
        return o.clusterCount - this.clusterCount;
    }

    public String toString() {
        StringBuilder builder = new StringBuilder();
        builder.append("clusterIndex:").append(clusterIndex).append("\t");
        builder.append("clusterCount:").append(clusterCount);
        return builder.toString();
    }
}
