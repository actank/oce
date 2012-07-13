/*
 * AdTwAdapter.cpp
 *
 *  Created on: 2010-9-14
 *      Author: ark
 */

#include "AdTwAdapter.h"


AdMatchPrx AdTwAdapter::getManager(int id){
	return locate<AdMatchPrx> (_managers, "M", id, TWO_WAY);
}

AdMatchPrx AdTwAdapter::getManagerOneway(int id) {
	return locate<AdMatchPrx> (_managersOneway, "M", id, ONE_WAY);
}



