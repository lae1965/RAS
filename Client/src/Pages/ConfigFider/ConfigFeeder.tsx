import { useState } from 'react';
import styles from '../ConfigFilter/ConfigFilter.module.css';

import { useFeederStore } from '../../store/feederStore';
import { Link } from 'react-router';
import { FeederProperty } from '../../components/DeviceProperty/FeederProperty';

export const ConfigFeeder = () => {
  const [isOpenDialogAdd, setIsOpenDialogAdd] = useState<boolean>(false);
  const { feederList, initialFeeder } = useFeederStore();

  return (
    <div className={styles.nav}>
      <div className={styles.drive}>
        <h1 className={styles.heading}>Кормушки</h1>
        {(!!feederList.length && (
          <ul className={styles.list}>
            {feederList.map((feeder) => (
              <li key={feeder.name}>
                <FeederProperty
                  onClose={() => {}}
                  feeder={feeder}
                  readonly={true}
                />
              </li>
            ))}
          </ul>
        )) || <p>В приложении не определено ни одной кормушки</p>}

        <div className={styles.addFeeder}>
          <button
            className={styles.button}
            type='button'
            onClick={() => {
              setIsOpenDialogAdd(true);
            }}
          >
            Добавить новую кормушку
          </button>
          {isOpenDialogAdd && (
            <FeederProperty
              onClose={() => setIsOpenDialogAdd(false)}
              feeder={initialFeeder}
            />
          )}
        </div>
      </div>
      <div className={styles.pagesNav}>
        <Link to='/'>На главную</Link>
        <Link to='/config/filters'>Конфигурация фильтров</Link>
      </div>
    </div>
  );
};
