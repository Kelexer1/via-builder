#include "SerializedCourse/Course.h"
#include "ViaBuilderAPITypes.h"

#include <memory>
#include <string>
#include <vector>

class ViaBuilderAPI {
public:
  /**
   * @brief Updates the builders preference parameters
   * @param preferences The preferences data
   */
  void setPreferences(const PreferencesInput& preferences);

  /**
   * @brief Updates the builders settings parameters
   * @param settings The settings data
   */
  void setSettings(const SettingsInput& settings);

  /**
   * @brief Adds a new course to the builder, does not generate a timetable automatically
   * @param course The course data
   */
  void addCourse(const CourseInput& course);

  /**
   * @brief Removes all courses with names matching courseCode and matching type. For example, removing CSC148H5 LEC
   * @param courseName The course code
   * @param type The type of course, ex LEC, TUT, or PRA
   */
  void removeCourse(const std::string& courseCode, const std::string& type);

  std::vector<CourseResult> getAddedCourses();

  std::vector<CourseResult> buildTimetable();

private:
  std::vector<std::unique_ptr<Course>> courses; // All courses currently set to be placed into a timetable
};