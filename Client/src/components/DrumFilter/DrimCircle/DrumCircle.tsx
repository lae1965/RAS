import styles from './DrumCircle.module.css';

interface DrumCircleProps extends React.HTMLAttributes<HTMLDivElement> {
  rotation: number;
}

export const DrumCircle: React.FC<DrumCircleProps> = ({ rotation }) => {
  return (
    <div className={styles.colorLayer}>
      <div
        className={styles.circle}
        style={{ transform: `rotate(${rotation}deg)` }}
      >
        <div className={styles.diameter}></div>
        <div className={`${styles.diameter} ${styles.diameter2}`}></div>
        <div className={`${styles.diameter} ${styles.diameter3}`}></div>
        <div className={`${styles.diameter} ${styles.diameter4}`}></div>
      </div>
    </div>
  );
};
