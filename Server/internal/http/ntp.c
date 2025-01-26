#include "ntp.h"

static time_t getNtpTime(void) {
  int                sockfd;
  struct sockaddr_in serverAddr;
  NtpPacket          packet;
  unsigned long      secondsCountBetween1900and1970years = 2208988800UL;

  memset(&packet, 0, sizeof(NtpPacket));

  // Создание сокета
  if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
    perror("socket");
    return -1;
  }

  // Настройка адреса сервера
  memset(&serverAddr, 0, sizeof(serverAddr));
  serverAddr.sin_family      = AF_INET;
  serverAddr.sin_port        = htons(123);
  serverAddr.sin_addr.s_addr = inet_addr("pool.ntp.org");

  // Инициализация пакета
  packet.li_vn_mode = (0 << 6) | (4 << 3) | 3;  // 0 - no leap, version 4, client mode

  // Отправка запроса
  if (sendto(sockfd, (char *)&packet, sizeof(NtpPacket), 0, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0) {
    perror("sendto");
    close(sockfd);
    return -1;
  }

  // Получение ответа
  if (recvfrom(sockfd, (char *)&packet, sizeof(NtpPacket), 0, NULL, NULL) < 0) {
    perror("recvfrom");
    close(sockfd);
    return -1;
  }

  close(sockfd);

  return ntohl(packet.txTimeSec) - secondsCountBetween1900and1970years;  // NTP -> UNIX
}

bool syncSystemTime(void) {
  time_t newTime = getNtpTime();
  if (newTime == -1) return false;

  struct timeval tv;
  tv.tv_sec  = newTime;
  tv.tv_usec = 0;

  if (settimeofday(&tv, NULL) < 0) {
    perror("settimeofday");
    return false;
  }

  return true;
}