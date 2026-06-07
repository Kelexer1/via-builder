#pragma once
#include "../config.h"
#include "Section.h"

#include <bitset>
#include <array>
#include <cstdint>

class Timetable {
public:
    Timetable();

    /**
     * @brief Returns whether this timetable has a conflict with another timetable
     * @param other The other timetable
     * @return True if there is a conflict, false otherwise
     */
    bool isConflicting(const Timetable& other) const;

    /**
     * @brief Returns whether this timetable has a conflict with another section
     * @param other The other timetable
     * @return True if there is a conflict, false otherwise
     */
    bool isConflicting(const Section& other) const;

    std::array<int, MAX_COURSES> chosenSections; // The section index of each chosen section per course, relative to an ordered vector of given courses
    uint32_t coursesPlaced; // The number of courses successfully placed so far

    // These bitsets encode the meeting times for in person, online, and zz using bitsets that flag whether specific
    // time intervals are occupied. These interval sizes are constant and determined in config.h

    std::array<std::array<std::bitset<TIMESLOTS_PER_DAY>, DAYS_PER_WEEK>, MAX_SEMESTERS> inPersonMeetingTimes;
    std::array<std::array<std::bitset<TIMESLOTS_PER_DAY>, DAYS_PER_WEEK>, MAX_SEMESTERS> onlineMeetingTimes;
    std::array<std::array<std::bitset<TIMESLOTS_PER_DAY>, DAYS_PER_WEEK>, MAX_SEMESTERS> zzMeetingTimes;

    // These arrays encode the first and last occurrences of specific types of meeting times, useful to avoid
    // recalculating every time they are needed

    std::array<std::array<uint32_t, DAYS_PER_WEEK>, MAX_SEMESTERS> inPersonFirstMeetingTimes;
    std::array<std::array<uint32_t, DAYS_PER_WEEK>, MAX_SEMESTERS> inPersonLastMeetingTimes;
    std::array<std::array<uint32_t, DAYS_PER_WEEK>, MAX_SEMESTERS> onlineFirstMeetingTimes;
    std::array<std::array<uint32_t, DAYS_PER_WEEK>, MAX_SEMESTERS> onlineLastMeetingTimes;

    std::array<std::array<bool, DAYS_PER_WEEK>, MAX_SEMESTERS> hasInPersonClass; // Whether any day of any semester has at least one in person class

    int fitnessScore; // The current total fitness score of this timetable
};