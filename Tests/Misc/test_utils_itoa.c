#include "utils.h"
#include "unity.h"
#include <string.h>

static char output[100];
static const char *expected;

void setUp(void)
{
    memset(output, 0xaa, sizeof output);
    expected = "";
}

void tearDown(void)
{
}

static void expect(const char *s)
{
    expected = s;
}

static void given(uint8_t chars_written)
{
    TEST_ASSERT_EQUAL(strlen(expected), chars_written);
    TEST_ASSERT_EQUAL_STRING(expected, output);
    TEST_ASSERT_EQUAL_HEX8(0xaa, output[strlen(expected) + 1]);
}

void test_itoa_should_pass_with_largest_positive_number(void)
{
    expect("9223372036854775807");
    given(itoa(output, INT64_MAX, 1));
}

void test_itoa_should_pass_with_largest_negative_number(void)
{
    expect("-9223372036854775808");
    given(itoa(output, INT64_MIN, 1));
}

void test_itoa_should_pass_with_largest_unsigned_number(void)
{
    expect("18446744073709551615");
    given(itoa(output, UINT64_MAX, 0));
}

void test_itoa_should_pass_with_a_positive_number(void)
{
    expect("30");
    given(itoa(output, 30, 1));
}

void test_itoa_should_pass_with_a_negative_number(void)
{
    expect("-30");
    given(itoa(output, -30, 1));
}

void test_itoa_should_pass_with_zero_number(void)
{
    expect("0");
    given(itoa(output, 0, 1));
}

int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_itoa_should_pass_with_largest_positive_number);
    RUN_TEST(test_itoa_should_pass_with_largest_negative_number);
    RUN_TEST(test_itoa_should_pass_with_largest_unsigned_number);
    RUN_TEST(test_itoa_should_pass_with_a_positive_number);
    RUN_TEST(test_itoa_should_pass_with_a_negative_number);
    RUN_TEST(test_itoa_should_pass_with_zero_number);
    UNITY_END();

    return 0;
}
