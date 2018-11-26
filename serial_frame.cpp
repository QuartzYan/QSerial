#include "serial_frame.h"

SerialFrame::SerialFrame(const byte id)
  :id_(id), frame_head_1_(0xFA), frame_head_2_(0xAA)
{
  RcvData_Len = 0;
  SenData_Len = 0;
}

SerialFrame::~SerialFrame()
{

}

bool SerialFrame::ReadFrame(const byte buf)
{
  static byte rcvBuf_[MAX_RcvBUFF_LEN] = {0,};
  static int frameLength_ = 0;
  static int rcvIndex_ = 0;

  rcvBuf_[rcvIndex_] = buf;
  rcvIndex_++;
  if(rcvIndex_ < 5)
  {
    if(rcvIndex_ == 1 && rcvBuf_[0] == frame_head_1_)
    {
      return false;
    }
    else if(rcvIndex_ == 2 && rcvBuf_[1] == frame_head_2_)//head
    {
      return false;
    }
    else if(rcvIndex_ == 3 && rcvBuf_[2] == id_)//id
    {
      return false;
    }
    else if(rcvIndex_ == 4 && rcvBuf_[3] > 0)//len
    {
      frameLength_ = buf;
      return false;
    }
    else
    {
      rcvIndex_ = 0;
      frameLength_ = 0;
      return false;
    }
  }
  else
  {
    if(rcvIndex_ < frameLength_)
    {
      RcvData[rcvIndex_ - 5] = buf;
      return false;
    }
    else
    {
      if(DelDataZero(rcvBuf_, frameLength_))
      {
        rcvIndex_ = 0;
        frameLength_ = 0;
        return true;
      }
      else
      {
        rcvIndex_ = 0;
        frameLength_ = 0;
        return false;
      }
    }
  }
  return false;
}

int SerialFrame::BuildFrame(byte *buf, int length)
{
  byte b0 = 0;
  byte b1 = 0;

  SenData[0] = frame_head_1_;
  SenData[1] = frame_head_2_;
  SenData[2] = id_;

  int j = 4;
  for (int i = 0; i < length; i++)
  {
    b0 = buf[i];
    if (b0 == frame_head_2_ && b1 == frame_head_1_)
    {
      SenData[j] = b0;
      j++;
      SenData[j] = 0x00;
      j++;
    }
    else
    {
      SenData[j] = b0;
      j++;
    }
    b1 = b0;
  }
  SenData[3] = j + 1;

  SenData[j] = AddDataSum(SenData, j);

  SenData_Len = j + 1;
  return SenData_Len;
}

bool SerialFrame::CheckDataSum(byte *data, const int data_L, const byte checksum)
{
  byte sum = 0x00;
  for (int i = 0; i < data_L; i++)
  {
    sum += data[i];
  }
  if (sum == checksum)
  {
    return true;
  }
  else
  {
    return false;
  }
}

byte SerialFrame::AddDataSum(byte *data, const int data_L)
{
  byte sum = 0x00;
  for (int i = 0; i < data_L; i++)
  {
    sum += data[i];
  }
  return sum;
}

bool SerialFrame::DelDataZero(byte *data, const int data_L)
{
  if (CheckDataSum(data, data_L-1, data[data_L - 1]) == false)
  {
    return false;
  }
  byte b0 = 0x00;
  byte b1 = 0x00;
  byte b2 = 0x00;

  int j = 0;
  for (int i = 4; i < data_L - 1; i++)
  {
    b0 = data[i];
    if (b0 == 0x00 && b1 == frame_head_2_ && b2 == frame_head_1_)
    {
      ;
    }
    else
    {
      RcvData[j] = b0;
      j++;
    }
    b2 = b1;
    b1 = b0;
  }
  RcvData_Len = j;

  return true;
}

int SerialFrame::bytesToInt(byte *bytes)
{
  int addr = bytes[3];
  addr |= (bytes[2] << 8);
  addr |= (bytes[1] << 16);
  addr |= (bytes[0] << 24);
  return addr;
}

void SerialFrame::intToByte(int i, byte *bytes)
{
  bytes[3] = (byte)(0xff & i);
  bytes[2] = (byte)((0xff00 & i) >> 8);
  bytes[1] = (byte)((0xff0000 & i) >> 16);
  bytes[0] = (byte)((0xff000000 & i) >> 24);
  return;
}

float SerialFrame::byteToFloat(byte *bytes)
{
  int accum = 0;
  accum |= (bytes[3] & 0xff) << 0;
  accum |= (bytes[2] & 0xff) << 8;
  accum |= (bytes[1] & 0xff) << 16;
  accum |= (bytes[0] & 0xff) << 24;
  union
  {
    int i;
    float f;
  } u;
  u.i = accum;
  return u.f;
}

void SerialFrame::floatToByte(float f, byte *bytes)
{
  byte *pdata = (byte *)&f;
  for (int i = 3; i >= 0; i--)
  {
    bytes[i] = *pdata++;
  }
}
