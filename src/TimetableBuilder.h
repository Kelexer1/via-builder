#pragma once
#include "SerializedCourse/Course.h"
#include "SerializedCourse/Timetable.h"

#include <optional>
#include <vector>

class TimetableBuilder {
public:
    /**
     * @brief Builds a timetable containing all the courses provided, as well as the config in config.h and config.c
     * @param courses The courses that need to be placed
     * @param checked Will be modified to contain the total number of timetables checked
     * @return std::nullopt if no valid timetable was found, otherwise a Timetable object encoding the generated timetable
     * and some other information about it
     */
    static std::optional<Timetable> buildTimetable(std::vector<Course*>& courses, uint32_t* checked = nullptr);

private:
    /**
     * @brief Calculates the priority of a specific course being placed based on how many possible sections it has, and how many
     * of those cause conflicts. Smaller priorities (more possible options) are placed later on
     * @param timetable The current state of the timetable
     * @param courses The courses being placed
     * @param index The index of the course being checked (relative to <courses>)
     * @return The placement score
     */
    static uint32_t getPlacementScore(
        const Timetable& timetable,
        const std::vector<Course*>& courses,
        int index
    );

    /**
     * @brief Return the fitness score of a timetable on a specific day
     * @param timetable The timetable to check the score of
     * @param courses The courses being placed
     * @param day The day as an integer (check config.h for day numberings)
     * @param semester Which semester to check (must be from 0..MAX_SEMESTERS-1)
     * @return The fitness score
     */
    static int getFitnessScoreForDay(const Timetable& timetable, const std::vector<Course*>& courses, int day, int semester);

    /**
     * @brief Return the fitness score additions for the entire week in a semester, handling penalties and bonuses
     * which require the state of multiple days to calculate. Does NOT include individual day scores
     * @param timetable The timetable to check the score of
     * @param semester Which semester to check (must be from 0..MAX_SEMESTERS-1)
     */
    static int getFitnessScoreForWeek(const Timetable& timetable, int semester);

    /**
     * @brief Adds a section of a course to a timetable, and updates the timetables masks, score, and other important data
     * @param timetable The timetable to add the section to
     * @param section The section to add
     * @param courses The courses being placed
     */
    static void addSectionAndUpdateFitness(
        Timetable& timetable,
        const Section* section,
        const std::vector<Course*>& courses
    );

    /**
     * @brief Returns the index into the courses vector based on what course is blocking a specific timeslot in a timetable
     * @param timetable The timetable to check
     * @param courses The courses being placed
     * @param day The day as an integer (check config.h for day numberings)
     * @param semester Which semester to check (must be from 0..MAX_SEMESTERS-1)
     * @param timeslot The timeslot offset from the start of the day (not hours or seconds)
     * @return The index into the courses vector indicating which course is at that timeslot
     */
    static int getCourseIndexAtSlot(
        const Timetable& timetable,
        const std::vector<Course*>& courses,
        int day,
        int semester,
        int timeslot
    );

    /**
     * @brief Converts timeslots to hours
     * @param timeslots the number of timeslots to convert to hours
     * @return The number of hours
     */
    static double timeslotsToHours(uint32_t timeslots);
};