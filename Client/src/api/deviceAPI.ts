import axios from 'axios';
import { devices, Devices, DeviceType } from '../constants';

const getURL = (deviceType: Devices) =>
  `http://localhost:8080/api/${devices[deviceType]}`;

export const fetchGetAllDevices = async (deviceType: Devices) => {
  try {
    const response = await axios.get(getURL(deviceType));
    return response.data;
  } catch (e) {
    console.log(e);
    throw e;
  }
};

export const fetchCreateNewDevice = async (
  device: DeviceType,
  deviceType: Devices
) => {
  try {
    // eslint-disable-next-line @typescript-eslint/no-unused-vars
    const { internal, ...rest } = device;
    const response = await axios.post(getURL(deviceType), {
      ...rest,
    });
    if (response.status !== 201)
      throw new Error(`failed saving new ${devices[deviceType]}`);
  } catch (e) {
    console.log(e);
    throw e;
  }
};

export const fetchUpdateDevice = async (
  oldDeviceName: string,
  device: DeviceType,
  deviceType: Devices
) => {
  try {
    // eslint-disable-next-line @typescript-eslint/no-unused-vars
    const { internal, ...rest } = device;
    const response = await axios.put(`${getURL(deviceType)}/${oldDeviceName}`, {
      ...rest,
    });
    if (response.status !== 200)
      throw new Error(`failed saving ${devices[deviceType]}'s changes`);
  } catch (e) {
    console.log(e);
    throw e;
  }
};

export const fetchRemoveDevice = async (
  deviceName: string,
  deviceType: Devices
) => {
  try {
    const response = await axios.delete(`${getURL(deviceType)}/${deviceName}`);
    if (response.status !== 204)
      throw new Error(`failed removing ${devices[deviceType]}`);
  } catch (e) {
    console.log(e);
    throw e;
  }
};

export const fetchChangeDevicePower = async (
  deviceName: string,
  deviceType: Devices
) => {
  try {
    const url = `${getURL(deviceType)}/change_power/${deviceName}`;
    const response = await axios.get(url);
    if (response.status !== 204)
      throw new Error(
        `failed changing power on ${devices[deviceType]} ${deviceName}`
      );
  } catch (e) {
    console.log(e);
    throw e;
  }
};
