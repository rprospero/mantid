#ifndef AlgorithmManagerTest_H_
#define AlgorithmManagerTest_H_

#include <cxxtest/TestSuite.h>

#include "MantidAPI/Algorithm.h"
#include <stdexcept>

using namespace Mantid::API;

class AlgTest : public Algorithm
{
public:

  AlgTest() : Algorithm() {}
  virtual ~AlgTest() {}
  void init() { }
  void exec() {  }
};

class AlgTestSecond : public Algorithm
{
public:

  AlgTestSecond() : Algorithm() {}
  virtual ~AlgTestSecond() {}
  void init() { }
  void exec() { }
};

DECLARE_ALGORITHM(AlgTest)
DECLARE_ALGORITHM(AlgTestSecond)

class AlgorithmManagerTest : public CxxTest::TestSuite
{
public:

  AlgorithmManagerTest() 
  {

  }

  void testInstance()
  {
    // Not really much to test
    //AlgorithmManager *tester = AlgorithmManager::Instance();
    //TS_ASSERT_EQUALS( manager, tester);
    TS_ASSERT_THROWS_NOTHING( AlgorithmManager::Instance().create("AlgTest") )
    TS_ASSERT_THROWS(AlgorithmManager::Instance().create("aaaaaa"), std::runtime_error )
  }

  void testClear()
  {
    AlgorithmManager::Instance().clear();
    AlgorithmManager::Instance().subscribe<AlgTest>("AlgorithmManager::myAlgclear");
    AlgorithmManager::Instance().subscribe<AlgTestSecond>("AlgorithmManager::myAlgBclear");
    TS_ASSERT_THROWS_NOTHING( AlgorithmManager::Instance().create("AlgorithmManager::myAlgBclear") );
    TS_ASSERT_THROWS_NOTHING(AlgorithmManager::Instance().create("AlgorithmManager::myAlgBclear") );
    TS_ASSERT_EQUALS(AlgorithmManager::Instance().size(),2);
    AlgorithmManager::Instance().clear();
    TS_ASSERT_EQUALS(AlgorithmManager::Instance().size(),0);
  }

  void testReturnType()
  {
    AlgorithmManager::Instance().clear();
    AlgorithmManager::Instance().subscribe<AlgTest>("AlgorithmManager::myAlg");
    AlgorithmManager::Instance().subscribe<AlgTestSecond>("AlgorithmManager::myAlgB");
    Algorithm_sptr alg;
    TS_ASSERT_THROWS_NOTHING( alg = AlgorithmManager::Instance().create("AlgorithmManager::myAlg") );
    TS_ASSERT_DIFFERS(dynamic_cast<AlgTest*>(alg.get()),static_cast<AlgTest*>(0));
    TS_ASSERT_THROWS_NOTHING( alg = AlgorithmManager::Instance().create("AlgorithmManager::myAlgB") );
    TS_ASSERT_DIFFERS(dynamic_cast<AlgTestSecond*>(alg.get()),static_cast<AlgTestSecond*>(0));
    TS_ASSERT_DIFFERS(dynamic_cast<Algorithm*>(alg.get()),static_cast<Algorithm*>(0));
    TS_ASSERT_EQUALS(AlgorithmManager::Instance().size(),2);   // To check that crea is called on local objects
  }

  void testManagedType()
  {
    AlgorithmManager::Instance().clear();
    Algorithm_sptr Aptr, Bptr;
    Aptr=AlgorithmManager::Instance().create("AlgTest");
    Bptr=AlgorithmManager::Instance().createUnmanaged("AlgTest");
    TS_ASSERT_DIFFERS(Aptr,Bptr);
    TS_ASSERT_EQUALS(AlgorithmManager::Instance().size(),1);
    TS_ASSERT_DIFFERS(Aptr.get(),static_cast<Algorithm*>(0));
    TS_ASSERT_DIFFERS(Bptr.get(),static_cast<Algorithm*>(0));
  }

};

#endif /* AlgorithmManagerTest_H_*/
