import styles from './Feeder.module.css';
import { Aquarium } from '../Icons/Aquarium';
import { FishFood } from '../Icons/FishFood';

interface PropFeeder extends React.HTMLAttributes<HTMLDivElement> {
  isFeeding: boolean;
}

export const Feeder: React.FC<PropFeeder> = ({ isFeeding }) => {
  return (
    <div className={styles.feeder}>
      <div className={styles.fishFood}>
        <FishFood />
        {isFeeding &&
          Array(8)
            .fill(null)
            .map((_, index) => (
              <div
                key={index}
                className={`${styles.food} ${styles[`food${index + 1}`]}`}
              ></div>
            ))}
      </div>
      <Aquarium className={styles.aquarium} />
    </div>
  );
};
