import { defineConfig, mergeConfig } from 'vite';
import type { ConfigEnv, UserConfig } from 'vite';
import { getCommonBuildConfig } from "./vite.base.config";


export default defineConfig((env: ConfigEnv<'build'>): UserConfig => {
    const { forgeConfigSelf: { entry } } = env;
  
    const pathRegex = new RegExp('^src\/(.*?)\/preload.ts$');
    const pathOut = (pathRegex.exec(entry as string) as Array<string>)[1];
  
    const preloadConfig: UserConfig = {
      build: {
        rollupOptions: {
          output: {
            // It should not be split chunks.
            entryFileNames: `${pathOut}/[name].js`,
            chunkFileNames: `${pathOut}/[name].js`,
            assetFileNames: `${pathOut}/[name].[ext]`,
          },
        },
      },
    };
    return mergeConfig(getCommonBuildConfig(env), preloadConfig);
  });