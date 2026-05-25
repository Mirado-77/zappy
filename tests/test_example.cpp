/*
** EPITECH PROJECT, 2025
** C++ Project Template
** File description:
** Example unit tests with Criterion
*/

#include <criterion/criterion.h>
#include <criterion/redirect.h>

// Example test suite
Test(example_suite, test_true)
{
    cr_assert(true, "This test should always pass");
}

Test(example_suite, test_equal)
{
    int a = 5;
    int b = 5;

    cr_assert_eq(a, b, "Expected %d but got %d", a, b);
}

Test(example_suite, test_not_null)
{
    int value = 42;
    int *ptr = &value;

    cr_assert_not_null(ptr, "Pointer should not be NULL");
}

// Example test with memory allocation (to test Valgrind)
Test(memory_suite, test_no_leak)
{
    int *array = new int[10];

    for (int i = 0; i < 10; i++) {
        array[i] = i;
    }
    cr_assert_eq(array[5], 5, "Array value at index 5 should be 5");
    delete[] array; // Properly free memory
}

// Uncomment to test memory leak detection:
// Test(memory_suite, test_with_leak)
// {
//     int *leaked = new int[100];
//     leaked[0] = 42;
//     cr_assert_eq(leaked[0], 42);
//     // Memory leak: delete[] not called
// }

// Uncomment to test crash detection:
// Test(crash_suite, test_segfault)
// {
//     int *ptr = nullptr;
//     *ptr = 42; // This will crash
// }
