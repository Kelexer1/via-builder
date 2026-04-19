#include "ViaBuilderAPI.h"
#include "SerializedCourse/Timetable.h"
#include "TimetableBuilder.h"
#include "config.h"
#include "JSON/json.hpp"

#include <memory>
#include <string>

using json = nlohmann::json;

void ViaBuilderAPI::setPreferences(const std::string& preferenceJSON) {
    json preferences = json::parse(preferenceJSON);

    MAX_GAP = preferences["MAX_GAP"];
    MAX_DAY_LENGTH = preferences["MAX_DAY_LENGTH"];
    MIN_DAY_LENGTH = preferences["MIN_DAY_LENGTH"];
    MAX_CONTINUOUS_CLASSES = preferences["MAX_CONTINUOUS_CLASSES"];
    PREFFERED_MIN_START = preferences["PREFFERED_MIN_START"];
    PREFFERED_MAX_END = preferences["PREFFERED_MAX_END"];
    GUARANTEE_CROSS_CAMPUS_GAP = preferences["GUARANTEE_CROSS_CAMPUS_GAP"];
    AVOID_RUSH_HOURS = preferences["AVOID_RUSH_HOURS"];
    ONLINE_PREFERENCE = preferences["ONLINE_PREFERENCE"];
}

void ViaBuilderAPI::setSettings(const std::string& settingsJSON) {
    json settings = json::parse(settingsJSON);

    MAX_GAP_PENALTY = settings["MAX_GAP_PENALTY"];
    MAX_GAP_PENALTY_EXPONENT = settings["MAX_GAP_PENALTY_EXPONENT"];
    MAX_DAY_LENGTH_PENALTY = settings["MAX_DAY_LENGTH_PENALTY"];
    MAX_DAY_LENGTH_PENALTY_EXPONENT = settings["MAX_DAY_LENGTH_PENALTY_EXPONENT"];
    MIN_DAY_LENGTH_PENALTY = settings["MIN_DAY_LENGTH_PENALTY"];
    MIN_DAY_LENGTH_PENALTY_EXPONENT = settings["MIN_DAY_LENGTH_PENALTY_EXPONENT"];
    MAX_CONTINUOUS_CLASSES_PENALTY = settings["MAX_CONTINUOUS_CLASSES_PENALTY"];
    MAX_CONTINUOUS_CLASSES_PENALTY_EXPONENT = settings["MAX_CONTINUOUS_CLASSES_PENALTY_EXPONENT"];
    PREFFERED_MIN_START_REWARD = settings["PREFFERED_MIN_START_REWARD"];
    PREFFERED_MAX_END_REWARD = settings["PREFFERED_MAX_END_REWARD"];
}

void ViaBuilderAPI::addCourse(const std::string& courseJSON) {
    json courseParsed = json::parse(courseJSON);
    auto course = std::make_unique<Course>(std::string(courseParsed["code"]), std::string(courseParsed["campus"]), std::string(courseParsed["type"]));

    for (const auto& sectionJSON : courseParsed["sections"]) {
        Section* section = new Section(std::string(sectionJSON["name"]));

        for (const auto& meetingTimeJSON : sectionJSON["meetingTimes"]) {
            section->addMeetingTime(meetingTimeJSON["start"], meetingTimeJSON["end"], meetingTimeJSON["day"], meetingTimeJSON["online"], meetingTimeJSON["zz"], meetingTimeJSON["semester"]);
        }

        course->sections.push_back(section);
    }

    this->courses.push_back(std::move(course));
}

void ViaBuilderAPI::removeCourse(const std::string& courseCode, const std::string& type) {
    this->courses.erase(
        std::remove_if(this->courses.begin(), this->courses.end(),
        [&courseCode, &type](const std::unique_ptr<Course>& course) {
            return course->code == courseCode && course->type == type;
        }),
        this->courses.end()
    );
}

std::string ViaBuilderAPI::getAddedCourses() {
    json result = json::array();

    for (const auto& course : this->courses) {
        json courseJSON;
        courseJSON["code"] = course->code;
        courseJSON["type"] = course->type;
        result.push_back(courseJSON);
    }

    return result.dump();
}

std::string ViaBuilderAPI::buildTimetable() {
    json result = json::array();

    std::vector<Course*> coursePtrs;
    for (const auto& courseUPtr : this->courses) {
        coursePtrs.push_back(courseUPtr.get());
    }

    if (coursePtrs.empty()) {
        return result.dump();
    }

    auto timetableOpt = TimetableBuilder::buildTimetable(coursePtrs);
    if (!timetableOpt) {
        return result.dump();
    }

    const Timetable& timetable = timetableOpt.value();
    for (size_t i = 0; i < coursePtrs.size(); i++) {
        json entry;
        entry["code"] = coursePtrs[i]->code;
        entry["type"] = coursePtrs[i]->type;
        entry["section"] = coursePtrs[i]->sections[timetable.chosenSections[i]]->name;
        result.push_back(entry);
    }

    return result.dump();
}