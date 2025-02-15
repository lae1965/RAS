import { create } from 'zustand';
import {
  DeviceActions,
  InternalDeviceType,
  RecursivePartial,
  SSEPropertyList,
} from '../types';

export interface FeederType {
  name: string;
  isPowerOn: boolean;
  timeBetweenFeedings: number;
  timeOfFeeding: number;
  internal: InternalDeviceType;
}

interface FeederStore {
  feederList: FeederType[];
  initialInternal: InternalDeviceType;
  initialFeeder: FeederType;
  addNewFeeder: (feeder: FeederType) => void;
  deleteFeeder: (feederName: string) => void;
  changeFeeder: (
    feederOldName: string,
    changingProps: RecursivePartial<FeederType>
  ) => void;
  setFeederList: (feederListData: FeederType[]) => void;
  setInternalToFeeder: (internalList: SSEPropertyList[]) => void;
}

export const useFeederStore = create<FeederStore>((set, get) => {
  const initialInternal = {
    action: DeviceActions.NO_ACTIONS,
    messages: [],
    isEmergencyLevel: false,
  };

  return {
    feederList: [],
    initialInternal,
    initialFeeder: {
      name: '',
      isPowerOn: false,
      timeBetweenFeedings: 0,
      timeOfFeeding: 0,
      internal: initialInternal,
    },
    setFeederList: (feederListData) => {
      const { initialInternal } = get();
      set({
        feederList: feederListData.map((feeder) => ({
          ...feeder,
          internal: { ...initialInternal },
        })),
      });
    },
    addNewFeeder: (feeder) => {
      const { feederList } = get();
      const findIndex = feederList.findIndex(
        (feederItem) => feederItem.name === feeder.name
      );

      if (findIndex != -1)
        throw new Error(`Feeder with name ${feeder.name} already exist`);
      set({
        feederList: [
          ...feederList,
          { ...feeder, internal: { ...get().initialInternal } },
        ],
      });
    },
    deleteFeeder: (feederName) => {
      const { feederList } = get();
      const feeder: FeederType | undefined = feederList.find(
        (feederItem) => feederItem.name === feederName
      );
      if (!feeder) throw new Error(`Feeder with name ${feederName} not exist`);
      set({
        feederList: feederList.filter(
          (feederItem) => feederItem.name !== feederName
        ),
      });
    },
    changeFeeder: (feederOldName, changingProps) => {
      const { feederList } = get();
      const feederIndex = feederList.findIndex(
        (feederItem) => feederItem.name === feederOldName
      );
      if (feederIndex === -1)
        throw new Error(`Feeder with name ${feederOldName} not exist`);

      set({
        feederList: <FeederType[]>feederList.map((feeder, index) => {
          if (feederIndex !== index) return feeder;
          const changingFeeder: RecursivePartial<FeederType> = {
            ...feeder,
            ...changingProps,
          };
          if (changingProps.internal)
            changingFeeder.internal = {
              ...changingFeeder.internal,
              messages: [
                ...(changingFeeder.internal?.messages || []),
                ...(changingProps.internal.messages || []),
              ],
              ...changingProps.internal,
            };
          return changingFeeder;
        }),
      });
    },
    setInternalToFeeder: (internalList) => {
      const { feederList } = get();
      set({
        feederList: feederList.map((feeder) => {
          const _internal = internalList.find(
            (internalItem) =>
              internalItem.name !== undefined &&
              internalItem.name === feeder.name
          );
          if (!_internal) return feeder;
          return {
            ...feeder,
            internal: {
              isEmergencyLevel: false,
              action: _internal.action,
              messages: [..._internal.messageList],
            },
          };
        }),
      });
    },
  };
});
