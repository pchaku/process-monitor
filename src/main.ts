import { app, BrowserWindow } from 'electron';
import { join } from 'node:path';
import started from 'electron-squirrel-startup';
import { WindowManager } from './main/window-manager';
import { ApplicationStatus } from './main/ApplicationStatus';

// Handle creating/removing shortcuts on Windows when installing/uninstalling.
if (started) {
  app.quit();
}

const createWindow = () => {

  // all main thread side application logic is handled by WindowManager

  // Output from the build script is

  //   .
  // └── .vite/
  //     ├── build/
  //     │   ├── RENDER_NAME/
  //     │   │   └── preload.js
  //     │   └── main.js/
  //     └── RENDER_NAME/
  //         ├── assets/
  //         │   └── index.[js, css] (render, bundle)
  //         └── index.html (static root)

  // the folowing locations are relative to the location of main.js

  const preloadEntry = join(
    __dirname,
    RENDER_VITE_NAME,
    "preload.js"
  );
  const staticEntry = join(
    __dirname,
    "..",
    RENDER_VITE_NAME,
    "index.html"
  );
  const devEntry = RENDER_VITE_DEV_SERVER_URL;
  new WindowManager(preloadEntry, staticEntry, devEntry);
};

// This method will be called when Electron has finished
// initialization and is ready to create browser windows.
// Some APIs can only be used after this event occurs.
app.on('ready', createWindow);

// Quit when all windows are closed, except on macOS. There, it's common
// for applications and their menu bar to stay active until the user quits
// explicitly with Cmd + Q.
app.on('window-all-closed', () => {
  if (process.platform !== 'darwin') {
    app.quit();
  }
});

app.on('activate', () => {
  // On OS X it's common to re-create a window in the app when the
  // dock icon is clicked and there are no other windows open.
  if (BrowserWindow.getAllWindows().length === 0) {
    createWindow();
  }
});

app.whenReady().then(() => {
  console.log('Electron App Started');

  const appStatus = ApplicationStatus.getInstance();

  appStatus.ListenForStatus(
    { applicationName: 'Finder', windowTitle: 'Recents' },
    (result) => {
      console.log('Status Update:', result);
    }
  );

  // Optional: Stop listening after 15s
  setTimeout(() => {
    appStatus.StopListening();
    console.log('Stopped Listening');
  }, 15000);
});