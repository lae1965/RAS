import { Link } from 'react-router';

import styles from './Main.module.css';

import { Devices, indicatorData, infoData } from '../../constants';
import { Indicator } from '../../components/Icons/Indicator/Indicator';
import { Device } from '../../components/Device/Device';
import { useFilterStore } from '../../store/filterStore';
import { useFeederStore } from '../../store/feederStore';

export const Main = () => {
  const filterList = useFilterStore((state) => state.filterList);
  const feederList = useFeederStore((state) => state.feederList);

  return (
    <>
      <section>
        <ul className={styles.RASInfo}>
          {infoData.map((item, index) => (
            <li key={index} className={styles.infoPair}>
              <div className={styles.name}>{item.name}</div>
              <div className={styles.value}>
                {`${item.value}${item.measurement}`}
              </div>
            </li>
          ))}
        </ul>
      </section>
      <section>
        <div className={styles.power}>
          {indicatorData.map((item, index) => (
            <div className={styles.indicator} key={index}>
              <Indicator color='green' />
              <div className={styles.text}>{item}</div>
            </div>
          ))}
        </div>
      </section>
      <section className={styles.content}>
        <div className={styles.devices}>
          <div className={styles.deviceList}>
            {!!filterList.length &&
              filterList.map((filter) => (
                <Device
                  key={filter.name}
                  deviceType={Devices.DRUM_FILTER}
                  device={filter}
                />
              ))}
          </div>
          <div className={styles.deviceList}>
            {!!feederList.length &&
              feederList.map((feeder) => (
                <Device
                  key={feeder.name}
                  deviceType={Devices.FEEDER}
                  device={feeder}
                />
              ))}
          </div>
        </div>
      </section>
      <nav className={styles.navigation}>
        <Link to='/config/filters'>Настройка фильтров</Link>
        <Link to='/config/feeders'>Настройка кормушек</Link>
      </nav>
    </>
  );
};
