package xce.distcache.tools;

import xce.distcache.VersionAnnotation;

public class Version {

    public static String getVersion() {
	Package pkg = Version.class.getPackage();
	VersionAnnotation version = pkg.getAnnotation(VersionAnnotation.class);
	StringBuffer buff = new StringBuffer();
	buff.append("Revision: ").append(version.revision());
	buff.append("Build: ").append(version.build());
	buff.append("Date: ").append(version.date());
	buff.append("User: ").append(version.user());
	return buff.toString();
    }

    public static void main(String[] args) {
	System.out.println(Version.getVersion());
    }
}
