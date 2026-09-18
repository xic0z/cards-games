#include "CUnit/Basic.h"
#include "serie.h"
#include <stdio.h>

void testar_fact() {
  CU_ASSERT_EQUAL(fact(0), 1);
  CU_ASSERT_EQUAL(fact(1), 1);
  CU_ASSERT_EQUAL(fact(5), 120);
  CU_ASSERT_EQUAL(fact(7), 5040);
}

void testar_pot() {
  CU_ASSERT_DOUBLE_EQUAL(pot(2, 0), 1, 0.5e-9);
  CU_ASSERT_DOUBLE_EQUAL(pot(2, 2), 4, 0.5e-9);
  CU_ASSERT_DOUBLE_EQUAL(pot(7, 3), 343, 0.5e-9);
  CU_ASSERT_DOUBLE_EQUAL(pot(0.1, 7), 1e-7, 0.5e-9);
  CU_ASSERT_DOUBLE_EQUAL(pot(0.5, 10), 0.0009765625, 0.5e-9);
  CU_ASSERT_DOUBLE_EQUAL(pot(0.79, 6), 0.2430874555, 0.5e-9);
}

void testar_serie() {
  CU_ASSERT_DOUBLE_EQUAL(serie(-0.2, 10), -0.2013579, 0.5e-5);
  CU_ASSERT_DOUBLE_EQUAL(serie(0.5, 10), 0.5236, 0.5e-3);
  CU_ASSERT_DOUBLE_EQUAL(serie(0.70710678, 10), 0.7854, 0.5e-3);
  CU_ASSERT_DOUBLE_EQUAL(serie(-0.9, 10), -1.1198, 0.5e-1);
}

int main() {
  // Initialize the CUnit test registry
  if (CUE_SUCCESS != CU_initialize_registry())
    return CU_get_error(); // Sets the basic run mode, CU_BRM_VERBOSE will show
                           // maximum output of run details
  // Other choices are: CU_BRM_SILENT and CU_BRM_NORMAL
  CU_basic_set_mode(CU_BRM_VERBOSE); // Run the tests and show the run summary

  CU_pSuite pSuite = NULL;

  // Add a suite to the registry
  pSuite = CU_add_suite("sum_test_suite", 0,
                        0); // Always check if add was successful
  if (NULL == pSuite) {
    CU_cleanup_registry();
    return CU_get_error();
  }

  // Add the test to the suite
  if (NULL == CU_add_test(pSuite, "testar_fact", testar_fact)) {
    CU_cleanup_registry();
    return CU_get_error();
  }
  if (NULL == CU_add_test(pSuite, "testar_pot", testar_pot)) {
    CU_cleanup_registry();
    return CU_get_error();
  }
  if (NULL == CU_add_test(pSuite, "testar_serie", testar_serie)) {
    CU_cleanup_registry();
    return CU_get_error();
  }


  CU_basic_run_tests();
  return CU_get_error();
}
