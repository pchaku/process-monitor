# Application Monitor

This repo makes use of electron-forge, vite, typecript, vue, and native modules to create a basic process monitor. Hopefully, with your help, we can make it do some more things!

** NOTE: THIS APP ONLY RUNS ON WINDOWS ATM **

## Requirements

- A C++ compiler that supports at least C++17 standard. We use the latest MVSC but other options should work fine
- NodeJS. This was built with V22.12.0, but any LTS version should work fine as well

## Setup

Install with `npm ci`. This should build the native modules as well. If you need to rebuild native modules, we provide the command `npm run build`.

You can run with a hot reloading development server with `npm run start`. Note that hot reloading only really works with render code; usually updating main code kicks off vite, but doesn't actually reload the running application.

As native modules are rebuilt for electron by electron-forge, changes to native modules will require stopping and stopping the dev server as well.

You can bundle the application for distribution with `npm run make`. Ensure that any code changes work in both dev and prod builds.

## Folder Structure

.
├── native - native node modules written in C++
│   ├── native.d.ts - typings for native modules for use with typescript
│   └── lib
│       ├── application-scanner - a native module that scans running applications with an active window
│       └── utilities - reusable components / functions
├── src
│   ├── main - code that will run in the main thread
│   ├── render - code that will run in the render (chromium) thread
│   ├── main.ts - application entrypoint
│   └── types.ts - constants common between main / render
├── binding.gyp - build system config for native modules
├── forge.config.ts - electron-forge configuration
└── vite.[main / preload / renderer].config.ts - specific vite configs for the different targets
# process-monitor
