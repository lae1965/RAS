import { UseInputReturn } from '../../hooks/useInput';
import styles from './NumberInput.module.css';

export interface NumberInputProps
  extends React.InputHTMLAttributes<HTMLInputElement> {
  min: number;
  max: number;
  inputReturn: UseInputReturn<string>;
  handlerBlur: (e: React.ChangeEvent<HTMLInputElement>) => void;
}

export const NumberInput: React.FC<NumberInputProps> = ({
  min,
  max,
  inputReturn,
  handlerBlur,
  className,
  ...props
}) => {
  const handlerFocus = (e: React.ChangeEvent<HTMLInputElement>): void => {
    let newValue: string;
    const value = +e.target.value || 0;
    if (value === 0) newValue = '';
    else newValue = value.toString();

    inputReturn.setValue(newValue);
  };

  return (
    <input
      type='number'
      className={`${styles.input} ${className}`}
      min={min}
      max={max}
      {...inputReturn.input}
      onFocus={handlerFocus}
      onBlur={handlerBlur}
      {...props}
    />
  );
};
