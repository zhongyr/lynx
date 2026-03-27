# Compose-to-Lynx Demo

This demo adds a minimal Compose-style DSL that is compiled by a real Kotlin IR compiler plugin into Lynx artifacts, then packed into a Lynx template bundle with `@lynx-js/tasm`.

The supported surface is intentionally small:

- `@Compose`
- `Row`
- `Column`
- `Text`
- `Modifier.backgroundColor(...)`

## Layout

- `runtime`: minimal Compose-style Kotlin API consumed by demo sources
- `compiler-plugin`: Kotlin IR compiler plugin that emits Lynx artifacts
- `sample`: demo Compose DSL source and Gradle task that runs the compiler with the plugin
- `bundle-cli`: TypeScript CLI that turns generated encode JSON into `.lynx.bundle`
- `android-host`: minimal Android host app that loads the generated bundle with `LynxView`

## Generated Artifacts

Running the sample compiler task emits files under `sample/build/generated/lynx`:

- `DemoPage.lynx.js`
- `DemoPage.main-thread.js`
- `DemoPage.ttml.json`
- `DemoPage.encode.json`

`DemoPage.main-thread.js` is the readable main-thread source that is copied into `lepusCode.root` inside `DemoPage.encode.json`.

`bundle-cli` consumes `DemoPage.encode.json` and writes `compose-demo.lynx.bundle`.

## Build Flow

1. Compile the sample with the Kotlin compiler plugin:

```bash
./platform/android/gradlew -p demo/compose-lynx :sample:compileComposeSample
```

2. Install bundle CLI dependencies:

```bash
cd demo/compose-lynx/bundle-cli && npm install
```

3. Build the Lynx bundle and copy it into the Android host assets:

```bash
cd demo/compose-lynx/bundle-cli && npm run bundle -- \
  --input ../sample/build/generated/lynx/DemoPage.encode.json \
  --out ../android-host/app/src/main/assets/compose-demo.lynx.bundle
```

4. Build the Android host app:

```bash
./platform/android/gradlew -p demo/compose-lynx/android-host :app:assembleDebug
```

## Notes

- The compiler plugin emits a minimal static TTML tree plus a tiny Lynx JS stub.
- `bundle-cli` enables `compileOptions.enableSimpleStyling` so `background-color` is encoded through Lynx simple styling.
- The Android host reads a local bundle from assets and loads it with `LynxLoadMeta`.
- The current IR lowering is intentionally minimal. If the plugin cannot lower a supported Compose tree from IR, it emits the checked-in demo page shape as a fallback so the end-to-end bundle flow still works.
- Building the Android host reuses the existing Lynx Android modules and expects the same native toolchain prerequisites, including CMake `3.18.1` and NDK `21.1.6352462`.
