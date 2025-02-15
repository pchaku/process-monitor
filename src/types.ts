
// requests renderer -> main

export const RenderRequestChannel = "render-request-channel";

export type RenderRequests = "ApplicationScanner:StartListener" | "ApplicationScanner:StopListener";

export type RenderMessage = {
  request: RenderRequests
}

// responses / callbacks main -> render

export const ApplicationScannerChannel = "applicationScannerChannel";