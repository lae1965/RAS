import { FeederType } from './store/feederStore';
import { FilterType } from './store/filterStore';

export const indicatorData = [
  'Общее питание',
  'Основной насос',
  'Резервный насос',
  'Основной компрессор',
  'Резервный компрессор',
  'Генератор кислорода',
  'Озонатор',
];

export const infoData = [
  {
    name: 'Дата',
    value: '22.11.2024',
    measurement: ' г.',
  },
  {
    name: 'Время',
    value: '22:57',
    measurement: '',
  },
  {
    name: 'T° воздуха',
    value: 25,
    measurement: '°',
  },
  {
    name: 'Влажность',
    value: 60,
    measurement: '%',
  },
  {
    name: 'T° воды',
    value: 22,
    measurement: '°',
  },
];

export const devices = ['filter', 'feeder'];

export enum Devices {
  DRUM_FILTER = 0,
  FEEDER,
}

export enum FilterActions {
  NO_ACTIONS = 0,
  ROTATION,
  WASHING,
}

export enum FeederActions {
  NO_ACTIONS = 0,
  FEEDING,
}

export const toStartRotation = 'До начала прокрутки: 02:24';
export const toEndRotation = 'До конца прокрутки: 00:27';
export const toStartWashing = 'До начала промывки: 03:48';
export const toEndWashing = 'До конца промывки: 00:56';
export const waterLevel = 'Уровень воды: 20%';
export const toStartFeeding = 'До начала кормления: 01:25:30';
export const toEndFeeding = 'До конца кормления: 00:15';
export const feedLevel = 'Количество корма: 20%';

export enum InputError {
  NO_ERROR = 0,
  ERROR_OF_TIME_BETWEEN_ROTATION,
  ERROR_OF_ROTATION_TIME,
  ERROR_OF_TIME_BETWEEN_WASHING,
  ERROR_OF_WASHING_TIME,
  ERROR_OF_TIME_BETWEEN_FEEDING,
  ERROR_OF_FEEDING_TIME,
}

export const deviceButtonsList = [
  ['Доп. прокрутка', 'Доп. промывка'],
  ['Доп. кормление'],
];

export type IndicatorColors = 'red' | 'green' | 'yellow' | 'grey';
export type DeviceType = FilterType | FeederType;
