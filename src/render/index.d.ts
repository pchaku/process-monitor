import { ApplicationMonitorApi, ApplicationMonitorApiName } from "./preload";

declare global {
  interface Window {
    [ApplicationMonitorApiName]: typeof ApplicationMonitorApi;
  }
}

export {}