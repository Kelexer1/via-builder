#ifdef __EMSCRIPTEN__
#include "ViaBuilderAPI.h"
#include "ViaBuilderAPITypes.h"
#include <emscripten/bind.h>
#include <emscripten/val.h>

using namespace emscripten;

namespace {
val toResultArray(const std::vector<CourseResult>& results) {
  val arr = val::array();
  for (const auto& r : results) {
    val obj = val::object();
    obj.set("code", r.code);
    obj.set("type", r.type);
    obj.set("section", r.section);
    arr.call<void>("push", obj);
  }
  return arr;
}

MeetingTimeInput meetingTimeInputFromVal(const val& v) {
  MeetingTimeInput out;
  out.start = v["start"].as<size_t>();
  out.end = v["end"].as<size_t>();
  out.day = v["day"].as<int>();
  out.online = v["online"].as<bool>();
  out.zz = v["zz"].as<bool>();
  out.semester = v["semester"].as<int>();
  return out;
}

SectionInput sectionInputFromVal(const val& v) {
  SectionInput out;
  out.name = v["name"].as<std::string>();

  val meetingTimes = v["meetingTimes"];
  unsigned length = meetingTimes["length"].as<unsigned>();
  out.meetingTimes.reserve(length);
  for (unsigned i = 0; i < length; i++) {
    out.meetingTimes.push_back(meetingTimeInputFromVal(meetingTimes[i]));
  }
  return out;
}

CourseInput courseInputFromVal(const val& v) {
  CourseInput out;
  out.code = v["code"].as<std::string>();
  out.campus = v["campus"].as<std::string>();
  out.type = v["type"].as<std::string>();

  val sections = v["sections"];
  unsigned length = sections["length"].as<unsigned>();
  out.sections.reserve(length);
  for (unsigned i = 0; i < length; i++) {
    out.sections.push_back(sectionInputFromVal(sections[i]));
  }
  return out;
}
} // namespace

EMSCRIPTEN_BINDINGS(via_builder) {
  value_object<PreferencesInput>("PreferencesInput")
      .field("maxGap", &PreferencesInput::MAX_GAP)
      .field("maxDayLength", &PreferencesInput::MAX_DAY_LENGTH)
      .field("minDayLength", &PreferencesInput::MIN_DAY_LENGTH)
      .field("maxContinuousClasses", &PreferencesInput::MAX_CONTINUOUS_CLASSES)
      .field("preferredMinStart", &PreferencesInput::PREFERRED_MIN_START)
      .field("preferredMaxEnd", &PreferencesInput::PREFERRED_MAX_END)
      .field("guaranteeCrossCampusGap", &PreferencesInput::GUARANTEE_CROSS_CAMPUS_GAP)
      .field("avoidRushHours", &PreferencesInput::AVOID_RUSH_HOURS)
      .field("onlinePreference", &PreferencesInput::ONLINE_PREFERENCE);

  value_object<SettingsInput>("SettingsInput")
      .field("maxGapPenalty", &SettingsInput::MAX_GAP_PENALTY)
      .field("maxGapPenaltyExponent", &SettingsInput::MAX_GAP_PENALTY_EXPONENT)
      .field("maxDayLengthPenalty", &SettingsInput::MAX_DAY_LENGTH_PENALTY)
      .field("maxDayLengthPenaltyExponent", &SettingsInput::MAX_DAY_LENGTH_PENALTY_EXPONENT)
      .field("minDayLengthPenalty", &SettingsInput::MIN_DAY_LENGTH_PENALTY)
      .field("minDayLengthPenaltyExponent", &SettingsInput::MIN_DAY_LENGTH_PENALTY_EXPONENT)
      .field("maxContinuousClassesPenalty", &SettingsInput::MAX_CONTINUOUS_CLASSES_PENALTY)
      .field("maxContinuousClassesPenaltyExponent", &SettingsInput::MAX_CONTINUOUS_CLASSES_PENALTY_EXPONENT)
      .field("preferredMinStartReward", &SettingsInput::PREFERRED_MIN_START_REWARD)
      .field("preferredMaxEndReward", &SettingsInput::PREFERRED_MAX_END_REWARD);

  class_<ViaBuilderAPI>("ViaBuilderAPI")
      .constructor<>()
      .function("setPreferences", &ViaBuilderAPI::setPreferences)
      .function("setSettings", &ViaBuilderAPI::setSettings)
      .function("addCourse",
                optional_override([](ViaBuilderAPI& self, val course) { self.addCourse(courseInputFromVal(course)); }))
      .function("removeCourse", &ViaBuilderAPI::removeCourse)
      .function("getAddedCourses",
                optional_override([](ViaBuilderAPI& self) { return toResultArray(self.getAddedCourses()); }))
      .function("buildTimetable",
                optional_override([](ViaBuilderAPI& self) { return toResultArray(self.buildTimetable()); }));
}
#endif