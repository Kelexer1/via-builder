#include "ViaBuilderAPI.h"
#include "JSON/json.hpp"

#include <iostream>
#include <iomanip>
#include <string>
#include <unordered_map>
#include <vector>
#include <fstream>
#include <chrono>

using json = nlohmann::json;

/**
 * @brief This function is mainly used for testing / demonstrating a semi-normal usage of the API. If you want to
 * experiment with it, follow the steps below to configure it:
 * 1. Add courses.json to the project root, which can be generated using https://github.com/Kelexer1/UofT-Scraper
 * 2. Modify all the variables in the commented block as required
 */
int main() {
    //===== MODIFY THESE VARIABLES TO SUIT YOUR NEEDS
    // Note that these values are assumed to be formatted correctly since the actual code does not have extensive
    // error checking. Moreover the code itself isn't heavily optimized since, again, it's mainly for testing

    // Mapping for semesters to their indexes, does not necessarily need to be in chronological order
    const std::unordered_map<std::string, int> semesterBySession = {
        { "20259", 0 },
        { "20261", 1 }
    };

    // Which sessions to scan for the course codes
    const std::vector<std::string> sessionBuckets = { "20261", "20259", "20259-20261" };

    // Which courses to look for, as well as their session code
    const std::vector<std::pair<std::string, std::string>> selectedCourses = {
        { "CSC263H5", "S" },
        { "CSC209H5", "S" },
        { "MAT224H5", "S" },
        { "PHL245H5", "S" },
        { "MAT232H5", "S" }
    };
    //=====


    // Parse the courses.json file
    auto startParseJSON = std::chrono::steady_clock::now();

    std::ifstream file("courses.json");
    if (!file.is_open()) {
        std::cerr << "Courses JSON failed to open\n";
        return -1;
    }

    json input;
    file >> input;

    ViaBuilderAPI api;
    std::unordered_map<std::string, json> sectionMeetingTimesByChoice;
    std::vector<const json*> selectedCourseData;

    std::cout << "Started searching for courses...\n";
    uint32_t coursesSearched = 0;

    // Scan the JSON for the raw serialized data for all the courses listed in selectedCourses
    for (const auto& [courseCode, sectionCode] : selectedCourses) {
        const json* found = nullptr;
        for (const std::string& bucket : sessionBuckets) {
            if (!input.contains(bucket)) continue;
            for (const auto& courseData : input.at(bucket)) {
                if (courseCode == std::string(courseData["code"]) && sectionCode == std::string(courseData["sectionCode"])) {
                    found = &courseData;
                    break;
                }
            }
            if (found) break;
        }
        if (!found) continue;
        coursesSearched++;
        selectedCourseData.push_back(found);
    }

    // Parse the JSON into courses, sessions, and meeting times
    for (const json* courseData : selectedCourseData) {
        json lecturesJSON, tutorialsJSON, practicalsJSON;

        // Set up course metadata for LEC, TUT, PRA (since they are treated as separate courses)
        for (auto* courseJSON : { &lecturesJSON, &tutorialsJSON, &practicalsJSON }) {
            (*courseJSON)["code"] = (*courseData)["code"];
            (*courseJSON)["campus"] = (*courseData)["campus"];
            (*courseJSON)["sections"] = json::array();
        }

        // Set up sections and assign to correct course
        for (const auto& sectionData : (*courseData)["sections"]) {
            const std::string sectionType = sectionData["type"];
            const std::string typeCode =
                sectionType == "Lecture" ? "LEC" :
                sectionType == "Tutorial" ? "TUT" :
                sectionType == "Practical" ? "PRA" : "";
            if (typeCode.empty()) continue;

            json sectionJSON;
            sectionJSON["name"] = sectionData["name"];
            sectionJSON["meetingTimes"] = json::array();
            json normalizedMeetingTimes = json::array();

            // Set up meeting times and assign to current section
            bool hasMeetingTime = false;
            for (const auto& meetingTimeData : sectionData["meetingTimes"]) {
                auto semIt = semesterBySession.find(std::string(meetingTimeData["sessionCode"]));
                if (semIt == semesterBySession.end()) continue;

                const int normalizedSemester = semIt->second;

                json mtJSON;
                mtJSON["start"] = meetingTimeData["start"];
                mtJSON["end"] = meetingTimeData["end"];
                mtJSON["day"] = meetingTimeData["day"];
                mtJSON["online"] = (meetingTimeData["building"]["buildingCode"] == "");
                mtJSON["zz"] = (meetingTimeData["building"]["buildingCode"] == "ZZ");
                mtJSON["semester"] = normalizedSemester;
                sectionJSON["meetingTimes"].push_back(mtJSON);

                json normalizedMeetingTime;
                normalizedMeetingTime["semester"] = normalizedSemester;
                normalizedMeetingTime["day"] = meetingTimeData["day"];
                normalizedMeetingTime["start"] = meetingTimeData["start"];
                normalizedMeetingTime["end"] = meetingTimeData["end"];
                normalizedMeetingTime["buildingCode"] = meetingTimeData["building"]["buildingCode"];
                normalizedMeetingTimes.push_back(normalizedMeetingTime);
                hasMeetingTime = true;
            }

            if (!hasMeetingTime) continue;

            // Cache the meeting times to easily fetch them after generating the timetable to print
            sectionMeetingTimesByChoice[std::string((*courseData)["code"]) + "|" + typeCode + "|" + std::string(sectionData["name"])] = normalizedMeetingTimes;

            if (typeCode == "LEC") lecturesJSON["sections"].push_back(sectionJSON);
            else if (typeCode == "TUT") tutorialsJSON["sections"].push_back(sectionJSON);
            else if (typeCode == "PRA") practicalsJSON["sections"].push_back(sectionJSON);
        }

        if (!lecturesJSON["sections"].empty()) {
            lecturesJSON["type"] = "LEC";
            api.addCourse(lecturesJSON.dump());
        }
        if (!tutorialsJSON["sections"].empty()) {
            tutorialsJSON["type"] = "TUT";
            api.addCourse(tutorialsJSON.dump());
        }
        if (!practicalsJSON["sections"].empty()) {
            practicalsJSON["type"] = "PRA";
            api.addCourse(practicalsJSON.dump());
        }
    }

    auto endParseJSON = std::chrono::steady_clock::now();
    auto durationParseJSON = std::chrono::duration<double>(endParseJSON - startParseJSON);

    std::cout << "Finished searching " << coursesSearched << " courses in " << durationParseJSON << " seconds...\n\n";

    auto startBuild = std::chrono::steady_clock::now();

    // All the heavy calculation is done here
    std::string resultStr = api.buildTimetable();

    auto endBuild = std::chrono::steady_clock::now();
    auto durationBuild = std::chrono::duration<double>(endBuild - startBuild);

    std::cout << "Finished building timetable in " << durationBuild << " seconds...\n";

    std::cout << "\n===== Results =====\n\n";

    json result = json::parse(resultStr);

    if (result.empty()) {
        std::cout << "No timetable was found\n";
        return 0;
    }

    // Pretty print the resulting timetable
    for (const auto& entry : result) {
        std::string code = entry["code"];
        std::string type = entry["type"];
        std::string sectionName = entry["section"];
        std::cout << "\nCourse: " << code << " - Section: " << sectionName << "\n";
        std::cout << "Meeting Times:\n";

        auto lookupIt = sectionMeetingTimesByChoice.find(code + "|" + type + "|" + sectionName);
        if (lookupIt == sectionMeetingTimesByChoice.end()) {
            continue;
        }

        for (const auto& meetingTimeData : lookupIt->second) {
            std::cout << "  Semester: " << (int)meetingTimeData["semester"]
                      << "  Day: " << meetingTimeData["day"]
                      << ", Start: " << (int)meetingTimeData["start"] / 3600 << ":"
                      << std::setw(2) << std::setfill('0') << ((int)meetingTimeData["start"] % 3600) / 60
                      << ", End: " << (int)meetingTimeData["end"] / 3600 << ":"
                      << std::setw(2) << std::setfill('0') << ((int)meetingTimeData["end"] % 3600) / 60
                      << ", Building: " << meetingTimeData["buildingCode"]
                      << "\n";
        }
    }

    return 0;
}