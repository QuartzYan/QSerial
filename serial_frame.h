/******************************************************
** Author: QuartzYan
** Version: 1.0.0
** Date: 2018.11.26
** Description: 适用于串行通信时的数据组包与解包
*******************************************************/

#ifndef SERIAL_FRAME_H
#define SERIAL_FRAME_H

#define MAX_RcvBUFF_LEN 1024
#define MAX_SenBUFF_LEN 1024

typedef unsigned char byte;

class SerialFrame {
public:
  SerialFrame(const byte id);
  ~SerialFrame();

  byte RcvData[MAX_RcvBUFF_LEN];
  unsigned int RcvData_Len;
  byte SenData[MAX_SenBUFF_LEN];
  unsigned int SenData_Len;

  bool ReadFrame(const byte buf);
  int  BuildFrame(byte* buf, int length);

  static int bytesToInt(byte* bytes);
  static void  intToByte(int i, byte *bytes);
  static float byteToFloat(byte *bytes);
  static void floatToByte(float f, byte *bytes);

private:
  bool CheckDataSum(byte *data, const int data_L, const byte checksum);
  byte AddDataSum(byte *data, const int data_L);
  bool DelDataZero(byte *data, const int data_L);

private:
  const byte id_;
  const byte frame_head_1_;
  const byte frame_head_2_;
};

#endif // SERIAL_FRAME_H
