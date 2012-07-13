package com.xiaonei.xce.init;

public class Init {
  /**
   * Initialize oce-adapter related Ice connections.
   * 
   * @return number of success initialized services.
   */
  
  private static String[] SERVICES = new String[]{
    
  };
  
  public int init() {
    
    return 0;
  }

  /**
   * @param args
   */
  public static void main(String[] args) {
    Init obj = new Init();
    System.out.println(System.currentTimeMillis() + " Starting.");

    System.out.println(System.currentTimeMillis() + " end of initializing");
  }

}
