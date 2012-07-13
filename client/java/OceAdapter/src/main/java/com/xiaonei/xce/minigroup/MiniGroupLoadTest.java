package com.xiaonei.xce.minigroup;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.IOException;
import java.util.ArrayList;
import java.util.List;

public class MiniGroupLoadTest {
  public static class MiniGroupRequest implements Runnable {
    private List<Integer> userIds = new ArrayList<Integer>();
    private int num = 0;
    
    public MiniGroupRequest(int num, final List<Integer> userIds) {
      this.num = num;
      if (userIds != null) {
        this.userIds.addAll(userIds);
      }
    }
    
    public void run() {
      long startTime = System.currentTimeMillis();
      int size = 0;
      for (Integer userId : userIds) {
        long start = System.currentTimeMillis();
        List<MiniGroupInfo> miniGroupInfos =
          MiniGroupAdapter.getInstance().getJoinedMiniGroups(userId);
        size += miniGroupInfos.size();
        //System.out.println("Get MiniGroupInfos of userId = " + userId + " cost "
        //   + (System.currentTimeMillis() - start) + " ms, result.size = " + miniGroupInfos.size());
      }
      System.out.println("Processed " + userIds.size() + " request cost " 
          + (System.currentTimeMillis() - startTime) + " ms in thread " + num
          + " average time = " + (System.currentTimeMillis() - startTime) / userIds.size() + " ms,"
          + " average size of MiniGroupInfos is " + size / userIds.size());
    }
  }
  public static void main(String args[]) {
    BufferedReader reader = null;
    try {
      File inputFile = new File("/home/shangmin/test/data/urls");
      reader = new BufferedReader(new FileReader(inputFile));
    } catch (FileNotFoundException e) {
      e.printStackTrace();
    }
    if (reader != null) {
      try {
        int maxThreads = 500, maxRequestPerThread = 160;
        if (args.length == 2) {
          maxThreads = Integer.valueOf(args[0]);
          maxRequestPerThread = Integer.valueOf(args[1]);
        }
        List<ArrayList<Integer>> userIds = new ArrayList<ArrayList<Integer>>();
        List<Integer> tempUserIds = new ArrayList<Integer>();
        String line = null;
        while ( userIds.size() < maxThreads && (line = reader.readLine()) != null) {
          if (tempUserIds.size() == maxRequestPerThread) {
            userIds.add(new ArrayList<Integer>(tempUserIds));
            tempUserIds.clear();
          }
          int tempUserId = Integer.valueOf(line.split("userId=")[1]);
          tempUserIds.add(tempUserId);
        }
        for (int i = 0; i < userIds.size(); ++i) {
          MiniGroupRequest request = new MiniGroupRequest(i, userIds.get(i));
          Thread thread = new Thread(request);
          //thread.setDaemon(true);
          thread.start();
        }
        reader.close();
      } catch (IOException e) {
        e.printStackTrace();
      }
    }
  }
}
