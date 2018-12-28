#ifndef _GLOBAL_EEP_H_
#define _GLOBAL_EEP_H_

#ifdef __cplusplus
extern "C" {
#endif

void SetUserTel(const char* ptr);
int GetUserTel(char* ptr);
_Bool RoamingGprsEnable(void);
void SetRoamingGprs(_Bool bRoamingGprs);
_Bool GetRoamingGprs(void);
void SetAccelSensitivity(uint8_t ucSensitivity);
uint8_t GetAccelSensitivity(void);
void SetAccelTimeCurrState(uint16_t usTimeCurrState);
uint16_t GetAccelTimeCurrState(void);
void SetAddrFirstServ(const char* ptr);
int GetAddrFirstServ(char* ptr);
void SetAddrSecondServ(const char* ptr);
int GetAddrSecondServ(char* ptr);
void SetUseTypeServ(TYPE_SERVER eUseTypeServ);
TYPE_SERVER GetUseTypeServ(void);
void SetNameUserSimApn(const char* ptr);
int GetNameUserSimApn(char* ptr);
void SetNameUserSimPass(const char* ptr);
int GetNameUserSimPass(char* ptr);
void SetNameUserSimLogin(const char* ptr);
int GetNameUserSimLogin(char* ptr);
void SetManualModeSimFirst(_Bool bManualMode);
_Bool GetManualModeSimFirst(void);
void SetManualModeSimSecond(_Bool bManualMode);
_Bool GetManualModeSimSecond(void);
void setFlagSimSecondInstld(_Bool value);
_Bool getFlagSimSecondInstld(void);
void SetUserSimPin(const char* ptr);
int GetUserSimPin(char* ptr);
void SetPinLock(_Bool bPinLock);
_Bool GetPinLock(void);
void SetJamDetect(_Bool bJamDetect);
_Bool GetJamDetect(void);
void SetScidFirstSim(const char* ptr);
int GetScidFirstSim(char* ptr);
void SetImeiFirstGsm(const char* ptr);
int GetImeiFirstGsm(char* ptr);
void SetAccessPass(const char* ptr);
int GetAccessPass(char* ptr);
void SetMaskMessageUser(TYPE_MESSAGE eMaskMessage);
TYPE_MESSAGE GetMaskMessageUser(void);
void SetGsmFindTimeout(uint8_t uc_gsm_find_timeout);
uint8_t GetGsmFindTimeout(void);
void SetLbsFindTimeout(uint8_t uc_lbs_find_timeout);
uint8_t GetAnswerTimeout(void);
void SetAnswerTimeout(uint8_t uc_wait_answer_timeout);
uint8_t GetGprsOpenCount(void);
void SetGprsOpenCount(uint8_t uc_gprs_open_count);
uint8_t GetLbsFindTimeout(void);
void SetGsmGprsTimeout(uint8_t uc_gprs_timeout);
uint8_t GetGsmGprsTimeout(void);
void SetUserPwrDevice(USER_PWR_STATUS eUserPwrDevice);
USER_PWR_STATUS GetUserPwrDevice(void);
void SetTimeoutLowPwrMode1(uint32_t uiTimeoutLowPwrMode1);
uint32_t GetTimeoutLowPwrMode1(void);
void SetTimeoutLowPwrMode2(uint32_t uiTimeoutLowPwrMode2);
uint32_t GetTimeoutLowPwrMode2(void);
void SetTimeLowPwrMode1(uint16_t usTimeSleepLowPwrMode1);
uint16_t GetTimeLowPwrMode1(void);
void SetTimeLowPwrMode2(uint16_t usTimeSleepLowPwrMode2);
uint16_t GetTimeLowPwrMode2(void);
void SetLenDataFlashReady(uint32_t uiLenDataFlashReady);
uint32_t GetLenDataFlashReady(void);
void SetEnableUseLowPwr1Mode(_Bool bEnableUseLowPwr1Mode);
_Bool GetEnableUseLowPwr1Mode(void);
void SetEnableUseLowPwr2Mode(_Bool bEnableUseLowPwr2Mode);
_Bool GetEnableUseLowPwr2Mode(void);
void SetSleepTimeStandart(uint32_t uiSleepTimeStandart);
uint32_t GetSleepTimeStandart(void);
void SetSleepTimeFind(uint32_t uiSleepTimeFind);
uint32_t GetSleepTimeFind(void);
void SetEnableAccelToFind(_Bool bEnableAccelToFind);
_Bool GetEnableAccelToFind(void);
void SetModeDevice(TYPE_MODE_DEV eModeDevice);
TYPE_MODE_DEV GetModeDevice(void);
void SetTimeReconnect(size_t index, uint32_t time_reconnect);
void SetTimeReconnect1(uint32_t time_reconnect);
void SetTimeReconnect2(uint32_t time_reconnect);
void SetTimeReconnect3(uint32_t time_reconnect);
void SetTimeReconnect4(uint32_t time_reconnect);
void SetTimeReconnect5(uint32_t time_reconnect);
uint32_t GetTimeReconnect(size_t index);
void SetCountReConnect(uint8_t ucCountReConnect);
uint16_t GetCountReConnect(void);
void SetGpsWait(uint32_t uiGpsWait);
uint32_t GetGpsWait(void);
void SetLedEnable(_Bool bLedEnable);
_Bool GetLedEnable(void);
void SetMinTemperaturWorkDevice(int8_t cMinTemperaturWork);
int8_t GetMinTemperaturWorkDevice(void);
void SetGpsRealtime(uint16_t us_gps_real_time_record_data);
uint16_t GetGpsRealtime(void);
void SetGpsRecordtime(uint16_t us_gps_time_record_data);
uint16_t GetGpsRecordtime(void);
void SetRecordAccel(_Bool b_gps_record_accel_data);
_Bool GetRecordAccel(void);
void SetGpsRecordCourse(uint8_t uc_gps_record_course);
uint8_t GetGpsRecordCourse(void);
void SetGpsRecordDistanse(uint16_t us_gps_record_distance);
uint16_t GetGpsRecordDistanse(void);
void SetGpsRecordMinSpeed(uint16_t us_gps_record_min_speed);
uint16_t GetGpsRecordMinSpeed(void);
void SetGpioRecordTime(uint16_t usTimeRecord);
uint16_t GetGpioRecordTime(void);
void SetAGpsAddrServ(const char* ptr);
int GetAGpsAddrServ(char* ptr);
void SetAGpsTokenServ(const char* ptr);
int GetAGpsTokenServ(char* ptr);
void SetAGpsFlagParam(uint16_t usFlagParam);
uint16_t GetAGpsFlagParam(void);
void SetAGpsUseAGps(_Bool bUseAGps);
_Bool GetAGpsUseAGps(void);
void SetFlagsStatusFirmware(FRAME_FIRMWARE_TYPE eFlagsStatusFirmware);
FRAME_FIRMWARE_TYPE GetFlagsStatusFirmware(void);
void SetIntNewFirmware(uint32_t uiNameNewFirmware);
uint32_t GetIntNewFirmware(void);
void SetAddrFirmSer(const char* ptr);
int GetAddrFirmSer(char* ptr);
void SetDefAddrFirmSer(const char* ptr);
int GetDefAddrFirmSer(char* ptr);
void DriveSimpleInit(void);
void SetGpsHdopFixCoordinates(float uc_hdop_fix_coordinates);
float GetGpsHdopFixCoordinates(void);
void SetModeProtocol(TYPE_MODE_PROTOCOL eModeProtocol);
TYPE_MODE_PROTOCOL GetModeProtocol(void);
void GetCountRebootDevice(uint16_t* pCountAll, uint16_t* pCountLowPwr);
void SetCountRebootDevice(const uint16_t usCountAll, const uint16_t usCountLowPwr);
void GetGpsFind(uint16_t* pGpsValid, uint16_t* pGpsFail);
void SetGpsFind(uint16_t pGpsValid, uint16_t pGpsFail);
void GetGsmLog(uint16_t* pGsmOk, uint16_t* pGsmErr);
void SetGsmLog(const uint8_t pGsmOk, const uint8_t pGsmErr);
void GetGsmPwrErr(uint16_t* pGsmOk, uint16_t* pGsmErr, uint16_t* pLowCsq, uint16_t* pCelFail);
void SetGsmPwrErr(uint8_t GsmOk, uint8_t GsmErr, uint16_t LowCsq, uint16_t CelFail);
void GetServerConnect(uint16_t* pServConnOk, uint16_t* pServConnErr);
void SetServerConnect(uint16_t pServConnOk, uint16_t pServConnErr);
uint16_t GetGsmFind(void);
void SetGsmFind(uint16_t usGsmFindErr);
void GetGsmWorkErr(uint16_t* pGsmLowPwr, uint16_t* pGsmLowTemp);
void SetGsmWorkErr(uint16_t GsmLowPwr, uint16_t GsmLowTemp);
uint16_t GetGsmGprsErr(void);
void SetGsmGprsErr(uint16_t usGsmGprsErr);
uint16_t GetServerErr(void);
void SetServerErr(uint16_t usServerErr);
uint16_t GetDeviceWakeup(void);
void SetDeviceWakeup(uint16_t usDeviceWakeup);
uint32_t GetDeviceWorkTime(void);
void SetDeviceWorkTime(uint32_t uiDeviceWorkTime);
uint32_t GetTimePwrGps(void);
void SetTimePwrGps(uint32_t uiTimePwrGps);
uint32_t GetTimePwrGsm(void);
void SetTimePwrGsm(uint32_t uiTimePwrGsm);
uint32_t GetTimeReconnect1(void);
uint32_t GetTimeReconnect2(void);
uint32_t GetTimeReconnect3(void);
uint32_t GetTimeReconnect4(void);
uint32_t GetTimeReconnect5(void);
uint16_t GetRtcFail(void);
void SetRtcFail(uint16_t usRtcFail);
uint16_t GetEepromFail(void);
void SetEepromFail(uint16_t usEepromFail);
uint32_t GetTimeWaitSms(void);
void SetTimeWaitSms(uint32_t uiWaitSMS);
void setNameUserSecondSimApn(const char* ptr);
size_t getNameUserSecondSimApn(char* ptr);
void setNameUserSecondSimPass(const char* ptr);
size_t getNameUserSecondSimPass(char* ptr);
void setNameUserSecondSimLogin(const char* ptr);
size_t getNameUserSecondSimLogin(char* ptr);
void setTimeContactSecondSim(uint32_t value);
uint32_t getTimeContactSecondSim(void);
void setOldPositionGps(const GPS_INFO* const ptr);
int getOldPositionGps(GPS_INFO* ptr);
#ifdef __cplusplus
}
#endif

#endif