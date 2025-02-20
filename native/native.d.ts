
////

// Application Scanner
// This exposes the NativeApplicationScanner to JS/TS and tells it what type of data to expect

declare interface ScannedApplication {
  windowTitle: string;
  procedurePath: string;
  procedureName: string;
}

declare type ScannedApplications = Array<ScannedApplication>;
declare type ApplicationScannerCallback = (sa: ScannedApplications) => void;

declare interface NativeApplicationScannerModule {
  ListenForApplications: (callback: ApplicationScannerCallback) => void;
  StopListener: () => void;
}

////

//Application Status
declare interface WindowStatus {
  status: 'open' | 'hidden' | 'closed';
  dimensions?: { x: number; y: number; width: number; height: number };
}

declare type WindowStatuses = (result: WindowStatus) => void;

declare interface NativeApplicationStatusModule {
  ListenForStatus: (
    options: { applicationName: string; windowTitle: string },
    callback: WindowStatuses
  ) => void;
  StopListening: () => void;
}

// Native Modules

declare type NativeModuleMap = {
  "NativeApplicationScanner": NativeApplicationScannerModule,
  "NativeApplicationStatus": NativeApplicationStatusModule,
};

declare type NativeModuleTypes = keyof NativeModuleMap;
