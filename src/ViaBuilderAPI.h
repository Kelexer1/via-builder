#include "SerializedCourse/Course.h"

#include <memory>
#include <vector>
#include <string>

class ViaBuilderAPI {
public:
    /**
     * @brief Updates the builders preference parameters
     * @param preferenceJSON A string representation of valid JSON containing all fields accessed by the function.
     * Consult config.h for details on a specific field
     */
    void setPreferences(const std::string& preferenceJSON);

    /**
     * @brief Updates the builders settings parameters
     * @param settingsJSON A string representation of valid JSON containing all the fields accessed by the function.
     * Consult config.json for details on a specific field
     */
    void setSettings(const std::string& settingsJSON);

    /**
     * @brief Adds a new course to the builder, does not generate a timetable automatically
     * @param courseJSON A string representation of valid course JSON, for example:
     * {
     *      "code": "CSC148H5",
     *      "campus": "University of Toronto at Mississauga",
     *      "sections": [
     *          {
     *              "name": "LEC0103",
     *              "meetingTimes": [
     *                 {
     *                     "start": 46800,
     *                     "end": 50400,
     *                     "day": 0,
     *                     "online": false,
     *                     "zz": false,
     *                     "semester": 1
     *                 },
     *                 {
     *                     "start": 68400,
     *                     "end": 75600,
     *                     "day": 0,
     *                     "online": false,
     *                     "zz": true,
     *                     "semester": 1
     *                 }
     *             ]
     *         },
     *     ],
     *     "type": "LEC"
     * }
     *
     **/
    void addCourse(const std::string& courseJSON);

    /**
     * @brief Removes all courses with names matching courseCode and matching type. For example, removing CSC148H5 LEC
     * @param courseName The course code
     * @param type The type of course, ex LEC, TUT, or PRA
     */
    void removeCourse(const std::string& courseCode, const std::string& type);
    std::string getAddedCourses();
    std::string buildTimetable();

private:
    std::vector<std::unique_ptr<Course>> courses; // All courses currently set to be placed into a timetable
};