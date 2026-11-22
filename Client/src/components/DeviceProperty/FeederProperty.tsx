import { useState } from 'react';
import styles from './DeviceProperty.module.css';

import { useInput } from '../../hooks/useInput';
import { useTimeInput } from '../../hooks/useTimeInput';
import { TextInput } from '../TextInput/TextInput';
import { TimeInput } from '../TimeInput/TimeInput';
import { DeleteIcon } from '../Icons/DeleteIcon';
import { Dialog } from '../Dialog/Dialog';
import { DeleteDialog } from '../DeleteDialog/DeleteDialog';
import { validateFeeder } from '../../util/validateDevice';
import { FeederType, useFeederStore } from '../../store/feederStore';
import {
  fetchCreateNewDevice,
  fetchRemoveDevice,
  fetchUpdateDevice,
} from '../../api/deviceAPI';
import { DeviceActions, Devices, InputError } from '../../types';

interface FeederPropertyProps extends React.HTMLAttributes<HTMLDivElement> {
  onClose: () => void;
  feeder: FeederType;
  readonly?: boolean;
}

export const FeederProperty: React.FC<FeederPropertyProps> = ({
  onClose,
  feeder,
  readonly = false,
}) => {
  const [isReadOnly, setIsReadOnly] = useState<boolean>(readonly);
  const [isOpenDialog, setIsOpenDialog] = useState<boolean>(false);
  const { addNewFeeder, changeFeeder, deleteFeeder } = useFeederStore();

  const feederNameInput = useInput('feederName', feeder.name);
  const timeBetweenFeedings = useTimeInput(
    'timeBetweenFeedings',
    feeder.timeBetweenFeedings
  );
  const timeOfFeeding = useTimeInput('timeOfFeeding', feeder.timeOfFeeding);

  const handleSubmit = async (e: React.FormEvent<HTMLFormElement>) => {
    e.preventDefault();

    const newFeeder: FeederType = {
      name: feederNameInput.input.value!,
      isPowerOn: feeder.isPowerOn,
      timeBetweenFeedings: timeBetweenFeedings.time,
      timeOfFeeding: timeOfFeeding.time,
      internal: {
        action: DeviceActions.NO_ACTIONS,
        messages: [],
        isEmergencyLevel: false,
      },
    };

    const error = validateFeeder(newFeeder);
    switch (error) {
      case InputError.ERROR_OF_TIME_BETWEEN_FEEDING:
        timeBetweenFeedings.setError(true);
        return;
      case InputError.ERROR_OF_FEEDING_TIME:
        timeOfFeeding.setError(true);
        return;
      default:
        break;
    }

    if (feeder.name === '') {
      try {
        await fetchCreateNewDevice(newFeeder, Devices.FEEDER);
        addNewFeeder(newFeeder);
      } catch (e) {
        console.log(e);
      }
      onClose();
    } else {
      try {
        await fetchUpdateDevice(feeder.name, newFeeder, Devices.FEEDER);
        changeFeeder(feeder.name, newFeeder);
      } catch (e) {
        console.log(e);
      }
      setIsReadOnly(true);
    }
  };

  const handleChancel = () => {
    if (feeder.name !== '') {
      feederNameInput.setValue(feeder.name);
      timeBetweenFeedings.setTime(feeder.timeBetweenFeedings);
      timeOfFeeding.setTime(feeder.timeOfFeeding);
      setIsReadOnly(true);
    } else onClose();
  };

  const handleDelete = async (e: React.MouseEvent<HTMLButtonElement>) => {
    e.preventDefault();
    try {
      await fetchRemoveDevice(feeder.name, Devices.FEEDER);
      deleteFeeder(feeder.name);
    } catch (e) {
      console.log(e);
    }
  };

  return (
    <form
      className={styles.form}
      onSubmit={handleSubmit}
      style={{ height: '160px' }}
    >
      <div className={styles.properties}>
        <TextInput
          description='Название кормушки:'
          inputReturn={feederNameInput}
          disabled={isReadOnly}
          required
        />
        <TimeInput
          description='Время между кормлением:'
          timeReturn={timeBetweenFeedings}
          readonly={isReadOnly}
          withHours={true}
        />
        <TimeInput
          description='Время кормления:'
          timeReturn={timeOfFeeding}
          readonly={isReadOnly}
        />
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
              title='Удалить кормушку'
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
                  device='кормушку'
                  nameOfDevice={feeder.name}
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
