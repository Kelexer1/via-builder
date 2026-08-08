export interface ViaBuilderApi {
    setPreferences(preferences: string): void;
    setSettings(settings: string): void;
    addCourse(course: string): void;
    removeCourse(courseCode: string, type: string): void;
    buildTimetable(): string | null | undefined;
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