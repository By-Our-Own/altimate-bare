#include "deadline.h"
#include "unity.h"
#include "mock_delay.h"

static uint16_t expected;

void setUp(void)
{
}

void tearDown(void)
{
}

static void expect(uint16_t n)
{
	expected = n;
}

static void given(uint16_t returned)
{
	TEST_ASSERT_EQUAL_UINT16(expected, returned);
}

void test_deadline_extend_should_extend_by_everything(void)
{
	uint16_t current;
	uint16_t increment;

	current = 0U;
	increment = 1U;
	expect(1U);
	given(deadline_extend(current, increment));

	current = 2U;
	increment = 5U;
	expect(7U);
	given(deadline_extend(current, increment));

	current = UINT16_MAX;
	increment = 1U;
	expect(0U);
	given(deadline_extend(current, increment));

	current = UINT16_MAX;
	increment = UINT16_MAX;
	expect(UINT16_MAX - 1);
	given(deadline_extend(current, increment));
}

void test_deadline_make_should_return_current_time_if_zero(void)
{
	uint16_t millis;
	uint16_t new_dl;

	millis = 9U;
	new_dl = 0;
	expect(millis);
	millis_ExpectAndReturn(millis);
	given(deadline_make(new_dl));
}

void test_deadline_make_should_pass_on_non_zero_deadlines(void)
{
	uint16_t millis;
	uint16_t new_dl;

	millis = 9U;
	new_dl = 5U;
	expect(14U);
	millis_ExpectAndReturn(millis);
	given(deadline_make(new_dl));
}

void test_deadline_reached_should_pass_with_no_wraparound(void)
{
	uint16_t millis;
	uint16_t dl;

	millis = 10U;
	dl = 9U;
	expect(1U);
	millis_ExpectAndReturn(millis);
	given(deadline_reached(dl));

	millis = 10U;
	dl = 10U;
	expect(1U);
	millis_ExpectAndReturn(millis);
	given(deadline_reached(dl));

	millis = 10U;
	dl = 11U;
	expect(0U);
	millis_ExpectAndReturn(millis);
	given(deadline_reached(dl));
}

void test_deadline_reached_should_pass_with_wraparound(void)
{
	uint16_t millis;
	uint16_t dl;

	millis = 1U;
	dl = UINT16_MAX - 3U;
	expect(1U);
	millis_ExpectAndReturn(millis);
	given(deadline_reached(dl));
}

int main(void)
{
	UNITY_BEGIN();
	RUN_TEST(test_deadline_extend_should_extend_by_everything);
	RUN_TEST(test_deadline_make_should_return_current_time_if_zero);
	RUN_TEST(test_deadline_make_should_pass_on_non_zero_deadlines);
	RUN_TEST(test_deadline_reached_should_pass_with_no_wraparound);
	RUN_TEST(test_deadline_reached_should_pass_with_wraparound);
	UNITY_END();

	return 0;
}
