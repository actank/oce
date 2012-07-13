#ifndef __ONLINEBITMAP_ICE__
#define __ONLINEBITMAP_ICE__
module talk
{
    module AddressBook
    {
        class AddreeBook
        {
            string getAddressBook(string name);
            bool   addAddressBook(string name, string email);
            bool   updateAddressBook(string name, string email);
            bool   deleteAddressBook(string name); 
        };
    };
};
#endif
