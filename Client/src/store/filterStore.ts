import { create } from 'zustand';
import { FilterActions } from '../constants';

export interface InternalFilterType {
  action: FilterActions;
  isEmergencyLevel: boolean;
}

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
  internal?: InternalFilterType;
}

type PartialFilter = Partial<FilterType>;

interface FilterStore {
  filterList: FilterType[];
  initialInternal: InternalFilterType;
  initialFilter: FilterType;
  addNewFilter: (filter: FilterType) => void;
  deleteFilter: (filterName: string) => void;
  changeFilter: (filterOldName: string, changingProps: PartialFilter) => void;
  setFilterList: (filterListData: FilterType[]) => void;
}

export const useFilterStore = create<FilterStore>((set, get) => {
  const initialInternal = {
    action: FilterActions.NO_ACTIONS,
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
        filterList: filterList.map((filter, index) =>
          filterIndex === index ? { ...filter, ...changingProps } : filter
        ),
      });
    },
  };
});
