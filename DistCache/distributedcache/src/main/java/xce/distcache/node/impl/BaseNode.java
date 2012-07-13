package xce.distcache.node.impl;

import xce.distcache.node.Node;

public abstract class BaseNode implements Node {
    protected String _name;
    protected String _config;

    @Override
    public String getConfig() {
	return _config;
    }

    @Override
    public void initialize(String name, String config) {
	this._name = name;
	this._config = config;
    }

    @Override
    public int compareTo(Node o) {
	return _config.compareTo(o.getConfig());
    }

}
