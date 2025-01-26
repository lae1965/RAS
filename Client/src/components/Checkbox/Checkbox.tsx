import { UseInputReturn } from '../../hooks/useInput';
import styles from './Checkbox.module.css';

interface CheckboxProps extends React.InputHTMLAttributes<HTMLInputElement> {
  description?: string;
  inputReturn: UseInputReturn<boolean>;
}

export const Checkbox: React.FC<CheckboxProps> = ({
  description = '',
  inputReturn,
  className,
  ...props
}) => {
  return (
    <label className={`${styles.checkbox} ${className}}`}>
      <input
        type='checkbox'
        checked={inputReturn.input.value}
        name={inputReturn.input.name}
        onChange={inputReturn.input.onChange}
        {...props}
      />
      {description}
    </label>
  );
};
