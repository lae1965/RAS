import { useEffect } from 'react';
import { Route, Routes } from 'react-router';

import { Main } from '../../Pages/Main/Main';
import { Layout } from '../Layout/Layout';
import { ConfigFilter } from '../../Pages/ConfigFilter/ConfigFilter';
import { FilterType, useFilterStore } from '../../store/filterStore';
import { fetchGetAllDevices } from '../../api/deviceAPI';
import { FeederType, useFeederStore } from '../../store/feederStore';
import { ConfigFeeder } from '../../Pages/ConfigFider/ConfigFeeder';
import { useSSE } from '../../hooks/useSSE';
import { Devices } from '../../types';

const App = () => {
  const { setFilterList } = useFilterStore();
  const { setFeederList } = useFeederStore();

  useSSE();

  useEffect(() => {
    (async () => {
      const filterList: FilterType[] = await fetchGetAllDevices(
        Devices.DRUM_FILTER
      );
      setFilterList(filterList);
      const feederList: FeederType[] = await fetchGetAllDevices(Devices.FEEDER);
      setFeederList(feederList);
    })();
    // eslint-disable-next-line react-hooks/exhaustive-deps
  }, []);

  return (
    <Layout>
      <Routes>
        <Route path='/' element={<Main />} />
        <Route path='/config/filters' element={<ConfigFilter />} />
        <Route path='/config/feeders' element={<ConfigFeeder />} />
      </Routes>
    </Layout>
  );
};

export default App;
