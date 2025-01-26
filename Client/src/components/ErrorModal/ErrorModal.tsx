import { Attention } from '../Icons/Attention';
import styles from './ErrorModal.module.css';

interface ErrorModalProps
  extends React.DialogHTMLAttributes<HTMLDialogElement> {
  text: string;
}

export const ErrorModal: React.FC<ErrorModalProps> = ({ text }) => {
  return (
    <dialog className={styles.errorModal} open>
      <Attention />
      {text}
    </dialog>
  );
};
