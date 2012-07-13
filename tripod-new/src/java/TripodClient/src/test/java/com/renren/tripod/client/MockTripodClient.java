package com.renren.tripod.client;

import java.io.ByteArrayOutputStream;
import java.io.IOException;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

import com.renren.tripod.client.model.AddressBookProtos.Person;


public class MockTripodClient implements ITripodClient{

    public MockTripodClient() {
        
    } 
    
    @Override
    public Map<String, byte[]> get(List<String> keys, List<String> missedKeys) {
        Map<String, byte[]> ret = new HashMap<String, byte[]>();
        for (String key : keys) {
            Person p = getPerson(key);
            ByteArrayOutputStream baos = new ByteArrayOutputStream();
            try {
                p.writeTo(baos);
            } catch (IOException e) {
                e.printStackTrace();
            }
            ret.put(key, baos.toByteArray());
        }
        return ret;
    }

    private Person getPerson(String name) {
        
        Person john =
            Person.newBuilder()
              .setId(1234)
              .setName(name)
              .setEmail("jdoe@example.com")
              .addPhone(
                Person.PhoneNumber.newBuilder()
                  .setNumber("555-4321")
                  .setType(Person.PhoneType.HOME))
              .build();
        return john;
    }

    @Override
    public Map<String, byte[]> getMissed(List<String> keys, int expiredTime, boolean useMaster) {
        // TODO Auto-generated method stub
        return null;
    }

    @Override
    public Map<String, byte[]> getMissed(List<String> keys, int expiredTime, boolean useMaster,
            int timeout) {
        // TODO Auto-generated method stub
        return null;
    }

    @Override
    public Map<String, byte[]> getWithMissed(List<String> keys, int expiredTime, boolean useMaster,
            int timeout) {
        // TODO Auto-generated method stub
        return null;
    }

    @Override
    public boolean deleteCache(List<String> keys) {
        // TODO Auto-generated method stub
        return false;
    }

    @Override
    public boolean deleteCache(List<String> keys, int timeout) {
        // TODO Auto-generated method stub
        return false;
    }
    
}
