#pragma once
#include <string>
#include <vector>

#include "Section.h"

/**
 * @brief Represents a single course that must be scheduled when building a timetable. Even though this represents a single course,
 * different courses should be used for lectures, tutorials, and practicals to ensure they are all considered when building
 */
class Course {
public:
    Course(
        const std::string& code,
        const std::string& campus,
        const std::string& type
    );

    std::string code;               // The course code (ex. "CSC108H5")
    std::string campus;             // The course campus (ex. "University of Toronto at Mississauga")
    std::string type;               // The type of section this is (ex. "LEC", "TUT", or "PRA")
    std::vector<Section*> sections; // The different sections a student can enrol in for this course
};