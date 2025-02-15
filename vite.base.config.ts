import * as path from 'node:path';
import type { ConfigEnv, UserConfig } from 'vite';
import EnvironmentPlugin from "vite-plugin-environment";

export const getCommonBuildConfig = (env: ConfigEnv<'build'>): UserConfig => {
  const { command } = env;
  let nativePath = "";
  if (command === "serve") {
    nativePath = path.join(__dirname, "build", "Release");
  }
  return {
    plugins: [
      EnvironmentPlugin({
        VITE_NATIVE_DIR: nativePath
      }, {
        // used to match default vite behavior
        defineOn: 'import.meta.env'
      })
    ]
  };
}