import styles from './DrumFilter.module.css';

import { DrumSpray } from './DrumSpray/DrumSpray';
import { DrumCircle } from './DrimCircle/DrumCircle';

interface PropsDrumFilter extends React.HTMLAttributes<HTMLDivElement> {
  isSpray: boolean;
  rotation: number;
}

export const DrumFilter: React.FC<PropsDrumFilter> = ({
  isSpray,
  rotation,
}) => {
  return (
    <div className={styles.filterPicture}>
      <DrumSpray isSpray={isSpray} />
      <DrumCircle rotation={rotation} />
    </div>
  );
};
