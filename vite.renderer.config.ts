import { defineConfig } from 'vite';
import vue from '@vitejs/plugin-vue';
import path from "node:path";

import type { ConfigEnv, UserConfig } from 'vite';

export default defineConfig((env: ConfigEnv<'renderer'>): UserConfig => {
  const { root, forgeConfigSelf: { name } } = env;
  return {
    root: path.join(__dirname, "src", name),
    build: {
      rollupOptions: {
        input: path.join(__dirname, "src", name, "index.html"),
      },
      outDir: path.join(root, ".vite", name),
    },
    plugins: [
      vue(),
    ],
  };
});