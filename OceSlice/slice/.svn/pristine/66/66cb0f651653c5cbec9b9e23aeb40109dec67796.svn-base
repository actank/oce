#ifndef __WINDOWS_LIVE_SCHEMA_ICE__
#define __WINDOWS_LIVE_SCHEMA_ICE__

module com
{
module live
{
module schema
{
	// Types for TOwner and TContacts
	struct TPersonal
	{
		string NameToFileAs;
		string NameTitle;
		string FirstName;
		string MiddleName;
		string LastName;
		string Suffix;
		string YomiFirstName;
		string YomiLastName;
		string UniqueName;
		string SortName;
		string NickName;
		string DisplayName;
		string Birthdate;
		string Anniversary;
		string Gender;
		string TimeZone;
		string SpouseName;
	};
	struct TProfessional
	{
		string JobTitle;
		string Profession;
		string Manager;
		string Assistant;
	};
	struct TProfiles
	{
		TPersonal Personal;
		TProfessional Professional;
	};

	struct TEmail
	{
		string ID;
		string EmailType;
		string Address;
		string IsIMEnabled;
		string IsDefault;
	};
	sequence<TEmail> TEmails;

	struct TPhone
	{
		string ID;
		string PhoneType;
		string Number;
		string IsIMEnabled;
		string IsDefault;
	};
	sequence<TPhone> TPhones;

	struct TLocation
	{
		string ID;
		string LocationType;
		string Office;
		string Department;
		string CompanyName;
		string YomiCompanyName;
		string StreetLine;
		string StreetLine2;
		string PrimaryCity;
		string SecondaryCity;
		string SubDivision;
		string PostalCode;
		string CountryRegion;
		string Latitude;
		string Longitude;
		string IsDefault;
	};
	sequence<TLocation> TLocations;

	struct TURI
	{
		string ID;
		string URIType;
		string Name;
		string Address;
	};
	sequence<TURI> TURIs;

	// Types for TTags
	struct TContactID
	{
		string ContactID;
	};
	sequence<TContactID> TContactIDs;

	// Level 1
	struct TOwner
	{
		string CID;
		string WindowsLiveID;
		TProfiles Profiles;
		TEmails Emails;
		TPhones Phones;
		TLocations Locations;
		TURIs URIs;
	};

	struct TContact
	{
		string ID;
		string CID;
		string WindowsLiveID;
		string AutoUpdateEnabled;
		string AutoUpdateStatus;
		TProfiles Profiles;
		TEmails Emails;
		TPhones Phones;
		TLocations Locations;
		TURIs URIs;
	};
	sequence<TContact> TContacts;

	struct TTag
	{
		string ID;
		string Name;
		TContactIDs ContactIDs;
	};
	sequence<TTag> TTags;

	// top level
	struct TLiveContacts
	{
		TOwner Owner;
		TContacts Contacts;
		TTags Tags;
	};
};
};
};
#endif
