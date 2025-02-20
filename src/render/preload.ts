
import {
  ApplicationScannerChannel, ApplicationStatusChannel, RenderMessage, RenderRequestChannel
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
  },

  StartApplicationStatus: (applicationName: string, windowTitle: string, callback: WindowStatuses) => {
    ipcRenderer.on(ApplicationStatusChannel, (_, statusUpdate: WindowStatus) => {
        callback(statusUpdate);
    });

    ipcRenderer.send(RenderRequestChannel, {
      request: 'ApplicationStatus:StartListening',
      data: { applicationName, windowTitle }
    });
  },

  StopApplicationStatus: () => {
    ipcRenderer.send(RenderRequestChannel, { request: 'ApplicationStatus:StopListening' });
  }
}

export const ApplicationMonitorApiName = 'ApplicationMonitorApi';

process.once("loaded", () => {
  contextBridge.exposeInMainWorld(ApplicationMonitorApiName, ApplicationMonitorApi);
});
  