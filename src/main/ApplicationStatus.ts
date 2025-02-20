import { LoadNativeModule } from './utils';
import type { NativeApplicationStatusModule } from '../../native/native'; // Assuming you put your types here

const NativeApplicationStatus = LoadNativeModule<NativeApplicationStatusModule>('NativeApplicationStatus');

export class ApplicationStatus {
  private static instance: ApplicationStatus;
  private isListening = false;

  public static getInstance(): ApplicationStatus {
    if (!ApplicationStatus.instance) {
      ApplicationStatus.instance = new ApplicationStatus();
    }
    return ApplicationStatus.instance;
  }

  public ListenForStatus(
    options: { applicationName: string; windowTitle: string },
    callback: (statusUpdate: {
      status: 'open' | 'hidden' | 'closed';
      dimensions?: { x: number; y: number; width: number; height: number };
    }) => void
  ): void {
    if (this.isListening) {
      this.StopListening();
    }
    NativeApplicationStatus.ListenForStatus(options, callback);
    this.isListening = true;
  }

  public StopListening(): void {
    if (this.isListening) {
      NativeApplicationStatus.StopListening();
      this.isListening = false;
    }
  }
}
