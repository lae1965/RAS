import { useEffect } from 'react';
import styles from './TimeInput.module.css';
import { useInput } from '../../hooks/useInput';
import { NumberInput } from '../NumberInput/NumberInput';
import { TimeReturn } from '../../hooks/useTimeInput';
import { ErrorModal } from '../ErrorModal/ErrorModal';

interface TimeInputProps extends React.LabelHTMLAttributes<HTMLLabelElement> {
  description?: string;
  timeReturn: TimeReturn;
  readonly?: boolean;
}

export const TimeInput: React.FC<TimeInputProps> = ({
  description = '',
  timeReturn,
  readonly = false,
}) => {
  const validateValue = (
    e: React.ChangeEvent<HTMLInputElement>,
    max: number
  ): string => {
    const value = +e.target.value || 0;
    if (value < 0) return '0';
    if (value > max) return max.toString();
    return value.toString();
  };

  const minutesInput = useInput(
    'minutes',
    Math.floor(timeReturn.time / 60)
      .toString()
      .padStart(2, '0'),
    (e) => validateValue(e, 99)
  );
  const secondsInput = useInput(
    'seconds',
    (timeReturn.time % 60).toString().padStart(2, '0'),
    (e) => validateValue(e, 59)
  );

  useEffect(() => {
    minutesInput.setValue(
      Math.floor(timeReturn.time / 60)
        .toString()
        .padStart(2, '0')
    );
    secondsInput.setValue((timeReturn.time % 60).toString().padStart(2, '0'));
    // eslint-disable-next-line react-hooks/exhaustive-deps
  }, [timeReturn.time]);

  const handlerBlur = (e: React.ChangeEvent<HTMLInputElement>): void => {
    let newValue: string;
    const value = +e.target.value || 0;
    if (value > 9) newValue = value.toString();
    else newValue = `0${value}`;

    if (e.target.name === 'minutes') minutesInput.setValue(newValue);
    else secondsInput.setValue(newValue);

    timeReturn.setTime(
      +minutesInput.input.value! * 60 + +secondsInput.input.value!
    );
  };

  return (
    <label className={styles.label}>
      {description}
      <div className={styles.time}>
        <NumberInput
          min={0}
          max={99}
          inputReturn={minutesInput}
          handlerBlur={handlerBlur}
          disabled={readonly}
        />
        :
        <NumberInput
          min={0}
          max={59}
          inputReturn={secondsInput}
          handlerBlur={handlerBlur}
          disabled={readonly}
        />
        {timeReturn.error && (
          <ErrorModal text='Значение не должно быть нулевым' />
        )}
      </div>
    </label>
  );
};
