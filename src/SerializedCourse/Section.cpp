#include "Section.h"
#include <cstddef>

Section::Section(const std::string& name) : name(name) {}


void Section::addMeetingTime(size_t startSeconds, size_t endSeconds, int day, bool online, bool zz, int semester) {
    // Timeslot start and end
    size_t startPos = (startSeconds - DAY_START) / SECONDS_PER_TIMESLOT;
    size_t endPos = (endSeconds - DAY_START) / SECONDS_PER_TIMESLOT;

    // Fill the correct bitset from startPos..endPos
    for (size_t i = startPos; i < endPos; i++) {
        if (online) this->onlineMeetingTimes[semester][day].set(i);
        else if (zz) this->zzMeetingTimes[semester][day].set(i);
        else this->inPersonMeetingTimes[semester][day].set(i);
    }
}


bool Section::isConflicting(const Section& other) {
    // Compare all days of all semesters
    for (uint32_t day = 0; day < DAYS_PER_WEEK; day++) {
        for (uint32_t semester = 0; semester < MAX_SEMESTERS; semester++) {
            const auto thisRegularMeetings = this->inPersonMeetingTimes[semester][day] | this->onlineMeetingTimes[semester][day];
            const auto otherRegularMeetings = other.inPersonMeetingTimes[semester][day] | other.onlineMeetingTimes[semester][day];
            const auto thisZzMeetings = this->zzMeetingTimes[semester][day];
            const auto otherZzMeetings = other.zzMeetingTimes[semester][day];

            // Regular/Regular conflicts
            if ((thisRegularMeetings & otherRegularMeetings).any()) return true;

            // Regular/zz conflicts
            if ((thisRegularMeetings & otherZzMeetings).any()) return true;
            if ((otherRegularMeetings & thisZzMeetings).any()) return true;
        }
    }

    return false;
}


int Section::getInPersonStart(int day, int semester) const {
    for (uint32_t i = 0; i < TIMESLOTS_PER_DAY; i++) {
        if (this->inPersonMeetingTimes[semester][day].test(i)) return i;
    }

    return -1;
}


int Section::getInPersonEnd(int day, int semester) const {
    for (int i = TIMESLOTS_PER_DAY - 1; i >= 0; i--) {
        if (this->inPersonMeetingTimes[semester][day].test(i)) return i;
    }

    return -1;
}


int Section::getOnlineStart(int day, int semester) const {
    for (uint32_t i = 0; i < TIMESLOTS_PER_DAY; i++) {
        if (this->onlineMeetingTimes[semester][day].test(i)) return i;
    }

    return -1;
}


int Section::getOnlineEnd(int day, int semester) const {
    for (int i = TIMESLOTS_PER_DAY - 1; i >= 0; i--) {
        if (this->onlineMeetingTimes[semester][day].test(i)) return i;
    }

    return -1;
}