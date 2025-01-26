import { useState } from 'react';

export interface UseInputReturn<T> {
  setValue: React.Dispatch<React.SetStateAction<T>>;
  input: {
    name: string;
    value?: T;
    checked?: boolean;
    onChange: (e: React.ChangeEvent<HTMLInputElement>) => void;
  };
}

export const useInput = <T>(
  name: string,
  initialValue: T,
  validateFunc?: (e: React.ChangeEvent<HTMLInputElement>) => T
): UseInputReturn<T> => {
  const [value, setValue] = useState<T>(initialValue);

  const handleChange = (e: React.ChangeEvent<HTMLInputElement>) => {
    let value: T;
    if (e.target.type === 'checkbox') {
      value = e.target.checked as unknown as T;
    } else if (validateFunc) value = validateFunc(e);
    else value = e.target.value as unknown as T;
    setValue(value);
  };

  return {
    setValue,
    input: {
      name,
      value,
      onChange: handleChange,
    },
  };
};
