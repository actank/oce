package com.xiaonei.xcetest.tester;

import java.util.Date;
import java.util.List;

import junit.framework.TestCase;
import mop.hi.oce.adapter.AdapterFactory;
import mop.hi.oce.adapter.FootprintAdapter;
import mop.hi.oce.domain.model.FootprintView;
import mop.hi.svc.fpt.Footprint;
import mop.hi.svc.fpt.FootprintException;

public class FootprintTester extends TestCase {

	public void test1() throws Exception {
		FootprintAdapter adapter = AdapterFactory.getFootprintAdapter();
		// adapter.getAll(1000, 0, 6);
		// adapter.getAll(1000);
		// int start = Math.abs(new Random().nextInt()) % 10000;
		int start = 5000;
		for (int i = start; i < start + 100; i++) {

			Footprint fp = new Footprint();
			fp.host = 200000159;
			fp.guest = i;
			fp.guestHead = "TestHead";
			fp.guestName = "TestName";
			fp.time = System.currentTimeMillis();
			FootprintView footprint = new FootprintView();
			footprint.setFootprint(fp);
			adapter.visitHomepage(footprint);

			try {
				Thread.sleep(1000);
			} catch (Exception e) {
			}
			// ;
			// //System.out.println(adapter.getAll(888, 0, 6).length);
			// try {
			// Thread.sleep(500);
			// } catch (Exception e) {
			// };
			// //Footprint[] a = adapter.getAll(888, 0, 6);
			// for(int j=0;j<a.length;j++){
			// System.out.print(a[j].id);
			// System.out.print("---");
			// }
			// System.out.print("\n");
			// System.out.println(adapter.getSize(888));
			// System.out.println("-----------------");

			try {
				Thread.sleep(100);
			} catch (Exception e) {
			}
			;
			//			
		}
		List<FootprintView> footprints = adapter.getAll(200000159, 0, -1);

		for (FootprintView view : footprints) {
			System.out.println(view);
		}
		System.out.println("\nTotal: " + footprints.size());
	}

	public void testSingle() throws Exception {
		int host = 200312372;
		int guest = 200353733;
		FootprintAdapter adapter = AdapterFactory.getFootprintAdapter();
		Footprint fp = new Footprint();
		fp.host = host;
		fp.guest = guest;
		fp.guestHead = "TestHead";
		fp.guestName = "TestName";
		fp.time = System.currentTimeMillis();
		FootprintView footprint = new FootprintView();
		footprint.setFootprint(fp);
		adapter.visitHomepage(footprint);
		AdapterFactory.getBlogFootprintAdapter().visitHomepage(footprint);
		System.out.println(AdapterFactory.getBlogFootprintAdapter().getAll(
				200312372));
	}

	public void test2() {
		FootprintAdapter adapter = AdapterFactory.getTribeFootprintAdapter();

		FootprintView fpt = new FootprintView();
		fpt.setGuest(1219);
		fpt.setGuestName("ww");
		fpt.setGuestUrl("wwwwww");
		fpt.setHost(2);
		fpt.setTime(new Date());
		Footprint ft = new Footprint();

		adapter.visitHomepage(fpt);
		try {
			System.out.print(adapter.getAll(30, 0, 6).size());
		} catch (FootprintException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
	}
	
	public void testGet() throws FootprintException{
		int hostId = 200000780;
		List<FootprintView> fs = AdapterFactory.getFootprintAdapter().getAll(hostId);
		for(FootprintView fv:fs){
			System.out.println(fv.toString());
		}
	}

	public void testTribe() throws FootprintException{
		int tribeId = 200190004;
		List<FootprintView> fs = AdapterFactory.getTribeFootprintAdapter().getAll(tribeId);
		for(FootprintView fv:fs){
			System.out.println(fv.toString());
		}
	}
}
