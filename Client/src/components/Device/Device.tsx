import { useEffect, useState } from 'react';

import styles from './Device.module.css';

import { Indicator } from '../Icons/Indicator/Indicator';
import {
  deviceButtonsList,
  Devices,
  FeederActions,
  FilterActions,
  IndicatorColors,
} from '../../constants';
import { DeviceMessages } from './DeviceMessages/DeviceMessages';
import { FilterType } from '../../store/filterStore';
import { DrumFilter } from '../DrumFilter/DrumFilter';
import { FeederType } from '../../store/feederStore';
import { Feeder } from '../Feeder/Feeder';
import { fetchChangeDevicePower } from '../../api/deviceAPI';

interface PropsDevice extends React.ButtonHTMLAttributes<HTMLButtonElement> {
  deviceType: Devices;
  device: FilterType | FeederType;
}

export const Device: React.FC<PropsDevice> = ({ deviceType, device }) => {
  const [rotation, setRotation] = useState<number>(0);
  const [color, setColor] = useState<IndicatorColors>('green');

  useEffect(() => {
    if (deviceType === Devices.FEEDER) return;
    let intervalId: number | null = null;
    if (
      device.internal!.action != undefined &&
      device.internal!.action > FilterActions.NO_ACTIONS
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
      await fetchChangeDevicePower(device.name, deviceType);
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
                isSpray={device.internal!.action === FilterActions.WASHING}
                rotation={rotation}
              />
            )) || (
              <Feeder
                isFeeding={device.internal!.action === FeederActions.FEEDING}
              />
            )}
            {device.internal!.isEmergencyLevel && (
              <div className={styles.error}>
                <Indicator color='red' className={styles.smallIndicator} />
                <p>Аварийный уровень!!!</p>
              </div>
            )}
          </div>
          <DeviceMessages
            action={device.internal!.action}
            device={device}
            isShowMessages={device.isPowerOn}
          />
        </div>
        <div className={styles.addActions}>
          {deviceButtonsList[deviceType].map((buttonText, index) => (
            <button
              key={index}
              className={styles.actionButton}
              disabled={!device.isPowerOn}
            >
              {buttonText}
            </button>
          ))}
        </div>
      </div>
    </div>
  );
};
