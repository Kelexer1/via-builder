import createViaBuilderModule, {
    ViaBuilderModule,
    ViaBuilderApi,
    ViaBuilderPreferencesInput,
    ViaBuilderSettingsInput,
    ViaBuilderCourseInput,
    ViaBuilderCourseResult,
} from './via-builder.js';

let modulePromise: Promise<ViaBuilderModule> | null = null;

async function getModule(): Promise<ViaBuilderModule> {
    if (!modulePromise) {
        modulePromise = createViaBuilderModule();
    }
    return modulePromise;
}

export class ViaBuilderManager {
    private readonly Module: ViaBuilderModule;
    private api: ViaBuilderApi;
    private preferences?: ViaBuilderPreferencesInput;
    private settings?: ViaBuilderSettingsInput;

    private constructor(Module: ViaBuilderModule) {
        this.Module = Module;
        this.api = new Module.ViaBuilderAPI();
    }

    static async create(): Promise<ViaBuilderManager> {
        const Module = await getModule();
        return new ViaBuilderManager(Module);
    }

    setPreferences(preferences: ViaBuilderPreferencesInput): void {
        this.preferences = preferences;
        this.api.setPreferences(preferences);
    }

    setSettings(settings: ViaBuilderSettingsInput): void {
        this.settings = settings;
        this.api.setSettings(settings);
    }

    addCourse(course: ViaBuilderCourseInput): void {
        this.api.addCourse(course);
    }

    removeCourse(courseCode: string, type: string): void {
        this.api.removeCourse(courseCode, type);
    }

    build(): ViaBuilderCourseResult[] {
        return this.api.buildTimetable();
    }

    reset(): void {
        this.api.delete();
        this.api = new this.Module.ViaBuilderAPI();
        if (this.preferences) this.api.setPreferences(this.preferences);
        if (this.settings) this.api.setSettings(this.settings);
    }

    dispose(): void {
        this.api.delete();
    }
}