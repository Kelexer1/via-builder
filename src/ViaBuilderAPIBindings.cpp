#ifdef __EMSCRIPTEN__
#include "ViaBuilderAPI.h"
#include "ViaBuilderAPITypes.h"
#include <emscripten/bind.h>

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

  value_object<MeetingTimeInput>("MeetingTimeInput")
      .field("start", &MeetingTimeInput::start)
      .field("end", &MeetingTimeInput::end)
      .field("day", &MeetingTimeInput::day)
      .field("online", &MeetingTimeInput::online)
      .field("zz", &MeetingTimeInput::zz)
      .field("semester", &MeetingTimeInput::semester);
  register_vector<MeetingTimeInput>("MeetingTimeInputVector");

  value_object<SectionInput>("SectionInput")
      .field("name", &SectionInput::name)
      .field("meetingTimes", &SectionInput::meetingTimes);
  register_vector<SectionInput>("SectionInputVector");

  value_object<CourseInput>("CourseInput")
      .field("code", &CourseInput::code)
      .field("campus", &CourseInput::campus)
      .field("type", &CourseInput::type)
      .field("sections", &CourseInput::sections);

  class_<ViaBuilderAPI>("ViaBuilderAPI")
      .constructor<>()
      .function("setPreferences", &ViaBuilderAPI::setPreferences)
      .function("setSettings", &ViaBuilderAPI::setSettings)
      .function("addCourse", &ViaBuilderAPI::addCourse)
      .function("removeCourse", &ViaBuilderAPI::removeCourse)
      .function("getAddedCourses",
                optional_override([](ViaBuilderAPI& self) { return toResultArray(self.getAddedCourses()); }))
      .function("buildTimetable",
                optional_override([](ViaBuilderAPI& self) { return toResultArray(self.buildTimetable()); }));
}
#endif
