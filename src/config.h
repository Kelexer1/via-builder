#pragma once
#include <cstdint>

// Variable fitness preferences
extern uint32_t MAX_GAP; // The max number of hours between two classes
extern uint32_t MAX_DAY_LENGTH; // The max length of every day in hours
extern uint32_t MIN_DAY_LENGTH; // The min length of every day in hours
extern uint32_t MAX_CONTINUOUS_CLASSES; // The max number of hours of classes in a row
extern uint32_t PREFFERED_MIN_START; // The preffered smallest hour that every day should start at
extern uint32_t PREFFERED_MAX_END; // The preffered latest hour that every day should end at

extern bool GUARANTEE_CROSS_CAMPUS_GAP; // Whether to penalize two (in person) meeting times close together that are at different campuses

extern bool AVOID_RUSH_HOURS; // Whether to penalize start and end times that fall into typical rush hours (useful for commuters)

extern uint32_t ONLINE_PREFERENCE; // 0 = In person, 1 = Online, 2 = Neutral


// Variable fitness config
// Penalties are typically calculated as Penalty * Num_Infractions^PenaltyExponent
// The number of infractions for time based penalties is usually the number of hours

extern uint32_t MAX_GAP_PENALTY;
extern double MAX_GAP_PENALTY_EXPONENT;

extern uint32_t MAX_DAY_LENGTH_PENALTY;
extern double MAX_DAY_LENGTH_PENALTY_EXPONENT;

extern uint32_t MIN_DAY_LENGTH_PENALTY;
extern double MIN_DAY_LENGTH_PENALTY_EXPONENT;

extern uint32_t MAX_CONTINUOUS_CLASSES_PENALTY;
extern double MAX_CONTINUOUS_CLASSES_PENALTY_EXPONENT;

extern uint32_t PREFFERED_MIN_START_REWARD; // Applied once per day

extern uint32_t PREFFERED_MAX_END_REWARD; // Applied once per day


// Constant fitness rewards and penalties
extern uint32_t MULTI_CAMPUS_GAP; // How many hours to place between two in person meetings occuring at different campuses

extern uint32_t AVOID_RUSH_HOUR_PENALTY; // Applied per hour overlapping (can be at most twice, for start and end overlaps)

extern uint32_t ONLINE_PREFERENCE_REWARD; // Applied per day

extern uint32_t BALANCED_WEEK_DELTA_REWARD; // Applied per week (aka per semester)
extern uint32_t BALANCED_WEEK_DELTA; // How much the day lengths in a week can differ before foregoing the balanced week reward

extern uint32_t DAY_OFF_REWARD; // Added per day off


// Build algorithm settings
const int FITNESS_PRUNE_THRESHOLD = INT32_MIN; // When to decide to stop exploring a timetable due to it being too bad

// Explore up to BEAM_WIDTH possible placements per course (most of the time there are < 10 options anyways) and then
// keep the best BEAM_WIDTH for the next iteration
const uint32_t BEAM_WIDTH = 100;


// Constants
const uint32_t MAX_COURSES = 42; // How many courses to allocate room for, this allows 2 semesters of 7 courses, each with a LEC, TUT, and PRA
const uint32_t MAX_SEMESTERS = 2; // How many semesters to allocate room for, 2 is good for most uses (Fall-Winter etc.)

const uint32_t DAY_START_HOUR = 8; // The earliest hour that a class will start (8am should be good)
const uint32_t DAY_START = DAY_START_HOUR * 3600; // In seconds

const uint32_t DAY_END_HOUR = 23; // The latest hour that a class will end (11pm should be good)
const uint32_t DAY_END = DAY_END_HOUR * 3600; // In seconds

const uint32_t DAYS_PER_WEEK = 7; // How many days to allocate room for (Can be 6 if Sunday etc. is guaranteed to not have meeting times)
const uint32_t HOURS_PER_DAY = DAY_END_HOUR - DAY_START_HOUR + 1; // How many hours each day spans from start to end

// How many timeslots each hour is broken into (15 mins per timeslot should be good, classes probably start on 30 minute intervals at worst)
const uint32_t TIMESLOTS_PER_HOUR = 4;
const uint32_t TIMESLOTS_PER_DAY = TIMESLOTS_PER_HOUR * HOURS_PER_DAY; // Extend the number of timeslots per hour to the number per day
const uint32_t SECONDS_PER_TIMESLOT = 3600 / TIMESLOTS_PER_HOUR; // How many seconds must have elapsed to elapse one timeslot

const uint32_t RUSH_HOURS[] = { 8, 9, 17, 18 }; // 8am, 9am, 5pm, 6pm are pretty standard rush hours
const uint32_t NUM_RUSH_HOURS = 4;

// Day numbering info:
// Days are numbered from 0-6 as follows: Sun = 0, Mon = 1, Tue = 2, Wed = 3, Thu = 4, Fri = 5, Sat = 6