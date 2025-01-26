import { useEffect, useState } from 'react';

import styles from './DeviceMessages.module.css';

import { DeviceType, FeederActions, FilterActions } from '../../../constants';
import { getMessagesList } from '../../../util/getMessagesList';

interface DeviceMessagesProps extends React.HTMLAttributes<HTMLUListElement> {
  device: DeviceType;
  action: FilterActions | FeederActions;
  isShowMessages: boolean;
}

export const DeviceMessages: React.FC<DeviceMessagesProps> = ({
  device,
  action,
  isShowMessages,
}) => {
  const [messageList, setMessageList] = useState<string[]>([]);

  useEffect(() => {
    if (isShowMessages) setMessageList(getMessagesList(device, action));
    else setMessageList([]);
  }, [action, device, isShowMessages]);

  return (
    <ul className={styles.list}>
      {messageList.map((message, index) => (
        <li key={index} className={styles.message}>
          {message}
        </li>
      ))}
    </ul>
  );
};
