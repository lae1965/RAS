import { create } from 'zustand';
import { FeederActions } from '../constants';

export interface InternalFeederType {
  action: FeederActions;
  isEmergencyLevel: boolean;
}

export interface FeederType {
  name: string;
  isPowerOn: boolean;
  timeBetweenFeedings: number;
  timeOfFeeding: number;
  internal?: InternalFeederType;
}

type PartialFeeder = Partial<FeederType>;

interface FilterStore {
  feederList: FeederType[];
  initialInternal: InternalFeederType;
  initialFeeder: FeederType;
  addNewFeeder: (feeder: FeederType) => void;
  deleteFeeder: (fieederName: string) => void;
  changeFeeder: (feederOldName: string, changingProps: PartialFeeder) => void;
  setFeederList: (feederListData: FeederType[]) => void;
}

export const useFeederStore = create<FilterStore>((set, get) => {
  const initialInternal = {
    action: FeederActions.NO_ACTIONS,
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
        feederList: feederList.map((feeder, index) =>
          feederIndex === index ? { ...feeder, ...changingProps } : feeder
        ),
      });
    },
  };
});
