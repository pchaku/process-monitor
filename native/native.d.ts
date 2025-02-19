
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

// Native Modules

declare type NativeModuleMap = {
  "NativeApplicationScanner": NativeApplicationScannerModule,
};

declare type NativeModuleTypes = keyof NativeModuleMap;
