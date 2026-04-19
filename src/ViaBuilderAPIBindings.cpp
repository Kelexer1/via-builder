#ifdef __EMSCRIPTEN__
#include <emscripten/bind.h>
#include "ViaBuilderAPI.h"

using namespace emscripten;

EMSCRIPTEN_BINDINGS(builder_api_module) {
    class_<ViaBuilderAPI>("ViaBuilderAPI")
        .constructor<>()
        .function("setPreferences", &ViaBuilderAPI::setPreferences)
        .function("setSettings", &ViaBuilderAPI::setSettings)
        .function("addCourse", &ViaBuilderAPI::addCourse)
        .function("removeCourse", &ViaBuilderAPI::removeCourse)
        .function("buildTimetable", &ViaBuilderAPI::buildTimetable)
        .function("getAddedCourses", &ViaBuilderAPI::getAddedCourses)
        ;
}
#endif
