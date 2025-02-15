
import { createRequire } from "node:module";
import { join } from "node:path";


export const LoadNativeModule = <T extends NativeModuleTypes>(nativeModuleName: T): NativeModuleMap[T] => {
  const nativeDir = import.meta.env.VITE_NATIVE_DIR || process.resourcesPath;
  const nativePath = join(nativeDir, nativeModuleName);
  try {
    const nodeRequire = createRequire(import.meta.url);
    const nativeModule = nodeRequire(nativePath);
    return nativeModule.default || nativeModule;
  } catch (err) {
    throw new Error(`
      Failed to load native module ${nativeModuleName}
      at ${nativePath}
      with error: ${err}
    `);
  }
};