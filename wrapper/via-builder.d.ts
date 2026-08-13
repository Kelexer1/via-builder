export interface ViaBuilderPreferencesInput {
    maxGap: number
    maxDayLength: number
    minDayLength: number
    maxContinuousClasses: number
    preferredMinStart: number
    preferredMaxEnd: number
    guaranteeCrossCampusGap: boolean
    avoidRushHours: boolean
    onlinePreference: number
}

export interface ViaBuilderSettingsInput {
    maxGapPenalty: number
    maxGapPenaltyExponent: number
    maxDayLengthPenalty: number
    maxDayLengthPenaltyExponent: number
    minDayLengthPenalty: number
    minDayLengthPenaltyExponent: number
    maxContinuousClassesPenalty: number
    maxContinuousClassesPenaltyExponent: number
    preferredMinStartReward: number
    preferredMaxEndReward: number
}

export interface ViaBuilderMeetingTime {
    start: number
    end: number
    day: number
    online: boolean
    zz: boolean
    semester: number
}

export interface ViaBuilderSectionInput {
    name: string
    meetingTimes: ViaBuilderMeetingTime[]
}

export interface ViaBuilderCourseInput {
    code: string
    campus: string
    type: string
    sections: ViaBuilderSectionInput[]
}

export interface ViaBuilderCourseResult {
    code: string
    type: string
    section: string
}

export interface ViaBuilderApi {
    setPreferences(preferences: ViaBuilderPreferencesInput): void;
    setSettings(settings: ViaBuilderSettingsInput): void;
    addCourse(course: ViaBuilderCourseInput): void;
    removeCourse(courseCode: string, type: string): void;
    buildTimetable(): ViaBuilderCourseResult[];
    delete(): void;
}

export interface ViaBuilderModule {
    ViaBuilderAPI: new () => ViaBuilderApi;
}

export interface ViaBuilderModuleOptions {
    locateFile?: (path: string) => string;
}

declare function createViaBuilderModule(
    options?: ViaBuilderModuleOptions
): Promise<ViaBuilderModule>;

export default createViaBuilderModule;