#include "CharityAdapter.h"
#include "Date.h"
using namespace xce::charity;

CharityManagerPrx CharityAdapter::getCharityManager(int id)
{
	return locate<CharityManagerPrx>(_managers, "M", id, TWO_WAY);
}

CharityManagerPrx CharityAdapter::getCharityManagerOneway(int id)
{
	return locate<CharityManagerPrx>(_managersOneway, "M", id, ONE_WAY);
}

void CharityAdapter::set(int id, bool flag)
{
        getCharityManager(id)->set(id, flag);
}
