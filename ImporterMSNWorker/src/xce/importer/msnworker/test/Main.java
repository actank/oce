package xce.importer.msnworker.test;

import xce.importer.AMI_ImporterMSNWorker_contacts;
import xce.importer.ImporterMSNWorkerPrx;
import xce.importer.ImporterMSNWorkerPrxHelper;
import xce.util.channel.Channel;
import Ice.LocalException;

import com.live.schema.TLiveContacts;

public class Main {

	/**
	 * @param args
	 */
	public static void main(String[] args) {
		Ice.Communicator ic = Channel.newOceChannel("MSNWorkerTest").getCommunicator();
		ImporterMSNWorkerPrx prx = ImporterMSNWorkerPrxHelper.uncheckedCast(ic.stringToProxy("M@ImporterMSNWorker"));
		System.out.println(System.currentTimeMillis());
		for (int x = 0; x < 1; ++x) {
			prx.contacts_async(new AMI_ImporterMSNWorker_contacts() {

				@Override
				public void ice_exception(LocalException ex) {
					System.out.println(System.currentTimeMillis());
					ex.printStackTrace();
				}

				@Override
				public void ice_response(TLiveContacts result) {
					System.out.println(System.currentTimeMillis() + " " + result.Contacts.length);
				}

			}, "jingmo@hotmail.com", "passghl123");
		}
		;
	}
}
