import { useEffect, useRef } from 'react';
import { useFilterStore } from '../store/filterStore';
import { useFeederStore } from '../store/feederStore';

export const useSSE = async () => {
  const eventSourceRef = useRef<EventSource | null>(null);
  const changeFilter = useFilterStore((store) => store.changeFilter);
  const changeFeeder = useFeederStore((store) => store.changeFeeder);

  useEffect(() => {
    eventSourceRef.current = new EventSource(
      'http://localhost:8080/sse/subscribe'
    );

    eventSourceRef.current.onmessage = (event: MessageEvent) => {
      const data = JSON.parse(event.data);
      switch (data.type) {
        case 'changeFilterPower':
          changeFilter(data.name, { isPowerOn: data.isPowerOn });
          break;
        case 'changeFeederPower':
          changeFeeder(data.name, { isPowerOn: data.isPowerOn });
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
