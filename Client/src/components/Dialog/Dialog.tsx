import styles from './Dialog.module.css';

export const Dialog: React.FC<
  React.DialogHTMLAttributes<HTMLDialogElement>
> = ({ children, className }) => {
  return (
    <dialog open className={`${styles.dialog} ${className}`}>
      {children}
    </dialog>
  );
};
