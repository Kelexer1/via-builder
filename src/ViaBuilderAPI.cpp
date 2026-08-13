#include "ViaBuilderAPI.h"
#include "SerializedCourse/Course.h"
#include "SerializedCourse/Section.h"
#include "SerializedCourse/Timetable.h"
#include "TimetableBuilder.h"
#include "ViaBuilderAPITypes.h"
#include "config.h"

#include <algorithm>
#include <memory>
#include <string>

void ViaBuilderAPI::setPreferences(const PreferencesInput& preferences) {
  MAX_GAP = preferences.MAX_GAP;
  MAX_DAY_LENGTH = preferences.MAX_DAY_LENGTH;
  MIN_DAY_LENGTH = preferences.MIN_DAY_LENGTH;
  MAX_CONTINUOUS_CLASSES = preferences.MAX_CONTINUOUS_CLASSES;
  PREFERRED_MIN_START = preferences.PREFERRED_MIN_START;
  PREFERRED_MAX_END = preferences.PREFERRED_MAX_END;
  GUARANTEE_CROSS_CAMPUS_GAP = preferences.GUARANTEE_CROSS_CAMPUS_GAP;
  AVOID_RUSH_HOURS = preferences.AVOID_RUSH_HOURS;
  ONLINE_PREFERENCE = preferences.ONLINE_PREFERENCE;
}

void ViaBuilderAPI::setSettings(const SettingsInput& settings) {
  MAX_GAP_PENALTY = settings.MAX_GAP_PENALTY;
  MAX_GAP_PENALTY_EXPONENT = settings.MAX_GAP_PENALTY_EXPONENT;
  MAX_DAY_LENGTH_PENALTY = settings.MAX_DAY_LENGTH_PENALTY;
  MAX_DAY_LENGTH_PENALTY_EXPONENT = settings.MAX_DAY_LENGTH_PENALTY_EXPONENT;
  MIN_DAY_LENGTH_PENALTY = settings.MIN_DAY_LENGTH_PENALTY;
  MIN_DAY_LENGTH_PENALTY_EXPONENT = settings.MIN_DAY_LENGTH_PENALTY_EXPONENT;
  MAX_CONTINUOUS_CLASSES_PENALTY = settings.MAX_CONTINUOUS_CLASSES_PENALTY;
  MAX_CONTINUOUS_CLASSES_PENALTY_EXPONENT = settings.MAX_CONTINUOUS_CLASSES_PENALTY_EXPONENT;
  PREFERRED_MIN_START_REWARD = settings.PREFERRED_MIN_START_REWARD;
  PREFERRED_MAX_END_REWARD = settings.PREFERRED_MAX_END_REWARD;
}

void ViaBuilderAPI::addCourse(const CourseInput& course) {
  auto courseData = std::make_unique<Course>(course.code, course.campus, course.type);

  for (const auto& section : course.sections) {
    Section* sectionData = new Section(section.name);

    for (const auto& meetingTimeData : section.meetingTimes) {
      sectionData->addMeetingTime(meetingTimeData.start, meetingTimeData.end, meetingTimeData.day,
                                  meetingTimeData.online, meetingTimeData.zz, meetingTimeData.semester);
    }

    courseData->sections.push_back(sectionData);
  }

  this->courses.push_back(std::move(courseData));
}

void ViaBuilderAPI::removeCourse(const std::string& courseCode, const std::string& type) {
  this->courses.erase(std::remove_if(this->courses.begin(), this->courses.end(),
                                     [&courseCode, &type](const std::unique_ptr<Course>& course) {
                                       return course->code == courseCode && course->type == type;
                                     }),
                      this->courses.end());
}

std::vector<CourseResult> ViaBuilderAPI::getAddedCourses() {
  std::vector<CourseResult> result;
  for (const auto& course : this->courses) {
    const CourseResult entry = {.code = course->code, .type = course->type, .section = ""};
    result.push_back(entry);
  }

  return result;
}

std::vector<CourseResult> ViaBuilderAPI::buildTimetable() {
  std::vector<CourseResult> result;

  std::vector<Course*> coursePtrs;
  for (const auto& courseUPtr : this->courses) {
    coursePtrs.push_back(courseUPtr.get());
  }

  if (coursePtrs.empty())
    return result;

  auto timetableOpt = TimetableBuilder::buildTimetable(coursePtrs);
  if (!timetableOpt)
    return result;

  const Timetable& timetable = timetableOpt.value();
  for (size_t i = 0; i < coursePtrs.size(); i++) {
    const CourseResult entry = {.code = coursePtrs[i]->code,
                                .type = coursePtrs[i]->type,
                                .section = coursePtrs[i]->sections[timetable.chosenSections[i]]->name};
    result.push_back(entry);
  }

  return result;
}