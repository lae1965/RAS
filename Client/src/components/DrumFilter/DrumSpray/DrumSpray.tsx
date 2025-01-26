import styles from './DrumSpray.module.css';

interface DrumSprayProps extends React.HTMLAttributes<HTMLDivElement> {
  isSpray?: boolean;
}

export const DrumSpray: React.FC<DrumSprayProps> = ({ isSpray = false }) => {
  return (
    <div className={styles.sprayIcon}>
      <div className={styles.nozzle}></div>
      {isSpray && (
        <div className={styles.waterSpray}>
          {Array(9)
            .fill(null)
            .map((_, index) => (
              <div
                key={index}
                className={`${styles.waterDrop} ${styles[`drop${index + 1}`]}`}
              ></div>
            ))}
        </div>
      )}
    </div>
  );
};
