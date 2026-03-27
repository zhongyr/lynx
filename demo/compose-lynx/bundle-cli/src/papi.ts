import type * as LynxTypes from "@lynx-js/types";

export interface ComposeLynxPapi {
  attach(lynx: LynxTypes.MainThread.Lynx | LynxTypes.Background.Lynx): void;
}

export function createGeneratedPagePapi(entryName: string): ComposeLynxPapi {
  return {
    attach(lynx) {
      (globalThis as Record<string, unknown>).__compose_demo_runtime = {
        entryName,
        hasLynx: Boolean(lynx),
      };
    },
  };
}
