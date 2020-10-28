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
	TEST_ASSERT_EQUAL_STRING(expected, output);
	TEST_ASSERT_EQUAL(strlen(expected), chars_written);
	TEST_ASSERT_EQUAL_HEX8(0xaa, output[strlen(expected) + 1]);
}

void test_fptoa_should_pass_with_largest_positive_number(void)
{
	expect("9223372036854775807");
	given(fptoa(output, INT64_MAX, 1, 0, 0));
}

void test_fptoa_should_pass_with_largest_negative_number(void)
{
	expect("-9223372036854775808");
	given(fptoa(output, INT64_MIN, 1, 0, 0));
}

void test_fptoa_should_pass_with_largest_unsigned_number(void)
{
	expect("18446744073709551615");
	given(fptoa(output, UINT64_MAX, 0, 0, 0));
}

void
test_fptoa_should_pass_with_positive_number_non_zero_before_point_and_zero_fraction_to_show__1(
	void)
{
	expect("3");
	given(fptoa(output, 3005, 1, 3, 0));
}

void
test_fptoa_should_pass_with_positive_number_non_zero_before_point_and_zero_fraction_to_show__2(
	void)
{
	expect("30");
	given(fptoa(output, 3005, 1, 2, 0));
}

void
test_fptoa_should_pass_with_positive_number_non_zero_before_point_and_zero_fraction_to_show__3(
	void)
{
	expect("300");
	given(fptoa(output, 3005, 1, 1, 0));
}

void
test_fptoa_should_pass_with_positive_number_non_zero_before_point_and_zero_fraction_to_show__4(
	void)
{
	expect("3005");
	given(fptoa(output, 3005, 1, 0, 0));
}

void
test_fptoa_should_pass_with_a_positive_number_which_has_non_zero_before_point_and_fraction_to_show__1(
	void)
{
	expect("30.0");
	given(fptoa(output, 3005, 1, 2, 1));
}

void
test_fptoa_should_pass_with_a_positive_number_which_has_non_zero_before_point_and_fraction_to_show__2(
	void)
{
	expect("300.5");
	given(fptoa(output, 3005, 1, 1, 1));
}

void
test_fptoa_should_pass_with_a_positive_number_which_has_non_zero_before_point_and_fraction_to_show_bigger_than_actual_fraction(
	void)
{
	expect("300.5");
	given(fptoa(output, 3005, 1, 1, 2));
}

void
test_fptoa_should_pass_with_a_positive_number_which_has_zero_before_point__1(
	void)
{
	expect("0.03005");
	given(fptoa(output, 3005, 1, 5, 5));
}

void
test_fptoa_should_pass_with_a_positive_number_which_has_zero_before_point__2(
	void)
{
	expect("0.03");
	given(fptoa(output, 3005, 1, 5, 2));
}

void
test_fptoa_should_pass_with_a_positive_number_which_has_zero_before_point__3(
	void)
{
	expect("0.3");
	given(fptoa(output, 3005, 1, 4, 1));
}

void
test_fptoa_should_pass_with_a_positive_number_which_has_zero_before_point_and_fraction_to_show_bigger_than_actual_fraction(
	void)
{
	expect("0.03005");
	given(fptoa(output, 3005, 1, 5, 7));
}

void
test_fptoa_should_pass_with_a_negative_number_which_has_non_zero_before_point_and_zero_fraction_to_show__1(
	void)
{
	expect("-3");
	given(fptoa(output, -3005, 1, 3, 0));
}

void
test_fptoa_should_pass_with_a_negative_number_which_has_non_zero_before_point_and_zero_fraction_to_show__2(
	void)
{
	expect("-30");
	given(fptoa(output, -3005, 1, 2, 0));
}

void
test_fptoa_should_pass_with_a_negative_number_which_has_non_zero_before_point_and_fraction_to_show__1(
	void)
{
	expect("-3.0");
	given(fptoa(output, -3005, 1, 3, 1));
}

void
test_fptoa_should_pass_with_a_negative_number_which_has_non_zero_before_point_and_fraction_to_show__2(
	void)
{
	expect("-30.0");
	given(fptoa(output, -3005, 1, 2, 1));
}

void test_fptoa_should_pass_with_zero_number(void)
{
	expect("0");
	given(fptoa(output, 0, 1, 0, 0));
}

int main(void)
{
	UNITY_BEGIN();
	RUN_TEST(test_fptoa_should_pass_with_largest_positive_number);
	RUN_TEST(test_fptoa_should_pass_with_largest_negative_number);
	RUN_TEST(test_fptoa_should_pass_with_largest_unsigned_number);
	RUN_TEST(
		test_fptoa_should_pass_with_positive_number_non_zero_before_point_and_zero_fraction_to_show__1);
	RUN_TEST(
		test_fptoa_should_pass_with_positive_number_non_zero_before_point_and_zero_fraction_to_show__2);
	RUN_TEST(
		test_fptoa_should_pass_with_positive_number_non_zero_before_point_and_zero_fraction_to_show__3);
	RUN_TEST(
		test_fptoa_should_pass_with_positive_number_non_zero_before_point_and_zero_fraction_to_show__4);
	RUN_TEST(
		test_fptoa_should_pass_with_a_positive_number_which_has_non_zero_before_point_and_fraction_to_show__1);
	RUN_TEST(
		test_fptoa_should_pass_with_a_positive_number_which_has_non_zero_before_point_and_fraction_to_show__2);
	RUN_TEST(
		test_fptoa_should_pass_with_a_positive_number_which_has_non_zero_before_point_and_fraction_to_show_bigger_than_actual_fraction);
	RUN_TEST(
		test_fptoa_should_pass_with_a_positive_number_which_has_zero_before_point__1);
	RUN_TEST(
		test_fptoa_should_pass_with_a_positive_number_which_has_zero_before_point__2);
	RUN_TEST(
		test_fptoa_should_pass_with_a_positive_number_which_has_zero_before_point__3);
	RUN_TEST(
		test_fptoa_should_pass_with_a_positive_number_which_has_zero_before_point_and_fraction_to_show_bigger_than_actual_fraction);
	RUN_TEST(
		test_fptoa_should_pass_with_a_negative_number_which_has_non_zero_before_point_and_fraction_to_show__1);
	RUN_TEST(
		test_fptoa_should_pass_with_a_negative_number_which_has_non_zero_before_point_and_fraction_to_show__2);
	RUN_TEST(
		test_fptoa_should_pass_with_a_negative_number_which_has_non_zero_before_point_and_zero_fraction_to_show__1);
	RUN_TEST(
		test_fptoa_should_pass_with_a_negative_number_which_has_non_zero_before_point_and_zero_fraction_to_show__2);
	RUN_TEST(test_fptoa_should_pass_with_zero_number);
	UNITY_END();

	return 0;
}
