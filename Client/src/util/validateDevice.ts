import { FeederType } from '../store/feederStore';
import { FilterType } from '../store/filterStore';
import { InputError } from '../types';

enum LocalError {
  NO_ERROR = 0,
  ERROR_OF_TIME_BETWEEN_ACTIONS,
  ERROR_OF_ACTION_TIME,
}

type FilterKeys = keyof FilterType;

const validate = (
  filter: FilterType,
  checkBoxName: FilterKeys,
  betweenName: FilterKeys,
  ofName: FilterKeys
): boolean | LocalError => {
  let isChanges = false;
  const timeBetween = filter[betweenName] as number;
  const timeOf = filter[ofName] as number;

  if (filter[checkBoxName]) {
    if (!timeBetween && !timeOf) {
      (filter[checkBoxName] as boolean) = false;
      isChanges = true;
    } else {
      if (!timeBetween) return LocalError.ERROR_OF_TIME_BETWEEN_ACTIONS;
      if (!timeOf) return LocalError.ERROR_OF_ACTION_TIME;
    }
  } else {
    if (timeBetween > 0 || timeOf > 0) {
      (filter[betweenName] as number) = 0;
      (filter[ofName] as number) = 0;
      isChanges = true;
    }
  }

  return isChanges;
};

export const validateFilter = (filter: FilterType): boolean | InputError => {
  const dryRotation = validate(
    filter,
    'isWithDryRotation',
    'timeBetweenRotations',
    'timeOfRotation'
  );
  if (typeof dryRotation !== 'boolean' && dryRotation > LocalError.NO_ERROR)
    return dryRotation as number as InputError;

  const timerWasing = validate(
    filter,
    'isWithTimerWashing',
    'timeBetweenWashings',
    'timeOfWashing'
  );
  if (typeof timerWasing !== 'boolean' && timerWasing > LocalError.NO_ERROR)
    return ((timerWasing as number) + 2) as InputError;

  return (dryRotation || timerWasing) as boolean;
};

export const validateFeeder = (feeder: FeederType): InputError | null => {
  if (!feeder.timeBetweenFeedings)
    return InputError.ERROR_OF_TIME_BETWEEN_FEEDING;
  if (!feeder.timeOfFeeding) return InputError.ERROR_OF_FEEDING_TIME;

  return null;
};
