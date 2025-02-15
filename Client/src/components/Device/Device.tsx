import { useEffect, useState } from 'react';

import styles from './Device.module.css';

import { Indicator } from '../Icons/Indicator/Indicator';
import { deviceButtonsList } from '../../constants';
import { DeviceMessages } from './DeviceMessages/DeviceMessages';
import { DrumFilter } from '../DrumFilter/DrumFilter';
import { Feeder } from '../Feeder/Feeder';
import { fetchShortCommands } from '../../api/deviceAPI';
import {
  Devices,
  DeviceActions,
  IndicatorColors,
  DeviceType,
} from '../../types';

interface PropsDevice extends React.ButtonHTMLAttributes<HTMLButtonElement> {
  deviceType: Devices;
  device: DeviceType;
}

export const Device: React.FC<PropsDevice> = ({ deviceType, device }) => {
  const [rotation, setRotation] = useState<number>(0);
  const [color, setColor] = useState<IndicatorColors>('green');

  useEffect(() => {
    if (deviceType === Devices.FEEDER) return;
    let intervalId: number | null = null;
    if (
      device.internal.action != undefined &&
      device.internal.action > DeviceActions.NO_ACTIONS
    ) {
      intervalId = setInterval(() => {
        setRotation((prev) => (prev + 1) % 360);
      }, 60);
    } else if (intervalId) {
      clearInterval(intervalId);
    }

    if (intervalId) return () => clearInterval(intervalId);
  }, [device.internal, deviceType]);

  useEffect(() => {
    setColor(device.isPowerOn ? 'green' : 'grey');
  }, [device.isPowerOn]);

  const handleChangePower = async () => {
    try {
      await fetchShortCommands(device.name, deviceType, 'change_power');
    } catch {
      return;
    }
  };

  const handleClick = async (index: number) => {
    let command;
    if (index === 0) {
      if (deviceType === Devices.DRUM_FILTER) command = 'forced_rotation';
      else command = 'forced_feeding';
    } else command = 'forced_washing';

    try {
      await fetchShortCommands(device.name, deviceType, command);
    } catch {
      return;
    }
  };

  return (
    <div className={styles.filter}>
      <div
        className={styles.drumFilterWrapper}
        data-poweroff={!device.isPowerOn}
      >
        <button
          type='button'
          className={styles.indicator}
          onClick={handleChangePower}
        >
          <Indicator color={color} />
        </button>
        <h1 className={styles.heading}>{device.name}</h1>
        <div className={styles.drumFilter}>
          <div>
            {(deviceType === Devices.DRUM_FILTER && (
              <DrumFilter
                isSpray={device.internal.action === DeviceActions.WASHING}
                rotation={rotation}
              />
            )) || (
              <Feeder
                isFeeding={device.internal.action === DeviceActions.FEEDING}
              />
            )}
            {device.internal.isEmergencyLevel && (
              <div className={styles.error}>
                <Indicator color='red' className={styles.smallIndicator} />
                <p>Аварийный уровень!!!</p>
              </div>
            )}
          </div>
          <DeviceMessages
            messages={device.internal.messages}
            isShow={device.isPowerOn}
          />
        </div>
        <div className={styles.addActions}>
          {deviceButtonsList[deviceType].map((buttonText, index) => (
            <button
              key={index}
              className={styles.actionButton}
              disabled={
                !device.isPowerOn ||
                device.internal.action > DeviceActions.NO_ACTIONS
              }
              onClick={() => {
                handleClick(index);
              }}
            >
              {buttonText}
            </button>
          ))}
        </div>
      </div>
    </div>
  );
};
