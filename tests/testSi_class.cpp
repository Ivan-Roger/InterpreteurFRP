/*
 * File:   testSi_class.cpp
 * Author: rogeri
 *
 * Created on 21 oct. 2015, 15:37:34
 */

#include "testSi_class.h"


CPPUNIT_TEST_SUITE_REGISTRATION(testSi_class);

testSi_class::testSi_class() {
}

testSi_class::~testSi_class() {
}

void testSi_class::setUp() {
}

void testSi_class::tearDown() {
}

void testSi_class::testMethod() {
    ifstream inFile("TestSi.frp"); // Ouverture du fichier
    Interpreteur reader(inFile); // Instanciation de l'Interpreteur
    CPPUNIT_ASSERT_NO_THROW_MESSAGE("Analyse syntaxe",reader.analyse()); // Verification de la syntaxe
    CPPUNIT_ASSERT_NO_THROW_MESSAGE("Execution",reader.getArbre()->executer()); // Verification de l'execution
    CPPUNIT_ASSERT_EQUAL_MESSAGE("Test Si 1 (test)",5,reader.getTable()[7].getValeur()); // Valeur de test après execution = 5
    CPPUNIT_ASSERT_EQUAL_MESSAGE("Test Si 2 (test1)",1,reader.getTable()[8].getValeur()); // Valeur de test1 après execution = 1
    CPPUNIT_ASSERT_EQUAL_MESSAGE("Test Si 3 (test2)",2,reader.getTable()[9].getValeur()); // Valeur de test2 après execution = 2
    CPPUNIT_ASSERT_EQUAL_MESSAGE("Test Si 4 (test3)",3,reader.getTable()[10].getValeur()); // Valeur de test3 après execution = 3
}

void testSi_class::testFailedMethod() {
    //nothing
}

