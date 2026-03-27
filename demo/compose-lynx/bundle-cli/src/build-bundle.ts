import { promises as fs } from "fs";
import path from "path";
import { getEncodeMode } from "@lynx-js/tasm";
import { createGeneratedPagePapi } from "./papi";

interface EncodeInput {
  compilerOptions?: Record<string, unknown>;
  manifest: Record<string, string>;
  sourceContent: Record<string, unknown>;
  css?: Record<string, unknown>;
  lepusCode?: {
    root?: string;
    lepusChunk?: Record<string, string>;
    chunks?: unknown;
    [key: string]: unknown;
  };
  customSections?: Record<string, unknown>;
  elementTemplate?: Record<string, unknown>;
}

function createAppServiceManifest(manifest: Record<string, string>): Record<string, string> {
  const appServiceName = "/app-service.js";
  const appServiceBanner = "(function(){'use strict';function n({tt}){";
  const amdBanner = `tt.define('${appServiceName}',function(e,module,_,i,l,u,a,c,s,f,p,d,h,v,g,y,lynx){`;
  const amdFooter = `});return tt.require('${appServiceName}');`;
  const appServiceFooter = "}return{init:n}})()";
  const formatJsName = (name: string) => `/${name}`;

  return {
    [appServiceName]: [
      appServiceBanner,
      amdBanner,
      Object.keys(manifest)
        .map(
          (name) =>
            `module.exports=lynx.requireModule('${formatJsName(name)}',globDynamicComponentEntry?globDynamicComponentEntry:'__Card__')`,
        )
        .join(","),
      amdFooter,
      appServiceFooter,
    ].join(""),
    ...Object.fromEntries(Object.entries(manifest).map(([name, source]) => [formatJsName(name), source])),
  };
}

function normalizeManifest(manifest: Record<string, string>): Record<string, string> {
  if (manifest["/app-service.js"]) {
    return manifest;
  }
  return createAppServiceManifest(manifest);
}

function resolveEncodeOptions(parsed: EncodeInput) {
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
        lepusChunk:
          parsed.lepusCode.lepusChunk && typeof parsed.lepusCode.lepusChunk === "object"
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

function argValue(name: string): string | undefined {
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

  const absoluteInput = path.resolve(process.cwd(), inputPath);
  const absoluteOutput = path.resolve(process.cwd(), outputPath);

  const parsed = JSON.parse(await fs.readFile(absoluteInput, "utf8")) as EncodeInput;
  const entryName = Object.keys(parsed.manifest)[0]?.replace(/\.js$/, "") ?? "ComposeDemo";
  createGeneratedPagePapi(entryName);
  const encodeOptions = resolveEncodeOptions(parsed);

  const encode = getEncodeMode("napi");
  const result = await encode(encodeOptions);

  await fs.mkdir(path.dirname(absoluteOutput), { recursive: true });
  await fs.writeFile(absoluteOutput, result.buffer);
  process.stdout.write(`Wrote bundle to ${absoluteOutput}\n`);
}

main().catch((error) => {
  process.stderr.write(`${error instanceof Error ? error.stack : String(error)}\n`);
  process.exitCode = 1;
});
