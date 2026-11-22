import { create } from 'zustand';

interface DataProperty {
  name: string;
  value: number | string;
  measurment?: string;
}

type InfoDataKey =
  | 'date'
  | 'time'
  | 'airTemperature'
  | 'humidity'
  | 'waterTemperature';

interface InfoDataStore {
  infoData: { [key in InfoDataKey]: DataProperty };
  setInfoData: (newData: { [key in InfoDataKey]: number | string }) => void;
}

export const useInfoDataStore = create<InfoDataStore>((set, get) => {
  return {
    infoData: {
      date: {
        name: 'Дата',
        value: '',
        measurment: ' г.',
      },
      time: {
        name: 'Время',
        value: '',
        measurment: '',
      },
      airTemperature: {
        name: 'T° воздуха',
        value: 25,
        measurment: '°',
      },
      humidity: {
        name: 'Влажность',
        value: 60,
        measurment: '%',
      },
      waterTemperature: {
        name: 'T° воды',
        value: 22,
        measurment: '°',
      },
    },
    setInfoData: (newData) => {
      const { infoData } = get();
      const newInfoData = { ...infoData };
      Object.entries(newData).forEach(([key, value]) => {
        newInfoData[key as InfoDataKey].value = value;
      });
      set({
        infoData: newInfoData,
      });
    },
  };
});
