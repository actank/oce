package xce.util.tools;

public class Version {

  public static String getVersion(String className) {
    StringBuffer buff = new StringBuffer();
    try {
      Class<?> target = Class.forName(className);
      Package pkg = target.getPackage();
      VersionAnnotation version = pkg.getAnnotation(VersionAnnotation.class);
      buff.append("Revision: ").append(version.revision());
      buff.append(" Build: ").append(version.build());
      buff.append(" Date: ").append(version.date());
      buff.append(" User: ").append(version.user());
    } catch (ClassNotFoundException e) {
      buff.append("Class not found: " + className);
    } catch (Throwable e) {
      buff.append("getVersion wrong: " + e.toString() + " " + className);
    }
    return buff.toString();
  }

  public static void main(String[] args) {
    for (String arg : args) {
      System.out.println(getVersion(arg));
    }
  }
}
