import { defineConfig } from 'vite';
import type { ConfigEnv, UserConfig } from 'vite';
import { getCommonBuildConfig } from './vite.base.config';

export default defineConfig((env: ConfigEnv<'build'>): UserConfig => {
  return getCommonBuildConfig(env);
});
