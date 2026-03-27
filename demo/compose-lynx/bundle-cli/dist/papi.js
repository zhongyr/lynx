"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
exports.createGeneratedPagePapi = createGeneratedPagePapi;
function createGeneratedPagePapi(entryName) {
    return {
        attach(lynx) {
            globalThis.__compose_demo_runtime = {
                entryName,
                hasLynx: Boolean(lynx),
            };
        },
    };
}
