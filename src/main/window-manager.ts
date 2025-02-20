import { BrowserWindow, globalShortcut, ipcMain } from "electron";
import { ApplicationScannerChannel, ApplicationStatusChannel, RenderMessage, RenderRequestChannel } from "../types";
import { LoadNativeModule } from "./utils";
import { ApplicationStatus } from "./ApplicationStatus";

// loads the NativeApplicationScanner module - the link between the back and front end
const NativeApplicationScanner = LoadNativeModule("NativeApplicationScanner");
// const NativeApplicationStatus = LoadNativeModule("NativeApplicationStatus");

// NativeApplicationStatus.ListenForStatus(
//   { applicationName: "Finder", windowTitle: "Recents" },
//   (result) => {
//     console.log("Status Update", result);
//   }
// );

export class WindowManager {
  private window: BrowserWindow;
  private readonly windowSize = { width: 720, height: 420 };
  private applicationScannerIsRunning = false;
  constructor(
    preloadEntry: string,
    staticEntry: string,
    devEntry: string
  ) {

    // create the window

    this.window = new BrowserWindow({
      webPreferences: { preload: preloadEntry},
      height: this.windowSize.height,
      width: this.windowSize.width,
      skipTaskbar: true,
      show: true,
      frame: true,
      alwaysOnTop: true,
      transparent: false,
      resizable: true,
      autoHideMenuBar: true
    });

    // either load it from vite hot reload server, or static file

    if (devEntry) {
      this.window.loadURL(devEntry);
    } else {
      this.window.loadFile(staticEntry);
    }

    // register a shortcut for opening up dev tools

    globalShortcut.register("Control+Shift+I", () => {
      this.window.webContents.openDevTools({
        mode: "detach",
        activate: false,
      });
    });

    // setup IPC handlers for bi-directional communication
    this.setupIpcHandlers();

  }

  private setupIpcHandlers() {
    ipcMain.on(RenderRequestChannel, (_, message: RenderMessage) => {
      switch (message.request) {
        case "ApplicationScanner:StartListener":
          if (this.applicationScannerIsRunning) {
            NativeApplicationScanner.StopListener();
          }
          NativeApplicationScanner.ListenForApplications((scannedApplications) => {
            this.window.webContents.send(ApplicationScannerChannel, scannedApplications)
          });
          this.applicationScannerIsRunning = true;
          break;
        case "ApplicationScanner:StopListener":
          NativeApplicationScanner.StopListener();
          this.applicationScannerIsRunning = false;
          break;
        case "ApplicationStatus:StartListening": {
          const { applicationName, windowTitle } = message.data;
          ApplicationStatus.getInstance().ListenForStatus(
            { applicationName, windowTitle },
            (statusUpdate) => {
              this.window.webContents.send(ApplicationStatusChannel, statusUpdate);
            }
          );
          break;
        }
        case "ApplicationStatus:StopListening": {
          ApplicationStatus.getInstance().StopListening();
          break;
        } 
        default:
          throw new Error(
            `Main thread received unhandled request type on ipc channel ${RenderRequestChannel}: ${message.request}`
          );
      }
    });
  }

}