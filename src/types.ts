
// requests renderer -> main

export const RenderRequestChannel = "render-request-channel";

export type RenderRequests = "ApplicationScanner:StartListener" 
| "ApplicationScanner:StopListener" 
| "ApplicationStatus:StartListening"
| "ApplicationStatus:StopListening";

export type RenderMessage = {
  request: RenderRequests;
  data?: any;
}

// responses / callbacks main -> render

export const ApplicationScannerChannel = "applicationScannerChannel";
export const ApplicationStatusChannel = "applicationStatusChannel";