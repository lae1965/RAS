import { useEffect, useState } from 'react';

export interface TimeReturn {
  name: string;
  time: number;
  setTime: React.Dispatch<React.SetStateAction<number>>;
  error: boolean;
  setError: React.Dispatch<React.SetStateAction<boolean>>;
}

export const useTimeInput = (name: string, initialTime: number): TimeReturn => {
  const [time, setTime] = useState<number>(initialTime);
  const [error, setError] = useState<boolean>(false);

  useEffect(() => {
    const handleClick = () => {
      setError(false);
    };
    window.addEventListener('click', handleClick);
    return () => window.removeEventListener('click', handleClick);
  }, []);

  return {
    name,
    time,
    setTime,
    error,
    setError,
  };
};
