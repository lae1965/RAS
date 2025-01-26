import styles from './DeleteDialog.module.css';

interface DeleteDialogProps extends React.HTMLAttributes<HTMLDivElement> {
  device: 'фильтр' | 'кормушку';
  nameOfDevice: string;
  handleDelete: (e: React.MouseEvent<HTMLButtonElement>) => void;
  setIsOpenDialog: React.Dispatch<React.SetStateAction<boolean>>;
}

export const DeleteDialog: React.FC<DeleteDialogProps> = ({
  device,
  nameOfDevice,
  handleDelete,
  setIsOpenDialog,
}) => {
  return (
    <div className={styles.action}>
      <h3>
        Вы уверены, что хотите удалить {device} "{nameOfDevice}"?
      </h3>
      <button onClick={handleDelete}>Удалить</button>
      <button
        className={styles.greyButton}
        onClick={() => {
          setIsOpenDialog(false);
        }}
      >
        Отменить
      </button>
    </div>
  );
};
