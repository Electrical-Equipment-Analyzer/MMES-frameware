/*
 * Test.h
 *
 *  Created on: 2016/2/25
 *      Author: D10307009
 */

#ifndef TEST_H_
#define TEST_H_

#include "Menu.h"

class Test {
public:

	Test(Menu *parent);

	Menu *getMenu();

//	void test();
//	void test_eth();

private:

	Menu m_menu;

};

#endif /* TEST_H_ */
