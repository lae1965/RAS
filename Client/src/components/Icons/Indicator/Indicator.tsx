import { IndicatorColors } from '../../../constants';
import styles from './Indicator.module.css';

interface IndicatorProps extends React.HTMLAttributes<HTMLDivElement> {
  color?: IndicatorColors;
}

const gradient = {
  red: '#ff0000, #ff0022, #ff0044, #ff0066, #ff0088, #ff00aa',
  green: '#366e36, #2d6d2d, #55903f, #7db252, #95c94d',
  yellow: '#ffff00, #ffff22, #ffff44, #ffff66, #ffff88',
  grey: '#3d3d3d, #5e5e5e, #7f7f7f, #a0a0a0, #c1c1c1',
};

export const Indicator: React.FC<IndicatorProps> = ({
  color = 'grey',
  className,
}) => {
  return (
    <div
      className={`${styles.circle} ${className}`}
      style={{
        background: `linear-gradient(${gradient[color]})`,
      }}
    >
      <div
        className={`${styles.glare} ${
          color === 'yellow' ? styles.glareForYellow : ''
        }`}
      />
    </div>
  );
};
