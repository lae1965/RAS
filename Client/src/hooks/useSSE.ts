import { useEffect, useRef } from 'react';
import { FilterType, useFilterStore } from '../store/filterStore';
import { FeederType, useFeederStore } from '../store/feederStore';
import { DeviceActions, RecursivePartial } from '../types';
import { useInfoDataStore } from '../store/infoDataStore';

export const useSSE = async () => {
  const eventSourceRef = useRef<EventSource | null>(null);
  const changeFilter = useFilterStore((store) => store.changeFilter);
  const setInternalToFilter = useFilterStore(
    (store) => store.setInternalToFilter
  );
  const changeFeeder = useFeederStore((store) => store.changeFeeder);
  const setInternalToFeeder = useFeederStore(
    (store) => store.setInternalToFeeder
  );
  const setInfoData = useInfoDataStore((store) => store.setInfoData);

  useEffect(() => {
    eventSourceRef.current = new EventSource(
      'http://localhost:8080/sse/subscribe'
    );

    eventSourceRef.current.onmessage = (event: MessageEvent) => {
      const data = JSON.parse(event.data);
      switch (data.type) {
        case 'changeFilterPower': {
          const changedProps: RecursivePartial<FilterType> = {
            isPowerOn: data.isPowerOn,
          };
          if (!data.isPowerOn) {
            changedProps.internal = {
              action: DeviceActions.NO_ACTIONS,
            };
          }
          changeFilter(data.name, changedProps);
          break;
        }
        case 'changeFeederPower': {
          const changedProps: RecursivePartial<FeederType> = {
            isPowerOn: data.isPowerOn,
          };
          if (!data.isPowerOn) {
            changedProps.internal = {
              action: DeviceActions.NO_ACTIONS,
            };
          }
          changeFeeder(data.name, changedProps);
          break;
        }
        case 'everySecondReport':
          setInfoData(data.info);
          setInternalToFilter(data.filters);
          setInternalToFeeder(data.feeders);
          break;
        default:
          break;
      }
    };
    eventSourceRef.current.onerror = (error) => {
      console.error('Ошибка SSE:', error);
    };

    return () => eventSourceRef.current?.close();
    // eslint-disable-next-line react-hooks/exhaustive-deps
  }, []);
};
