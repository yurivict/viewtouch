#include "gtest/gtest.h"

#include "time_info.hh"

// external system libraries

// standard libraries
#include <string>
#include <vector>
#include <fstream>
#include <limits> // std::numeric_limits
#include <cmath> // std::isnan
#include <locale>

TEST(time_info, addition_seconds_basic)
{}
TEST(time_info, addition_seconds_wrapping_minute)
{}
TEST(time_info, addition_seconds_wrapping_hour)
{}
TEST(time_info, addition_seconds_wrapping_day)
{}
TEST(time_info, addition_seconds_wrapping_month)
{}
TEST(time_info, addition_seconds_wrapping_year)
{}
TEST(time_info, addition_minutes_basic)
{}
TEST(time_info, addition_minutes_wrapping_hour)
{}
TEST(time_info, addition_minutes_wrapping_day)
{}
TEST(time_info, addition_hours_basic)
{}
TEST(time_info, addition_hours_wrapping_day)
{}
TEST(time_info, addition_days_basic)
{}
TEST(time_info, addition_days_wrapping_month)
{}
TEST(time_info, addition_month_basic)
{}
TEST(time_info, addition_month_wrapping_year)
{}
TEST(time_info, addition_year_basic)
{}
TEST(time_info, set_year_results_in_jan_first_midnight)
{
    TimeInfo ti;
    ti.Set(0, 2018);
    EXPECT_EQ(ti.Year(), 2018);
    EXPECT_EQ(ti.Month(), 1);
    EXPECT_EQ(ti.Day(), 1);
    EXPECT_EQ(ti.Hour(), 0);
    EXPECT_EQ(ti.Min(), 0);
    EXPECT_EQ(ti.Sec(), 0);
}

TEST(time_info, set_yesterday_night_00_00)
{
    TimeInfo ti;
    EXPECT_FALSE(ti.IsSet());
    // set date to yesterday morning, 00:00
    ti.Set(60*60*24*3 + 60*30 + 24, 2018); // 2018-01-04 00:30:24
    EXPECT_EQ(ti.Year(), 2018);
    EXPECT_EQ(ti.Month(), 1);
    EXPECT_EQ(ti.Day(), 4);
    EXPECT_EQ(ti.Hour(), 0);
    EXPECT_EQ(ti.Min(), 30);
    ASSERT_EQ(ti.Sec(), 24);
    ti -= date::days(1);
    ASSERT_EQ(ti.Day(), 3); // 2018-01-03 00:30:24
    EXPECT_EQ(ti.Hour(), 0);
    EXPECT_EQ(ti.Min(), 30);
    ASSERT_EQ(ti.Sec(), 24);
    ti.Floor<date::days>(); // 2018-01-03 00:00:00
    ASSERT_EQ(ti.Day(), 3);
    ASSERT_EQ(ti.Hour(), 0);
    ASSERT_EQ(ti.Min(),  0);
    ASSERT_EQ(ti.Sec(),  0);
}
TEST(time_info, set_yesterday_night_23_59)
{
    TimeInfo ti;
    EXPECT_FALSE(ti.IsSet());
    // set date to last night, 23:59
    ti.Set(60*60*24*3 + 60*30 + 24, 2018); // 2018-01-04 00:30:24
    ti.Floor<date::days>(); // 2018-01-04 00:00:00
    ti -= std::chrono::seconds(1); // 2018-01-03 23:59:59
    EXPECT_EQ(ti.Day(),   3);
    EXPECT_EQ(ti.Hour(), 23);
    EXPECT_EQ(ti.Min(),  59);
    EXPECT_EQ(ti.Sec(),  59);
}
TEST(time_info, ceil_to_last_day_of_month)
{
    TimeInfo ti;
    EXPECT_FALSE(ti.IsSet());
    // set date to last night, 23:59
    ti.Set(60*60*24*31 + 60*30 + 24, 2018); // 2018-01-04 00:30:24
    //std::cout << ti.to_string() << std::endl;
    //ti.Ceil<date::months>(); // 2018-01-31 23::59:59
    //std::cout << ti.to_string() << std::endl;
    //EXPECT_EQ(ti.Month(), 1);
    //EXPECT_EQ(ti.Day(),  31);
    //EXPECT_EQ(ti.Hour(), 23);
    //EXPECT_EQ(ti.Min(),  59);
    //EXPECT_EQ(ti.Sec(),  59);
    // expect last day of month
}

TEST(time_info, adjust_month_doesnt_change_day_and_time)
{
    TimeInfo ti;
    ti.Set(60*60*24*3 + 60*30 + 24, 2018); // 2018-01-04 00:30:24
    std::cout << ti.to_string() << std::endl;
    ti += date::months(1);
    std::cout << ti.to_string() << std::endl;
    EXPECT_EQ(ti.Month(), 2);
    EXPECT_EQ(ti.Day(),   4);
    EXPECT_EQ(ti.Hour(),  0);
    EXPECT_EQ(ti.Min(),  30);
    EXPECT_EQ(ti.Sec(),  24);
}
TEST(time_info, adjust_year_doesnt_change_day_and_time)
{
    TimeInfo ti;
    ti.Set(60*60*24*3 + 60*30 + 24, 2018); // 2018-01-04 00:30:24
    std::cout << ti.to_string() << std::endl;
    ti += date::years(1);
    std::cout << ti.to_string() << std::endl;
    EXPECT_EQ(ti.Month(), 1);
    EXPECT_EQ(ti.Day(),   4);
    EXPECT_EQ(ti.Hour(),  0);
    EXPECT_EQ(ti.Min(),  30);
    EXPECT_EQ(ti.Sec(),  24);
}
TEST(time_info, same_time_after_substracting_and_adding_durations)
{
    TimeInfo ti;
    ti.Set(60*60*24*3 + 60*30 + 24, 2018); // 2018-01-04 00:30:24
    ti -= std::chrono::seconds(1);
    ti -= std::chrono::minutes(1);
    ti -= std::chrono::hours(1);
    ti -= date::days(1);
    ti -= date::months(1);
    ti -= date::years(1);
    ti += date::years(1);
    ti += date::months(1);
    ti += date::days(1);
    ti += std::chrono::hours(1);
    ti += std::chrono::minutes(1);
    ti += std::chrono::seconds(1);

    // we should be when we started
    EXPECT_EQ(ti.Year(), 2018);
    EXPECT_EQ(ti.Month(), 1);
    EXPECT_EQ(ti.Day(),   4);
    EXPECT_EQ(ti.Hour(),  0);
    EXPECT_EQ(ti.Min(),  30);
    EXPECT_EQ(ti.Sec(),  24);
}


TEST(time_info, half_month_jump_1_15_doesnt_change_time)
{
    TimeInfo ti;
    ti.Set(60*60*24*3 + 60*30 + 24, 2018); // 2018-01-04 00:30:24
    constexpr int d_low = 1;
    constexpr int d_high = 15;
    ti.half_month_jump(1, d_low, d_high);

    // same time after half-month-jump
    EXPECT_EQ(ti.Hour(),  0);
    EXPECT_EQ(ti.Min(),  30);
    EXPECT_EQ(ti.Sec(),  24);
}

TEST(time_info, half_month_jump_1_15_forward_snaps_backwards)
{
    TimeInfo ti;
    ti.Set(60*60*24*3 + 60*30 + 24, 2018); // 2018-01-04 00:30:24
    constexpr int d_low = 1;
    constexpr int d_high = 15;
    ti.half_month_jump(1, d_low, d_high);

    // we should be when we started
    EXPECT_EQ(ti.Year(), 2018);
    EXPECT_EQ(ti.Month(), 1);
    EXPECT_EQ(ti.Day(),   1);
    EXPECT_EQ(ti.Hour(),  0);
    EXPECT_EQ(ti.Min(),  30);
    EXPECT_EQ(ti.Sec(),  24);
}

TEST(time_info, half_month_jump_1_15_forward_on_first_jumps_to_15th)
{
    TimeInfo ti;
    ti.Set(60*30 + 24, 2018); // 2018-01-01 00:30:24
    constexpr int d_low = 1;
    constexpr int d_high = 15;
    ti.half_month_jump(1, d_low, d_high);

    // we should be when we started
    EXPECT_EQ(ti.Year(), 2018);
    EXPECT_EQ(ti.Month(), 1);
    EXPECT_EQ(ti.Day(),  15);
    EXPECT_EQ(ti.Hour(),  0);
    EXPECT_EQ(ti.Min(),  30);
    EXPECT_EQ(ti.Sec(),  24);
}

TEST(time_info, half_month_jump_1_15_forward_on_15th_jumps_to_next_month_first)
{
    TimeInfo ti;
    ti.Set(60*30 + 24, 2018); // 2018-01-01 00:30:24
    constexpr int d_low = 1;
    constexpr int d_high = 15;
    ti.half_month_jump(1, d_low, d_high); // 2018-01-15 00:30:24
    ti.half_month_jump(1, d_low, d_high); // 2018-02-01 00:30:24

    EXPECT_EQ(ti.Year(), 2018);
    EXPECT_EQ(ti.Month(), 2);
    EXPECT_EQ(ti.Day(),   1);
    EXPECT_EQ(ti.Hour(),  0);
    EXPECT_EQ(ti.Min(),  30);
    EXPECT_EQ(ti.Sec(),  24);
}

TEST(time_info, half_month_jump_11_26_forward_snaps_backwards)
{
    TimeInfo ti;
    ti.Set(60*60*24*3 + 60*30 + 24, 2018); // 2018-01-04 00:30:24
    constexpr int d_low = 11;
    constexpr int d_high = 26;
    ti.half_month_jump(1, d_low, d_high);

    EXPECT_EQ(ti.Year(), 2017);
    EXPECT_EQ(ti.Month(), 12);
    EXPECT_EQ(ti.Day(),   26);
    EXPECT_EQ(ti.Hour(),  0);
    EXPECT_EQ(ti.Min(),  30);
    EXPECT_EQ(ti.Sec(),  24);
}

TEST(time_info, half_month_jump_11_26_forward_on_11th_jumps_to_26th)
{
    TimeInfo ti;
    ti.Set(60*60*24*10 + 60*30 + 24, 2018); // 2018-01-11 00:30:24
    constexpr int d_low = 11;
    constexpr int d_high = 26;
    ti.half_month_jump(1, d_low, d_high); // 2018-01-26 00:30:24

    EXPECT_EQ(ti.Year(), 2018);
    EXPECT_EQ(ti.Month(), 1);
    EXPECT_EQ(ti.Day(),  26);
    EXPECT_EQ(ti.Hour(),  0);
    EXPECT_EQ(ti.Min(),  30);
    EXPECT_EQ(ti.Sec(),  24);
}

TEST(time_info, half_month_jump_11_26_forward_on_26th_jumps_to_11th_of_next_month)
{
    TimeInfo ti;
    ti.Set(60*60*24*25 + 60*30 + 24, 2018); // 2018-01-26 00:30:24
    constexpr int d_low = 11;
    constexpr int d_high = 26;
    ti.half_month_jump(1, d_low, d_high); // 2018-02-11 00:30:24

    EXPECT_EQ(ti.Year(), 2018);
    EXPECT_EQ(ti.Month(), 2);
    EXPECT_EQ(ti.Day(),  11);
    EXPECT_EQ(ti.Hour(),  0);
    EXPECT_EQ(ti.Min(),  30);
    EXPECT_EQ(ti.Sec(),  24);
}

TEST(time_info, half_month_jump_1_15_backward_snaps_forwards)
{
    TimeInfo ti;
    ti.Set(60*60*24*3 + 60*30 + 24, 2018); // 2018-01-04 00:30:24
    constexpr int d_low = 1;
    constexpr int d_high = 15;
    ti.half_month_jump(-1, d_low, d_high); // 2018-01-15 00:30:24

    // we should be when we started
    EXPECT_EQ(ti.Year(), 2018);
    EXPECT_EQ(ti.Month(), 1);
    EXPECT_EQ(ti.Day(),  15);
    EXPECT_EQ(ti.Hour(),  0);
    EXPECT_EQ(ti.Min(),  30);
    EXPECT_EQ(ti.Sec(),  24);
}

TEST(time_info, half_month_jump_1_15_backward_on_first_jumps_to_15th_of_last_month)
{
    TimeInfo ti;
    ti.Set(60*30 + 24, 2018); // 2018-01-01 00:30:24
    constexpr int d_low = 1;
    constexpr int d_high = 15;
    ti.half_month_jump(-1, d_low, d_high); // 2017-12-15 00:30:24

    // we should be when we started
    EXPECT_EQ(ti.Year(), 2017);
    EXPECT_EQ(ti.Month(),12);
    EXPECT_EQ(ti.Day(),  15);
    EXPECT_EQ(ti.Hour(),  0);
    EXPECT_EQ(ti.Min(),  30);
    EXPECT_EQ(ti.Sec(),  24);
}

TEST(time_info, half_month_jump_1_15_backward_on_15th_jumps_to_first)
{
    TimeInfo ti;
    ti.Set(60*60*24*14 + 60*30 + 24, 2018); // 2018-01-15 00:30:24
    constexpr int d_low = 1;
    constexpr int d_high = 15;
    ti.half_month_jump(-1, d_low, d_high); // 2018-01-01 00:30:24

    EXPECT_EQ(ti.Year(), 2018);
    EXPECT_EQ(ti.Month(), 1);
    EXPECT_EQ(ti.Day(),   1);
    EXPECT_EQ(ti.Hour(),  0);
    EXPECT_EQ(ti.Min(),  30);
    EXPECT_EQ(ti.Sec(),  24);
}

TEST(time_info, half_month_jump_11_26_backward_snaps_forward)
{
    TimeInfo ti;
    ti.Set(60*60*24*3 + 60*30 + 24, 2018); // 2018-01-04 00:30:24
    constexpr int d_low = 11;
    constexpr int d_high = 26;
    ti.half_month_jump(-1, d_low, d_high); // 2018-01-11 00:30:24

    EXPECT_EQ(ti.Year(), 2018);
    EXPECT_EQ(ti.Month(), 1);
    EXPECT_EQ(ti.Day(),  11);
    EXPECT_EQ(ti.Hour(),  0);
    EXPECT_EQ(ti.Min(),  30);
    EXPECT_EQ(ti.Sec(),  24);
}

TEST(time_info, half_month_jump_11_26_backward_on_11th_jumps_to_26th_of_previous_month)
{
    TimeInfo ti;
    ti.Set(60*60*24*10 + 60*30 + 24, 2018); // 2018-01-11 00:30:24
    constexpr int d_low = 11;
    constexpr int d_high = 26;
    ti.half_month_jump(-1, d_low, d_high); // 2017-12-26 00:30:24

    EXPECT_EQ(ti.Year(), 2017);
    EXPECT_EQ(ti.Month(),12);
    EXPECT_EQ(ti.Day(),  26);
    EXPECT_EQ(ti.Hour(),  0);
    EXPECT_EQ(ti.Min(),  30);
    EXPECT_EQ(ti.Sec(),  24);
}

TEST(time_info, half_month_jump_11_26_backward_on_26th_jumps_to_11th)
{
    TimeInfo ti;
    ti.Set(60*60*24*25 + 60*30 + 24, 2018); // 2018-01-26 00:30:24
    constexpr int d_low = 11;
    constexpr int d_high = 26;
    ti.half_month_jump(-1, d_low, d_high); // 2018-01-11 00:30:24

    EXPECT_EQ(ti.Year(), 2018);
    EXPECT_EQ(ti.Month(), 1);
    EXPECT_EQ(ti.Day(),  11);
    EXPECT_EQ(ti.Hour(),  0);
    EXPECT_EQ(ti.Min(),  30);
    EXPECT_EQ(ti.Sec(),  24);
}


TEST(time_info, floor_days_results_in_zero_hours_mins_secs)
{
    TimeInfo ti;
    EXPECT_FALSE(ti.IsSet());
    // set date to last night, 23:59
    ti.Set();
    ti.Floor<date::days>();
    // expect zero time
}
// Floor

TEST(time_info, floor_years_sets_all_lower_values_to_start)
{
    TimeInfo ti;
    EXPECT_FALSE(ti.IsSet());
    ti.Floor<date::years>();
    EXPECT_EQ(ti.Sec(), 0);
    EXPECT_EQ(ti.Min(), 0);
    EXPECT_EQ(ti.Hour(), 0);
    EXPECT_EQ(ti.Day(), 1);
    EXPECT_EQ(ti.Month(), 1);
}

TEST(time_info, check_weekday_has_the_ctime_indizes)
{
    TimeInfo ti;
    EXPECT_FALSE(true);
}