import { useState } from 'react';
import styles from './ConfigFilter.module.css';

import { useFilterStore } from '../../store/filterStore';
import { Link } from 'react-router';
import { FilterProperty } from '../../components/DeviceProperty/FilterProperty';

export const ConfigFilter = () => {
  const [isOpenDialogAdd, setIsOpenDialogAdd] = useState<boolean>(false);
  const { filterList, initialFilter } = useFilterStore();

  return (
    <div className={styles.nav}>
      <div className={styles.drive}>
        <h1 className={styles.heading}>Барабанные фильтры</h1>
        {(!!filterList.length && (
          <ul className={styles.list}>
            {filterList.map((filter) => (
              <li key={filter.name}>
                <FilterProperty
                  onClose={() => {}}
                  filter={filter}
                  readonly={true}
                />
              </li>
            ))}
          </ul>
        )) || <p>В приложении не определено ни одного фильтра</p>}

        <div className={styles.addFilter}>
          <button
            className={styles.button}
            type='button'
            onClick={() => {
              setIsOpenDialogAdd(true);
            }}
          >
            Добавить новый фильтр
          </button>
          {isOpenDialogAdd && (
            <FilterProperty
              onClose={() => setIsOpenDialogAdd(false)}
              filter={initialFilter}
            />
          )}
        </div>
      </div>
      <div className={styles.pagesNav}>
        <Link to='/'>На главную</Link>
        <Link to='/config/feeders'>Конфигурация кормушек</Link>
      </div>
    </div>
  );
};
