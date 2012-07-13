package xce.importer.msnworker.test;

import java.util.ArrayList;
import java.util.List;

import net.sf.jml.MsnContact;
import net.sf.jml.MsnContactList;
import net.sf.jml.MsnGroup;
import net.sf.jml.MsnMessenger;
import net.sf.jml.event.MsnContactListListener;
import net.sf.jml.impl.MsnMessengerFactory;

import com.live.schema.TContact;
import com.live.schema.TEmail;
import com.live.schema.TLiveContacts;
import com.live.schema.TOwner;
import com.live.schema.TPersonal;
import com.live.schema.TProfessional;
import com.live.schema.TProfiles;
import com.live.schema.TTag;

public class Bot {

	/**
	 * @param args
	 */
	public static void main(String[] args) {
		MsnMessenger messenger = MsnMessengerFactory.createMsnMessenger("xiaonei-test@hotmail.com", "tester!");

		messenger.setLogIncoming(false);
		messenger.setLogOutgoing(false);
		messenger.addContactListListener(new MsnContactListListener() {

			@Override
			public void contactAddCompleted(MsnMessenger arg0, MsnContact arg1) {
			}

			@Override
			public void contactAddedMe(MsnMessenger arg0, MsnContact arg1) {
			}

			@Override
			public void contactListInitCompleted(MsnMessenger messenger) {
				System.out.println("contactListInitCompleted");
				List<TContact> contacts = new ArrayList<TContact>();
				MsnContactList list = messenger.getContactList();
				System.out.println("contactListInitCompleted " + list.getContacts().length);
				for (MsnContact c : list.getContacts()) {
					System.out.println("contactListInitCompleted " + c.getEmail().getEmailAddress());
					TContact contact = new TContact();
					contact.Profiles = new TProfiles(new TPersonal(), new TProfessional());
					contact.Profiles.Personal.DisplayName = c.getDisplayName();
					contact.Profiles.Personal.NickName = c.getFriendlyName();
					contact.Emails = new TEmail[] { new TEmail("", "", c.getEmail().getEmailAddress(), "", "") };
					contacts.add(contact);

				}

				System.out.println("contactListInitCompleted  owner");
				TOwner owner = new TOwner();
				owner.Emails = new TEmail[] { new TEmail("", "", messenger.getOwner().getEmail().getEmailAddress(), "", "") };
				owner.Profiles = new TProfiles(new TPersonal(), new TProfessional());
				owner.Profiles.Personal.DisplayName = messenger.getOwner().getDisplayName();
				owner.Profiles.Personal.NickName = messenger.getOwner().getPersonalMessage();
				TLiveContacts result = new TLiveContacts(owner, contacts.toArray(new TContact[contacts.size()]), new TTag[] {});
				System.out.println("contactListInitCompleted A");
				System.out.println("contactListInitCompleted B" + result.Contacts.length);
				messenger.logout();
				System.out.println("contactListInitCompleted C");
			}

			@Override
			public void contactListSyncCompleted(MsnMessenger messenger) {
				System.out.println("contactListSyncCompleted");
			}

			@Override
			public void contactRemoveCompleted(MsnMessenger arg0, MsnContact arg1) {
			}

			@Override
			public void contactRemovedMe(MsnMessenger arg0, MsnContact arg1) {
			}

			@Override
			public void contactStatusChanged(MsnMessenger arg0, MsnContact arg1) {
			}

			@Override
			public void groupAddCompleted(MsnMessenger arg0, MsnGroup arg1) {
			}

			@Override
			public void groupRemoveCompleted(MsnMessenger arg0, MsnGroup arg1) {
			}

			@Override
			public void ownerStatusChanged(MsnMessenger arg0) {
			}
		});

		try {
			messenger.login();
		} catch (Exception e) {
			e.printStackTrace();
		}
		System.out.println("Login done.");
	}

}
