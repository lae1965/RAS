import { deviceMessageList } from '../../../constants';
import { DeviceMessageType } from '../../../types';
import styles from './DeviceMessages.module.css';

interface DeviceMessagesProps extends React.HTMLAttributes<HTMLUListElement> {
  messages: DeviceMessageType[];
  isShow: boolean;
}

export const DeviceMessages: React.FC<DeviceMessagesProps> = ({
  messages,
  isShow,
}) => {
  return (
    <ul className={styles.list}>
      {isShow &&
        messages.map((message, index) => {
          const h = Math.floor(message.time / 3600);
          let hours;
          if (h) hours = h.toString().padStart(2, '0');
          const minutes = Math.floor((message.time % 3600) / 60)
            .toString()
            .padStart(2, '0');
          const seconds = Math.floor(message.time % 60)
            .toString()
            .padStart(2, '0');
          return (
            <li key={index} className={styles.message}>
              {deviceMessageList[message.message]}
              <span>
                {h > 0 && <>{hours}:</>}
                {minutes}:{seconds}
              </span>
            </li>
          );
        })}
    </ul>
  );
};
