package com.renren.messageconfig;

import java.util.List;

public abstract class NamespaceListener {
    private String namespace = "";

    public NamespaceListener(String namespace) {
	if (namespace != null) {
	    this.namespace = namespace;
	}
    }

    public String getNamespace() {
	return namespace;
    }

    public abstract boolean update(List<String> childrenNameList);
}
