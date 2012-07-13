/*
 * AdTwSAdapter.cpp
 *
 *  Created on: 2010-9-14
 *      Author: ark
 */

#include "AdTwSAdapter.h"


AdMatchManagerPrx AdTwAdapter::getManager(int id){
	return locate<AdMatchManagerPrx> (_managers, "M", id, TWO_WAY, 500);
}

AdMatchManagerPrx AdTwAdapter::getManagerOneway(int id) {
	return locate<AdMatchManagerPrx> (_managersOneway, "M", id, ONE_WAY, 500);
}



