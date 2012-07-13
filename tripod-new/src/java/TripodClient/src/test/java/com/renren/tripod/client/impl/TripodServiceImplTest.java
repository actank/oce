package com.renren.tripod.client.impl;

import java.util.ArrayList;
import java.util.List;
import java.util.Map;
import java.util.Map.Entry;

import org.junit.Test;

import com.renren.tripod.client.MockTripodClient;
import com.renren.tripod.client.model.AddressBookProtos.Person;

/**
 * @author Li Weibo (weibo.leo@gmail.com) //I believe spring-brother
 * @since 2012-1-4 下午04:32:52
 */
public class TripodServiceImplTest {
    
    @Test
    public void test() {
        MockTripodClient tripodClient = new MockTripodClient();
        
        TripodServiceImpl<Person> tripodService = new TripodServiceImpl<Person>();
        tripodService.setTripodClient(tripodClient);
        tripodService.setDataClass(Person.class);
        
        List<String> keys = new ArrayList<String>();
        keys.add("John Doe");
        keys.add("James");
        keys.add("Joe");
        Map<String, Person> res = tripodService.get(keys, null);
        for(Entry<String, Person> entry : res.entrySet()) {
            System.out.println(entry);
        }
    }
    
}
