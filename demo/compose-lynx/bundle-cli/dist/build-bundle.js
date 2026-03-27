"use strict";
var __importDefault = (this && this.__importDefault) || function (mod) {
    return (mod && mod.__esModule) ? mod : { "default": mod };
};
Object.defineProperty(exports, "__esModule", { value: true });
const fs_1 = require("fs");
const path_1 = __importDefault(require("path"));
const tasm_1 = require("@lynx-js/tasm");
const papi_1 = require("./papi");
function createAppServiceManifest(manifest) {
    const appServiceName = "/app-service.js";
    const appServiceBanner = "(function(){'use strict';function n({tt}){";
    const amdBanner = `tt.define('${appServiceName}',function(e,module,_,i,l,u,a,c,s,f,p,d,h,v,g,y,lynx){`;
    const amdFooter = `});return tt.require('${appServiceName}');`;
    const appServiceFooter = "}return{init:n}})()";
    const formatJsName = (name) => `/${name}`;
    return {
        [appServiceName]: [
            appServiceBanner,
            amdBanner,
            Object.keys(manifest)
                .map((name) => `module.exports=lynx.requireModule('${formatJsName(name)}',globDynamicComponentEntry?globDynamicComponentEntry:'__Card__')`)
                .join(","),
            amdFooter,
            appServiceFooter,
        ].join(""),
        ...Object.fromEntries(Object.entries(manifest).map(([name, source]) => [formatJsName(name), source])),
    };
}
function normalizeManifest(manifest) {
    if (manifest["/app-service.js"]) {
        return manifest;
    }
    return createAppServiceManifest(manifest);
}
function resolveEncodeOptions(parsed) {
    const compilerOptions = {
        ...(parsed.compilerOptions ?? {}),
    };
    if (compilerOptions.targetSdkVersion === undefined) {
        compilerOptions.targetSdkVersion = "3.2";
    }
    if (compilerOptions.enableSimpleStyling === undefined) {
        compilerOptions.enableSimpleStyling = true;
    }
    const css = parsed.css
        ? {
            ...parsed.css,
            chunks: undefined,
            contentMap: undefined,
        }
        : {
            chunks: undefined,
            contentMap: undefined,
        };
    const lepusCode = parsed.lepusCode
        ? {
            ...parsed.lepusCode,
            root: typeof parsed.lepusCode.root === "string" ? parsed.lepusCode.root : "",
            lepusChunk: parsed.lepusCode.lepusChunk && typeof parsed.lepusCode.lepusChunk === "object"
                ? parsed.lepusCode.lepusChunk
                : {},
        }
        : {
            root: "",
            lepusChunk: {},
        };
    return {
        compilerOptions,
        sourceContent: parsed.sourceContent,
        css,
        lepusCode,
        manifest: normalizeManifest(parsed.manifest),
        customSections: parsed.customSections ?? {},
        elementTemplate: parsed.elementTemplate ?? {},
    };
}
function argValue(name) {
    const index = process.argv.indexOf(name);
    if (index < 0 || index + 1 >= process.argv.length) {
        return undefined;
    }
    return process.argv[index + 1];
}
async function main() {
    const inputPath = argValue("--input");
    const outputPath = argValue("--out");
    if (!inputPath || !outputPath) {
        throw new Error("Usage: node dist/build-bundle.js --input <encode.json> --out <bundle>");
    }
    const absoluteInput = path_1.default.resolve(process.cwd(), inputPath);
    const absoluteOutput = path_1.default.resolve(process.cwd(), outputPath);
    const parsed = JSON.parse(await fs_1.promises.readFile(absoluteInput, "utf8"));
    const entryName = Object.keys(parsed.manifest)[0]?.replace(/\.js$/, "") ?? "ComposeDemo";
    (0, papi_1.createGeneratedPagePapi)(entryName);
    const encodeOptions = resolveEncodeOptions(parsed);
    const encode = (0, tasm_1.getEncodeMode)("napi");
    const result = await encode(encodeOptions);
    await fs_1.promises.mkdir(path_1.default.dirname(absoluteOutput), { recursive: true });
    await fs_1.promises.writeFile(absoluteOutput, result.buffer);
    process.stdout.write(`Wrote bundle to ${absoluteOutput}\n`);
}
main().catch((error) => {
    process.stderr.write(`${error instanceof Error ? error.stack : String(error)}\n`);
    process.exitCode = 1;
});
