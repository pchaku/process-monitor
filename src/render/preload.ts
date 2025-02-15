
import {
  ApplicationScannerChannel, RenderMessage, RenderRequestChannel
} from "../types";
import { contextBridge, ipcRenderer } from "electron";

export const ApplicationMonitorApi = {
  StartApplicationScanner: (callback: ApplicationScannerCallback) => {
    // bind the callback
    ipcRenderer.on(ApplicationScannerChannel, (_, sa: ScannedApplications) => {
      callback(sa);
    });
    // start the service
    const message: RenderMessage = {
      request: "ApplicationScanner:StartListener"
    };
    ipcRenderer.send(RenderRequestChannel, message);
  }
}

export const ApplicationMonitorApiName = 'ApplicationMonitorApi';

process.once("loaded", () => {
  contextBridge.exposeInMainWorld(ApplicationMonitorApiName, ApplicationMonitorApi);
});
  