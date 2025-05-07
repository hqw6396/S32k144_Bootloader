#ifndef UDS_SERVICE_H_
#define UDS_SERVICE_H_

#include "include.h"      

typedef enum
{
  udsSession_Defalut=1,
  udsSession_Programing=2,
  udsSession_Extended=3,  
}enum_udsSession;

typedef enum
{
  udsSecurityLevel_None,
  udsSecurityLevel_Level1,
  udsSecurityLevel_Level2,
  udsSecurityLevel_Level3,
}enum_udsSecurityLevel;

void UdsServiceProcessFunction(uint8_t *pTpData,uint32_t tpDataLength,uint8_t FunctionalAddress);
#endif 
