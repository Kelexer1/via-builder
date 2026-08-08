import createViaBuilderModule, { ViaBuilderModule, ViaBuilderApi } from './via-builder.js';

type JsonObject = Record<string, unknown>;

let modulePromise: Promise<ViaBuilderModule> | null = null;

async function getModule(): Promise<ViaBuilderModule> {
    if (!modulePromise) {
        modulePromise = createViaBuilderModule() as Promise<ViaBuilderModule>;
    }
    return modulePromise;
}

export class ViaBuilderManager {
    private readonly Module: ViaBuilderModule;
    private api: ViaBuilderApi;
    private preferences?: JsonObject;
    private settings?: JsonObject;

    private constructor(Module: ViaBuilderModule) {
        this.Module = Module;
        this.api = new Module.ViaBuilderAPI();
    }

    static async create(): Promise<ViaBuilderManager> {
        const Module = await getModule();
        return new ViaBuilderManager(Module);
    }

    setPreferences(preferences: JsonObject): void {
        this.preferences = preferences;
        this.api.setPreferences(JSON.stringify(preferences));
    }

    setSettings(settings: JsonObject): void {
        this.settings = settings;
        this.api.setSettings(JSON.stringify(settings));
    }

    addCourse(course: JsonObject): void {
        this.api.addCourse(JSON.stringify(course));
    }

    removeCourse(courseCode: string, type: string): void {
        this.api.removeCourse(courseCode, type);
    }

    build(): unknown {
        const raw = this.api.buildTimetable();
        return raw ? JSON.parse(raw) : {};
    }

    reset(): void {
        this.api.delete();
        this.api = new this.Module.ViaBuilderAPI();
        if (this.preferences) this.api.setPreferences(JSON.stringify(this.preferences));
        if (this.settings) this.api.setSettings(JSON.stringify(this.settings));
    }

    dispose(): void {
        this.api.delete();
    }
}