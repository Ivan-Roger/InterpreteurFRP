/*
 * File:   testSi_class.h
 * Author: rogeri
 *
 * Created on 21 oct. 2015, 15:37:33
 */

#ifndef TESTSI_CLASS_H
#define	TESTSI_CLASS_H

#include <cppunit/extensions/HelperMacros.h>
#include "../Interpreteur.h"

class testSi_class : public CPPUNIT_NS::TestFixture {
    CPPUNIT_TEST_SUITE(testSi_class);

    CPPUNIT_TEST(testMethod);
    CPPUNIT_TEST(testFailedMethod);

    CPPUNIT_TEST_SUITE_END();

public:
    testSi_class();
    virtual ~testSi_class();
    void setUp();
    void tearDown();

private:
    void testMethod();
    void testFailedMethod();
};

#endif	/* TESTSI_CLASS_H */

