#pragma once
#include <cstdint>
#include <string>
#include <vector>

struct PreferencesInput {
  uint32_t MAX_GAP;
  uint32_t MAX_DAY_LENGTH;
  uint32_t MIN_DAY_LENGTH;
  uint32_t MAX_CONTINUOUS_CLASSES;
  uint32_t PREFERRED_MIN_START;
  uint32_t PREFERRED_MAX_END;
  bool GUARANTEE_CROSS_CAMPUS_GAP;
  bool AVOID_RUSH_HOURS;
  uint32_t ONLINE_PREFERENCE;
};

struct SettingsInput {
  uint32_t MAX_GAP_PENALTY;
  double MAX_GAP_PENALTY_EXPONENT;
  uint32_t MAX_DAY_LENGTH_PENALTY;
  double MAX_DAY_LENGTH_PENALTY_EXPONENT;
  uint32_t MIN_DAY_LENGTH_PENALTY;
  double MIN_DAY_LENGTH_PENALTY_EXPONENT;
  uint32_t MAX_CONTINUOUS_CLASSES_PENALTY;
  double MAX_CONTINUOUS_CLASSES_PENALTY_EXPONENT;
  uint32_t PREFERRED_MIN_START_REWARD;
  uint32_t PREFERRED_MAX_END_REWARD;
};

struct MeetingTimeInput {
  size_t start;
  size_t end;
  int day;
  bool online;
  bool zz;
  int semester;
};

struct SectionInput {
  std::string name;
  std::vector<MeetingTimeInput> meetingTimes;
};

struct CourseInput {
  std::string code;
  std::string campus;
  std::string type;
  std::vector<SectionInput> sections;
};

struct CourseResult {
  std::string code;
  std::string type;
  std::string section; // empty for getAddedCourses(), populated for buildTimetable()
};