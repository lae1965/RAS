import { FeederType } from './store/feederStore';
import { FilterType } from './store/filterStore';

export enum Devices {
  DRUM_FILTER = 0,
  FEEDER,
}

export enum DeviceActions {
  NO_ACTIONS = 0,
  ROTATION = 1,
  FEEDING = 1,
  WASHING = 2,
}

export enum InputError {
  NO_ERROR = 0,
  ERROR_OF_TIME_BETWEEN_ROTATION,
  ERROR_OF_ROTATION_TIME,
  ERROR_OF_TIME_BETWEEN_WASHING,
  ERROR_OF_WASHING_TIME,
  ERROR_OF_TIME_BETWEEN_FEEDING,
  ERROR_OF_FEEDING_TIME,
}

export type IndicatorColors = 'red' | 'green' | 'yellow' | 'grey';
export type DeviceType = FilterType | FeederType;

export interface DeviceMessageType {
  message: number;
  time: number;
}

export interface InternalDeviceType {
  action: DeviceActions;
  messages: DeviceMessageType[];
  isEmergencyLevel: boolean;
}

export interface SSEPropertyList {
  name: string;
  action: DeviceActions;
  messageList: DeviceMessageType[];
}

export type RecursivePartial<T> = {
  [P in keyof T]?: T[P] extends object ? RecursivePartial<T[P]> : T[P];
};
