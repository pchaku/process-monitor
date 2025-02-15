
////

// Application Scanner

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
