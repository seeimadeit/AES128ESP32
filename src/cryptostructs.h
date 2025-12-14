#pragma once

#define AES128BYTES 16
#define AES128IVSIZE 12
// #define NUMBYTESX  32


struct databytes {
  size_t size;
  unsigned char* data;
  databytes(size_t s);
  ~databytes();
  void cleardata();

  private:
  void deletedata(); 
};


/** \class encryptedData
\brief variable size encrypted data*/
struct encryptedData : public databytes
{
  encryptedData(size_t datasize) : databytes(datasize){}
};

/** \class ivData
\brief IV data for AES128 is 12 bytes*/
struct ivData : public databytes
{
  ivData() : databytes(AES128IVSIZE){}
};

/**\class MasterKeyData
\brief masterkey data for AES128 is 16bytes */
struct MasterKeyData : public databytes
{
  MasterKeyData(): databytes(AES128BYTES /*NUMBYTESX*/){};
};

