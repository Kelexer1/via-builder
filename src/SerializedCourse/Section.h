#pragma once
#include "../config.h"

#include <cstddef>
#include <string>
#include <bitset>
#include <array>

class Section {
public:
    Section(const std::string& name);

    /**
     * @brief Registers a new meeting time to this section
     * @param startSeconds The seconds after midnight that this meeting time starts
     * @param endSeconds The seconds after midnight that this meeting time end
     * @param day The day as an integer (check README.MD for day numberings)
     * @param online Whether this meeting takes place online
     * @param zz Whether this meeting time is misc (often used for tests etc.)
     * @param semester Which semester this meeting time is in (must be from 0..MAX_SEMESTERS-1)
     */
    void addMeetingTime(size_t startSeconds, size_t endSeconds, int day, bool online, bool zz, int semester);

    /**
     * @brief Returns whether this section has a conflict with another section (zz/zz conflicts are ignored)
     * @param other The other section
     * @return True if there is a conflict, false otherwise
     */
    bool isConflicting(const Section& other);

    /**
     * @brief Returns the timeslot where the first occuring in person meeting occurs
     * @param day The day as an integer (check README.md for day numberings)
     * @param semester Which semester to check (must be from 0..MAX_SEMESTERS-1)
     */
    int getInPersonStart(int day, int semester) const;

    /**
     * @brief Returns the timeslot where the last occuring in person meeting occurs
     * @param day The day as an integer (check README.md for day numberings)
     * @param semester Which semester to check (must be from 0..MAX_SEMESTERS-1)
     */
    int getInPersonEnd(int day, int semester) const;

    /**
     * @brief Returns the timeslot where the last occuring online meeting occurs
     * @param day The day as an integer (check README.md for day numberings)
     * @param semester Which semester to check (must be from 0..MAX_SEMESTERS-1)
     */
    int getOnlineStart(int day, int semester) const;

    /**
     * @brief Returns the timeslot where the last occuring online meeting occurs
     * @param day The day as an integer (check README.md for day numberings)
     * @param semester Which semester to check (must be from 0..MAX_SEMESTERS-1)
     */
    int getOnlineEnd(int day, int semester) const;

    std::string name; // The name of this section (ex. "LEC0101")

    // These bitsets encode the meeting times for in person, online, and zz using bitsets that flag whether specific
    // time intervals are occupied. These interval sizes are constant and determined in config.h

    std::array<std::array<std::bitset<TIMESLOTS_PER_DAY>, DAYS_PER_WEEK>, MAX_SEMESTERS> inPersonMeetingTimes;
    std::array<std::array<std::bitset<TIMESLOTS_PER_DAY>, DAYS_PER_WEEK>, MAX_SEMESTERS> onlineMeetingTimes;
    std::array<std::array<std::bitset<TIMESLOTS_PER_DAY>, DAYS_PER_WEEK>, MAX_SEMESTERS> zzMeetingTimes;
};