#include "Timetable.h"
#include "Section.h"
#include <cstdint>

Timetable::Timetable() {
    for (uint32_t i = 0; i < MAX_COURSES; i++) {
        chosenSections[i] = -1;
    }

    // Initialize bitsets and first/last meeting times
    for (uint32_t semester = 0; semester < MAX_SEMESTERS; semester++) {
        for (uint32_t day = 0; day < DAYS_PER_WEEK; day++) {
            inPersonMeetingTimes[semester][day].reset();
            onlineMeetingTimes[semester][day].reset();
            zzMeetingTimes[semester][day].reset();

            inPersonFirstMeetingTimes[semester][day] = TIMESLOTS_PER_DAY;
            inPersonLastMeetingTimes[semester][day] = 0;
            onlineFirstMeetingTimes[semester][day] = TIMESLOTS_PER_DAY;
            onlineLastMeetingTimes[semester][day] = 0;

            hasInPersonClass[semester][day] = false;
        }
    }

    coursesPlaced = 0;
    fitnessScore = 0;
}


bool Timetable::isConflicting(const Timetable& other) const {
    for (uint32_t day = 0; day < DAYS_PER_WEEK; day++) {
        for (uint32_t semester = 0; semester < MAX_SEMESTERS; semester++) {
            const auto thisRegularMeetings =
                this->inPersonMeetingTimes[semester][day] |
                this->onlineMeetingTimes[semester][day];
            const auto otherRegularMeetings =
                other.inPersonMeetingTimes[semester][day] |
                other.onlineMeetingTimes[semester][day];
            const auto thisZzMeetings = this->zzMeetingTimes[semester][day];
            const auto otherZzMeetings = other.zzMeetingTimes[semester][day];

            // Regular/regular conflicts.
            if ((thisRegularMeetings & otherRegularMeetings).any()) return true;

            // Regular/zz conflicts.
            if ((thisRegularMeetings & otherZzMeetings).any()) return true;
            if ((otherRegularMeetings & thisZzMeetings).any()) return true;
        }
    }

    return false;
}


bool Timetable::isConflicting(const Section& other) const {
    for (uint32_t day = 0; day < DAYS_PER_WEEK; day++) {
        for (uint32_t semester = 0; semester < MAX_SEMESTERS; semester++) {
            const auto thisRegularMeetings =
                this->inPersonMeetingTimes[semester][day] |
                this->onlineMeetingTimes[semester][day];
            const auto otherRegularMeetings =
                other.inPersonMeetingTimes[semester][day] |
                other.onlineMeetingTimes[semester][day];
            const auto thisZzMeetings = this->zzMeetingTimes[semester][day];
            const auto otherZzMeetings = other.zzMeetingTimes[semester][day];

            // Regular/regular conflicts.
            if ((thisRegularMeetings & otherRegularMeetings).any()) return true;

            // Regular/zz conflicts.
            if ((thisRegularMeetings & otherZzMeetings).any()) return true;
            if ((otherRegularMeetings & thisZzMeetings).any()) return true;
        }
    }

    return false;
}