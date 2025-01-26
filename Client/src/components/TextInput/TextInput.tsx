import { UseInputReturn } from '../../hooks/useInput';
import styles from './TextInput.module.css';

interface TextInputProps extends React.InputHTMLAttributes<HTMLInputElement> {
  description?: string;
  inputReturn: UseInputReturn<string>;
}

export const TextInput: React.FC<TextInputProps> = ({
  inputReturn,
  description = '',
  className,
  ...props
}) => {
  return (
    <label className={`${styles.name} ${className}`}>
      {description}
      <input type='text' maxLength={16} {...inputReturn.input} {...props} />
    </label>
  );
};
