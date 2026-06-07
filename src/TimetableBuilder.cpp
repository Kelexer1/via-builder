#include "TimetableBuilder.h"
#include "config.h"
#include "SerializedCourse/Course.h"
#include "SerializedCourse/Section.h"
#include "SerializedCourse/Timetable.h"

#include <bitset>
#include <cstddef>
#include <cstdint>
#include <iterator>
#include <vector>
#include <algorithm>
#include <cmath>

std::optional<Timetable> TimetableBuilder::buildTimetable(std::vector<Course*>& courses, uint32_t* checked) {
    std::vector<Timetable> currentTimetables = { Timetable{} };
    uint32_t courseCount = (uint32_t)courses.size();
    uint32_t timetablesChecked = 0;

    while (!currentTimetables.empty() && currentTimetables[0].coursesPlaced < courseCount) {
        std::vector<Timetable> newCandidates;

        for (Timetable& timetable : currentTimetables) {
            // Find course with lowest placement score to place next
            uint32_t minPlacementScore = UINT32_MAX;
            int courseIndexToPlace = 0;
            for (uint32_t i = 0; i < courseCount; i++) {
                if (timetable.chosenSections[i] == -1) {
                    uint32_t score = getPlacementScore(timetable, courses, i);
                    if (score < minPlacementScore) {
                        minPlacementScore = score;
                        courseIndexToPlace = i;
                    }
                }
            }
            Course* courseToPlace = courses[courseIndexToPlace];

            // Branch into each section and make new potential timetable
            for (size_t i = 0; i < courseToPlace->sections.size(); i++) {
                Section* section = courseToPlace->sections[i];

                if (timetable.isConflicting(*section)) continue;

                Timetable newTimetable = timetable;
                newTimetable.chosenSections[courseIndexToPlace] = i;
                newTimetable.coursesPlaced++;

                addSectionAndUpdateFitness(newTimetable, section, courses);

                if (newTimetable.fitnessScore > FITNESS_PRUNE_THRESHOLD) newCandidates.push_back(newTimetable);
            }
        }

        if (newCandidates.empty() && currentTimetables.empty()) return std::nullopt;

        // Sort timetables by score and keep the top BEAM_WIDTH candidates
        size_t end = std::min(BEAM_WIDTH, (uint32_t)newCandidates.size());
        timetablesChecked += newCandidates.size();
        std::partial_sort(newCandidates.begin(),
            newCandidates.begin() + end,
            newCandidates.end(),
            [](const Timetable& a, const Timetable& b) { return a.fitnessScore > b.fitnessScore; }
        );

        currentTimetables.clear();
        for (uint32_t i = 0; i < BEAM_WIDTH && i < newCandidates.size(); i++) {
            currentTimetables.push_back(newCandidates[i]);
        }
    }

    if (checked) *checked = timetablesChecked;
    return currentTimetables[0];
}

uint32_t TimetableBuilder::getPlacementScore(
    const Timetable& timetable,
    const std::vector<Course*>& courses,
    int index
) {
    uint32_t validSections = 0;

    for (Section* section : courses[index]->sections) {
        if (!(timetable.isConflicting(*section))) validSections++;
    }

    return std::max(1u, validSections);
}

int TimetableBuilder::getFitnessScoreForDay(const Timetable& timetable, const std::vector<Course*>& courses, int day, int semester) {
    int fitness = 0;

    auto meetingTimes = timetable.inPersonMeetingTimes[semester][day] | timetable.onlineMeetingTimes[semester][day];
    uint32_t startTimeslot = std::min(timetable.inPersonFirstMeetingTimes[semester][day], timetable.onlineFirstMeetingTimes[semester][day]);
    uint32_t endTimeslot = std::max(timetable.inPersonLastMeetingTimes[semester][day], timetable.onlineLastMeetingTimes[semester][day]);

    // Day off reward
    if (!timetable.hasInPersonClass[semester][day]) { 
        fitness += DAY_OFF_REWARD;
    }

    if (!meetingTimes.any()) return fitness;

    // Day lengths
    if (endTimeslot >= startTimeslot) {
        uint32_t daySpan = endTimeslot - startTimeslot + 1;
        if (daySpan > MAX_DAY_LENGTH * TIMESLOTS_PER_HOUR) {
            fitness -= MAX_DAY_LENGTH_PENALTY * pow(timeslotsToHours(daySpan - MAX_DAY_LENGTH * TIMESLOTS_PER_HOUR), MAX_DAY_LENGTH_PENALTY_EXPONENT);
        }
        if (daySpan < MIN_DAY_LENGTH * TIMESLOTS_PER_HOUR) {
            fitness -= MIN_DAY_LENGTH_PENALTY * pow(timeslotsToHours(MIN_DAY_LENGTH * TIMESLOTS_PER_HOUR - daySpan), MIN_DAY_LENGTH_PENALTY_EXPONENT);
        }
    }

    const double prefStartSlot = (double)(PREFERRED_MIN_START - DAY_START) / SECONDS_PER_TIMESLOT;
    const double prefEndSlot = (double)(PREFERRED_MAX_END - DAY_START) / SECONDS_PER_TIMESLOT;
    const double startSlot = (double)startTimeslot;
    const double endSlot = (double)endTimeslot;

    const double startDiffHours = std::abs(startSlot - prefStartSlot) / TIMESLOTS_PER_HOUR;
    const int startScore = (int)std::lround(
        std::max(0.0, (double)PREFERRED_MIN_START_REWARD - startDiffHours)
    );

    const double lateEndHours = std::max(0.0, endSlot - prefEndSlot) / TIMESLOTS_PER_HOUR;
    const int endScore = (int)std::lround(
        std::max(0.0, (double)PREFERRED_MAX_END_REWARD - lateEndHours)
    );

    fitness += startScore + endScore;

    // Gaps
    uint32_t currentBlock = 0, maxBlock = 0;
    int lastTimeslot = -1;
    const Course* lastCourse = nullptr;
    const Section* lastSection = nullptr;

    for (uint32_t timeslot = startTimeslot; timeslot <= endTimeslot; timeslot++) {
        if (!meetingTimes.test(timeslot)) {
            currentBlock = 0;
            continue;
        }

        currentBlock++;
        maxBlock = std::max(maxBlock, currentBlock);

        int courseIndex = getCourseIndexAtSlot(timetable, courses, day, semester, timeslot);
        if (courseIndex == -1) continue;

        const Course* course = courses[courseIndex];
        const Section* section = course->sections[timetable.chosenSections[courseIndex]];

        if (lastTimeslot != -1) {
            uint32_t gap = timeslot - lastTimeslot - 1;
            if (gap > MAX_GAP * TIMESLOTS_PER_HOUR) {
                fitness -= MAX_GAP_PENALTY * pow(timeslotsToHours(gap - MAX_GAP * TIMESLOTS_PER_HOUR), MAX_GAP_PENALTY_EXPONENT);
            }

            if (!lastSection || !lastCourse) continue;

            bool lastOnline = lastSection->onlineMeetingTimes[semester][day].test(lastTimeslot);
            bool currentOnline = section->onlineMeetingTimes[semester][day].test(timeslot);

            if (
                GUARANTEE_CROSS_CAMPUS_GAP &&
                !lastOnline && !currentOnline &&
                lastCourse->campus != course->campus &&
                gap < MULTI_CAMPUS_GAP * TIMESLOTS_PER_HOUR
            ) {
                return INT32_MIN;
            }
        }

        lastTimeslot = timeslot;
        lastCourse = course;
        lastSection = section;
    }

    // Continuous classes
    if (maxBlock > MAX_CONTINUOUS_CLASSES * TIMESLOTS_PER_HOUR) {
        fitness -= MAX_CONTINUOUS_CLASSES_PENALTY * pow(timeslotsToHours(maxBlock - MAX_CONTINUOUS_CLASSES * TIMESLOTS_PER_HOUR), MAX_CONTINUOUS_CLASSES_PENALTY_EXPONENT);
    }

    // Rush hour
    if (AVOID_RUSH_HOURS) {
        uint32_t startHour = DAY_START_HOUR + startTimeslot / TIMESLOTS_PER_HOUR;
        uint32_t endHour = DAY_START_HOUR + endTimeslot / TIMESLOTS_PER_HOUR;

        for (auto it = std::begin(RUSH_HOURS); it != std::end(RUSH_HOURS); it++) {
            if (*it == startHour || *it == endHour) fitness -= AVOID_RUSH_HOUR_PENALTY;
        }
    }

    // Prefer online
    if (ONLINE_PREFERENCE != 2) {
        int onlineCountDay = 0;
        int totalCountDay = 0;

        for (uint32_t courseIndex = 0; courseIndex < (uint32_t)courses.size(); courseIndex++) {
            int chosenSectionIndex = timetable.chosenSections[courseIndex];
            if (chosenSectionIndex == -1) continue;

            Section* chosenSection = courses[courseIndex]->sections[chosenSectionIndex];
            int onlineCount = (uint32_t)chosenSection->onlineMeetingTimes[semester][day].count();
            int inPersonCount = (uint32_t)chosenSection->inPersonMeetingTimes[semester][day].count();
            int totalCount = onlineCount + inPersonCount;

            if (totalCount == 0) continue;

            onlineCountDay += onlineCount;
            totalCountDay += totalCount;
        }

        if (totalCountDay > 0) {
            uint32_t score;
            if (ONLINE_PREFERENCE == 0) {
                score = (uint32_t)std::lround((double)(ONLINE_PREFERENCE_REWARD * (totalCountDay - onlineCountDay)) / totalCountDay);
            } else {
                score = (uint32_t)std::lround((double)(ONLINE_PREFERENCE_REWARD * onlineCountDay) / totalCountDay);
            }
            fitness += score;
        }
    }

    return fitness;
}

int TimetableBuilder::getFitnessScoreForWeek(const Timetable& timetable, int semester) {
    int minDaySpan = INT32_MAX, maxDaySpan = INT32_MIN;

    for (uint32_t day = 0; day < DAYS_PER_WEEK; day++) {
        if ((timetable.inPersonMeetingTimes[semester][day] | timetable.onlineMeetingTimes[semester][day]).any()) {
            int start = (int)std::min(timetable.inPersonFirstMeetingTimes[semester][day], timetable.onlineFirstMeetingTimes[semester][day]);
            int end = (int)std::max(timetable.inPersonLastMeetingTimes[semester][day], timetable.onlineLastMeetingTimes[semester][day]);
            int daySpan = end - start + 1;
            minDaySpan = std::min(minDaySpan, daySpan);
            maxDaySpan = std::max(maxDaySpan, daySpan);
        }
    }

    if (minDaySpan == INT32_MAX) minDaySpan = 0;
    if (maxDaySpan == INT32_MIN) maxDaySpan = 0;

    if ((uint32_t)maxDaySpan - (uint32_t)minDaySpan <= BALANCED_WEEK_DELTA * TIMESLOTS_PER_HOUR) return BALANCED_WEEK_DELTA_REWARD;
    return 0;
}

void TimetableBuilder::addSectionAndUpdateFitness(
    Timetable& timetable,
    const Section* section,
    const std::vector<Course*>& courses
) {
    // Remove old daily scores for days that will change
    for (uint32_t semester = 0; semester < MAX_SEMESTERS; semester++) {
        for (uint32_t day = 0; day < DAYS_PER_WEEK; day++) {
            if ((section->inPersonMeetingTimes[semester][day] | section->onlineMeetingTimes[semester][day]).any())
                timetable.fitnessScore -= getFitnessScoreForDay(timetable, courses, day, semester);
        }

        // Remove old weekly score
        timetable.fitnessScore -= getFitnessScoreForWeek(timetable, semester);
    }

    // Update masks and metadata
    for (uint32_t semester = 0; semester < MAX_SEMESTERS; semester++) {
        for (uint32_t day = 0; day < DAYS_PER_WEEK; day++) {
            if (section->inPersonMeetingTimes[semester][day].any()) timetable.hasInPersonClass[semester][day] = true;

            timetable.inPersonMeetingTimes[semester][day] |= section->inPersonMeetingTimes[semester][day];
            timetable.onlineMeetingTimes[semester][day] |= section->onlineMeetingTimes[semester][day];
            timetable.zzMeetingTimes[semester][day] |= section->zzMeetingTimes[semester][day];

            int inPersonStart = section->getInPersonStart(day, semester);
            int inPersonEnd = section->getInPersonEnd(day, semester);
            int onlineStart = section->getOnlineStart(day, semester);
            int onlineEnd = section->getOnlineEnd(day, semester);

            if (inPersonStart != -1) timetable.inPersonFirstMeetingTimes[semester][day] = std::min(timetable.inPersonFirstMeetingTimes[semester][day], (uint32_t)inPersonStart);
            if (inPersonEnd != -1) timetable.inPersonLastMeetingTimes[semester][day] = std::max(timetable.inPersonLastMeetingTimes[semester][day], (uint32_t)inPersonEnd);
            if (onlineStart != -1) timetable.onlineFirstMeetingTimes[semester][day] = std::min(timetable.onlineFirstMeetingTimes[semester][day], (uint32_t)onlineStart);
            if (onlineEnd != -1) timetable.onlineLastMeetingTimes[semester][day] = std::max(timetable.onlineLastMeetingTimes[semester][day], (uint32_t)onlineEnd);
        }
    }

    // Add new daily scores for changed days
    for (uint32_t semester = 0; semester < MAX_SEMESTERS; semester++) {
        for (uint32_t day = 0; day < DAYS_PER_WEEK; day++) {
            if ((section->inPersonMeetingTimes[semester][day] | section->onlineMeetingTimes[semester][day]).any())
                timetable.fitnessScore += getFitnessScoreForDay(timetable, courses, day, semester);
        }

        // Add new weekly score
        timetable.fitnessScore += getFitnessScoreForWeek(timetable, semester);
    }
}

int TimetableBuilder::getCourseIndexAtSlot(
    const Timetable& timetable,
    const std::vector<Course*>& courses,
    int day,
    int semester,
    int timeslot
) {
    for (uint32_t courseIndex = 0; courseIndex < (uint32_t)courses.size(); courseIndex++) {
        int sectionIndex = timetable.chosenSections[courseIndex];
        if (sectionIndex == -1) continue;

        Section* section = courses[courseIndex]->sections[sectionIndex];
        if (
            section->inPersonMeetingTimes[semester][day].test(timeslot) ||
            section->onlineMeetingTimes[semester][day].test(timeslot)
        ) {
            return courseIndex;
        }
    }

    return -1;
}

double TimetableBuilder::timeslotsToHours(uint32_t timeslots) {
    return (double)timeslots / TIMESLOTS_PER_HOUR;
}