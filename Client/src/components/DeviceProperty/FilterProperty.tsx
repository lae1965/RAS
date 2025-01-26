import { useState } from 'react';
import styles from './DeviceProperty.module.css';

import { useInput } from '../../hooks/useInput';
import { useTimeInput } from '../../hooks/useTimeInput';
import { FilterType, useFilterStore } from '../../store/filterStore';
import { Checkbox } from '../Checkbox/Checkbox';
import { NumberInput } from '../NumberInput/NumberInput';
import { TextInput } from '../TextInput/TextInput';
import { TimeInput } from '../TimeInput/TimeInput';
import { Devices, FilterActions, InputError } from '../../constants';
import { DeleteIcon } from '../Icons/DeleteIcon';
import { Dialog } from '../Dialog/Dialog';
import { DeleteDialog } from '../DeleteDialog/DeleteDialog';
import { validateFilter } from '../../util/validateDevice';
import {
  fetchCreateNewDevice,
  fetchRemoveDevice,
  fetchUpdateDevice,
} from '../../api/deviceAPI';

interface FilterPropertyProps extends React.HTMLAttributes<HTMLDivElement> {
  onClose: () => void;
  filter: FilterType;
  readonly?: boolean;
}

export const FilterProperty: React.FC<FilterPropertyProps> = ({
  onClose,
  filter,
  readonly = false,
}) => {
  const [isReadOnly, setIsReadOnly] = useState<boolean>(readonly);
  const [isOpenDialog, setIsOpenDialog] = useState<boolean>(false);
  const { addNewFilter, changeFilter, deleteFilter } = useFilterStore();

  const filterNameInput = useInput('filterName', filter.name);
  const isWithDryRotation = useInput(
    'isWithDryRotation',
    filter.isWithDryRotation
  );
  const timeBetweenRotations = useTimeInput(
    'timeBetweenRotations',
    filter.timeBetweenRotations
  );
  const timeOfRotation = useTimeInput('timeOfRotation', filter.timeOfRotation);
  const isWithTimerWashing = useInput(
    'isWithTimerWashing',
    filter.isWithTimerWashing
  );
  const timeBetweenWashings = useTimeInput(
    'timeBetweenWashings',
    filter.timeBetweenWashings
  );
  const timeOfWashing = useTimeInput('timeOfWashing', filter.timeOfWashing);
  const isWithLevelWashing = useInput(
    'isWithLevelWashing',
    filter.isWithLevelWashing
  );
  const level = useInput(
    'level',
    filter.levelOfBeginWashing.toString() || '30'
  );

  const handlerLevelBlur = (e: React.ChangeEvent<HTMLInputElement>) => {
    const value = +e.target.value;
    let newValue: string;
    if (value < 20) newValue = '20';
    else if (value > 50) newValue = '50';
    else newValue = value.toString();
    level.setValue(newValue);
  };

  const handleSubmit = async (e: React.FormEvent<HTMLFormElement>) => {
    e.preventDefault();

    const newFilter: FilterType = {
      name: filterNameInput.input.value!,
      isPowerOn: false,
      isWithDryRotation: isWithDryRotation.input.value!,
      isWithTimerWashing: isWithTimerWashing.input.value!,
      isWithLevelWashing: isWithLevelWashing.input.value!,
      timeBetweenRotations: timeBetweenRotations.time,
      timeOfRotation: timeOfRotation.time,
      timeBetweenWashings: timeBetweenWashings.time,
      timeOfWashing: timeOfWashing.time,
      levelOfBeginWashing: +level.input.value!,
      internal: {
        action: FilterActions.NO_ACTIONS,
        isEmergencyLevel: false,
      },
    };

    const errorOrIsChange = validateFilter(newFilter);
    if (typeof errorOrIsChange === 'boolean') {
      if (errorOrIsChange) {
        isWithDryRotation.setValue(newFilter.isWithDryRotation);
        timeBetweenRotations.setTime(newFilter.timeBetweenRotations);
        timeOfRotation.setTime(newFilter.timeOfRotation);
        isWithTimerWashing.setValue(newFilter.isWithTimerWashing);
        timeBetweenWashings.setTime(newFilter.timeBetweenWashings);
        timeOfWashing.setTime(newFilter.timeOfWashing);
      }
    } else {
      switch (errorOrIsChange) {
        case InputError.ERROR_OF_TIME_BETWEEN_ROTATION:
          timeBetweenRotations.setError(true);
          return;
        case InputError.ERROR_OF_ROTATION_TIME:
          timeOfRotation.setError(true);
          return;
        case InputError.ERROR_OF_TIME_BETWEEN_WASHING:
          timeBetweenWashings.setError(true);
          return;
        case InputError.ERROR_OF_WASHING_TIME:
          timeOfWashing.setError(true);
          return;
        default:
          break;
      }
    }

    if (filter.name === '') {
      try {
        await fetchCreateNewDevice(newFilter, Devices.DRUM_FILTER);
        addNewFilter(newFilter);
      } catch (e) {
        console.log(e);
      }
      onClose();
    } else {
      try {
        await fetchUpdateDevice(filter.name, newFilter, Devices.DRUM_FILTER);
        changeFilter(filter.name, newFilter);
      } catch (e) {
        console.log(e);
      }
      setIsReadOnly(true);
    }
  };

  const handleChancel = () => {
    if (filter.name !== '') {
      filterNameInput.setValue(filter.name);
      isWithDryRotation.setValue(filter.isWithDryRotation);
      timeBetweenRotations.setTime(filter.timeBetweenRotations);
      timeOfRotation.setTime(filter.timeOfRotation);
      isWithTimerWashing.setValue(filter.isWithTimerWashing);
      timeBetweenWashings.setTime(filter.timeBetweenWashings);
      timeOfWashing.setTime(filter.timeOfWashing);
      isWithLevelWashing.setValue(filter.isWithLevelWashing);
      level.setValue(filter.levelOfBeginWashing.toString());
      setIsReadOnly(true);
    } else onClose();
  };

  const handleDelete = async (e: React.MouseEvent<HTMLButtonElement>) => {
    e.preventDefault();
    try {
      await fetchRemoveDevice(filter.name, Devices.DRUM_FILTER);
      deleteFilter(filter.name);
    } catch (e) {
      console.log(e);
    }
  };

  return (
    <form className={styles.form} onSubmit={handleSubmit}>
      <div className={styles.properties}>
        <TextInput
          description='Название фильтра:'
          inputReturn={filterNameInput}
          disabled={isReadOnly}
          required
        />
        <Checkbox
          description='Сухая прокрутка'
          inputReturn={isWithDryRotation}
          disabled={isReadOnly}
        />
        {isWithDryRotation.input.value && (
          <div className={styles.timeInputs}>
            <TimeInput
              description='Время между прокрутками:'
              timeReturn={timeBetweenRotations}
              readonly={isReadOnly}
            />
            <TimeInput
              description='Время прокрутки:'
              timeReturn={timeOfRotation}
              readonly={isReadOnly}
            />
          </div>
        )}
        <Checkbox
          description='Промывка по таймеру'
          inputReturn={isWithTimerWashing}
          disabled={isReadOnly}
        />
        {isWithTimerWashing.input.value && (
          <div className={styles.timeInputs}>
            <TimeInput
              description='Время между промывками:'
              timeReturn={timeBetweenWashings}
              readonly={isReadOnly}
            />
            <TimeInput
              description='Время промывки:'
              timeReturn={timeOfWashing}
              readonly={isReadOnly}
            />
          </div>
        )}
        <Checkbox
          description='Промывка по уровню'
          inputReturn={isWithLevelWashing}
          disabled={isReadOnly}
        />
        {isWithLevelWashing.input.value && (
          <label className={styles.level}>
            Максимальный уровень:{' '}
            <div className={styles.percents}>
              <NumberInput
                min={20}
                max={50}
                inputReturn={level}
                handlerBlur={handlerLevelBlur}
                disabled={isReadOnly}
              />
              %
            </div>
          </label>
        )}
      </div>
      <div className={styles.buttons}>
        {(isReadOnly && (
          <>
            <button
              type='button'
              className={styles.button}
              onClick={(e) => {
                e.preventDefault();
                setIsReadOnly(false);
              }}
            >
              Редактировать
            </button>
            <button
              className={styles.buttonIcon}
              title='Удалить фильтр'
              onClick={(e) => {
                e.preventDefault();
                setIsOpenDialog(true);
              }}
            >
              <DeleteIcon className={styles.deleteIcon} />
            </button>
            {isOpenDialog && (
              <Dialog>
                <DeleteDialog
                  device='фильтр'
                  nameOfDevice={filter.name}
                  setIsOpenDialog={setIsOpenDialog}
                  handleDelete={handleDelete}
                />
              </Dialog>
            )}
          </>
        )) || (
          <>
            <button type='submit' className={styles.button}>
              Сохранить
            </button>
            <button
              type='button'
              className={`${styles.button} ${styles.greyButton}`}
              onClick={handleChancel}
            >
              Отменить
            </button>
          </>
        )}
      </div>
    </form>
  );
};
