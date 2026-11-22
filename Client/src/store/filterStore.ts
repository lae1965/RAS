import { create } from 'zustand';
import {
  DeviceActions,
  InternalDeviceType,
  RecursivePartial,
  SSEPropertyList,
} from '../types';

export interface FilterType {
  name: string;
  isPowerOn: boolean;
  isWithDryRotation: boolean;
  isWithTimerWashing: boolean;
  isWithLevelWashing: boolean;
  timeBetweenRotations: number;
  timeOfRotation: number;
  timeBetweenWashings: number;
  timeOfWashing: number;
  levelOfBeginWashing: number;
  internal: InternalDeviceType;
}

interface FilterStore {
  filterList: FilterType[];
  initialInternal: InternalDeviceType;
  initialFilter: FilterType;
  addNewFilter: (filter: FilterType) => void;
  deleteFilter: (filterName: string) => void;
  changeFilter: (
    filterOldName: string,
    changingProps: RecursivePartial<FilterType>
  ) => void;
  setFilterList: (filterListData: FilterType[]) => void;
  setInternalToFilter: (internalList: SSEPropertyList[]) => void;
}

export const useFilterStore = create<FilterStore>((set, get) => {
  const initialInternal = {
    action: DeviceActions.NO_ACTIONS,
    messages: [],
    isEmergencyLevel: false,
  };

  return {
    filterList: [],
    initialInternal,
    initialFilter: {
      name: '',
      isPowerOn: false,
      isWithDryRotation: false,
      isWithTimerWashing: false,
      isWithLevelWashing: false,
      timeBetweenRotations: 0,
      timeOfRotation: 0,
      timeBetweenWashings: 0,
      timeOfWashing: 0,
      levelOfBeginWashing: 30,
      internal: initialInternal,
    },
    setFilterList: (filterListData) => {
      const { initialInternal } = get();
      set({
        filterList: filterListData.map((filter) => ({
          ...filter,
          internal: { ...initialInternal },
        })),
      });
    },
    addNewFilter: (filter) => {
      const { filterList } = get();
      const findIndex = filterList.findIndex(
        (filterItem) => filterItem.name === filter.name
      );

      if (findIndex != -1)
        throw new Error(`Filter with name ${filter.name} already exist`);
      set({
        filterList: [
          ...filterList,
          { ...filter, internal: { ...get().initialInternal } },
        ],
      });
    },
    deleteFilter: (filterName) => {
      const { filterList } = get();
      const filter: FilterType | undefined = filterList.find(
        (filterItem) => filterItem.name === filterName
      );
      if (!filter) throw new Error(`Filter with name ${filterName} not exist`);
      set({
        filterList: filterList.filter(
          (filterItem) => filterItem.name !== filterName
        ),
      });
    },
    changeFilter: (filterOldName, changingProps) => {
      const { filterList } = get();
      const filterIndex = filterList.findIndex(
        (filterItem) => filterItem.name === filterOldName
      );
      if (filterIndex === -1)
        throw new Error(`Filter with name ${filterOldName} not exist`);

      set({
        filterList: <FilterType[]>filterList.map((filter, index) => {
          if (filterIndex !== index) return filter;
          const changingFilter: RecursivePartial<FilterType> = {
            ...filter,
            ...changingProps,
          };
          if (changingProps.internal)
            changingFilter.internal = {
              ...changingFilter.internal,
              messages: [
                ...(changingFilter.internal!.messages || []),
                ...(changingProps.internal.messages || []),
              ],
              ...changingProps.internal,
            };
          return changingFilter;
        }),
      });
    },
    setInternalToFilter: (internalList) => {
      const { filterList } = get();
      set({
        filterList: filterList.map((filter) => {
          const _internal = internalList.find(
            (internalItem) =>
              internalItem.name !== undefined &&
              internalItem.name === filter.name
          );
          if (!_internal) return filter;
          return {
            ...filter,
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
