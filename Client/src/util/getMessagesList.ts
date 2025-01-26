import {
  DeviceType,
  FeederActions,
  feedLevel,
  FilterActions,
  toEndFeeding,
  toEndRotation,
  toEndWashing,
  toStartFeeding,
  toStartRotation,
  toStartWashing,
  waterLevel,
} from '../constants';

export const getMessagesList = (
  device: DeviceType,
  action: FilterActions | FeederActions
) => {
  const messagesList: string[] = [];

  if ('isWithDryRotation' in device) {
    // Если это фильтр
    if (device.isWithDryRotation) {
      if (action === FilterActions.NO_ACTIONS)
        messagesList.push(toStartRotation);
      else if (action === FilterActions.ROTATION)
        messagesList.push(toEndRotation);
    }
    if (device.isWithTimerWashing) {
      if (action === FilterActions.NO_ACTIONS)
        messagesList.push(toStartWashing);
      else if (action === FilterActions.WASHING)
        messagesList.push(toEndWashing);
    }
    messagesList.push(waterLevel);
  } else {
    // Если это кормушка
    if (action === FeederActions.NO_ACTIONS) messagesList.push(toStartFeeding);
    else messagesList.push(toEndFeeding);
    messagesList.push(feedLevel);
  }

  return messagesList;
};
