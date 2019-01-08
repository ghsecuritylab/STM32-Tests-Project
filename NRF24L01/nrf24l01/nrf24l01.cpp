/**
 ******************************************************************************
 * @file    nrf24l01.cpp
 * @author  Ivan Orfanidi
 * @version V1.0.0
 * @date    04/01/2018
 * @brief
 ******************************************************************************
 * @attention
 *
 *
 * <h2><center>&copy; COPYRIGHT 2018 </center></h2>
 ******************************************************************************
 */


/* Includes ------------------------------------------------------------------*/
#include "nrf24l01.hpp"
#include "stm32f10x_rcc.h"
#include "board.hpp"

     
/// Addresses of the RX_PW_P# registers
const uint8_t nRF24_RX_PW_PIPE[] = 
{
    Nrf::nRF24_REG_RX_PW_P0,
    Nrf::nRF24_REG_RX_PW_P1,
    Nrf::nRF24_REG_RX_PW_P2,
    Nrf::nRF24_REG_RX_PW_P3,
    Nrf::nRF24_REG_RX_PW_P4,
    Nrf::nRF24_REG_RX_PW_P5
};

/// Addresses of the address registers
const uint8_t nRF24_ADDR_REGS[] = 
{
    Nrf::nRF24_REG_RX_ADDR_P0,
    Nrf::nRF24_REG_RX_ADDR_P1,
    Nrf::nRF24_REG_RX_ADDR_P2,
    Nrf::nRF24_REG_RX_ADDR_P3,
    Nrf::nRF24_REG_RX_ADDR_P4,
    Nrf::nRF24_REG_RX_ADDR_P5,
    Nrf::nRF24_REG_TX_ADDR
};


/**
 * @brief Static instances of a class
 */
Nrf* Nrf::Nrf24l01[NRF24_MAX_COUNT];


/**
 * @brief —onstructor
 */
Nrf::Nrf(VirtualPort* port, GPIO_TypeDef* ce_port, uint16_t ce_pin, GPIO_TypeDef* cns_port, uint16_t cns_pin)
{
    InterfaceSettings_t newSettings;
    newSettings.CE_Port = ce_port;
    newSettings.CNS_Port = cns_port;
    newSettings.CE_Pin.GPIO_Pin = ce_pin;
    newSettings.CNS_Pin.GPIO_Pin = cns_pin;
    VPort = port;
    
    // Def settings Pin
    newSettings.CE_Pin.GPIO_Mode = GPIO_Mode_Out_PP;
    newSettings.CE_Pin.GPIO_Speed = GPIO_Speed_10MHz;
    newSettings.CNS_Pin.GPIO_Mode = GPIO_Mode_Out_PP;
    newSettings.CNS_Pin.GPIO_Speed = GPIO_Speed_10MHz;
    
    InterfaceSettings = nullptr;
    size_t freeClass = NRF24_MAX_COUNT;
    for(size_t i = 0; i < NRF24_MAX_COUNT; i++) {
        if(Nrf24l01[i]) {
            InterfaceSettings_t nrfSettings;
            Nrf24l01[i]->GetInterfaceSettings(&nrfSettings);
            if(((memcmp(&nrfSettings.CE_Pin, &newSettings.CE_Pin, sizeof(GPIO_InitTypeDef)) == 0) &&
               (nrfSettings.CE_Port == newSettings.CE_Port)) ||
               ((memcmp(&nrfSettings.CNS_Pin, &newSettings.CNS_Pin, sizeof(GPIO_InitTypeDef)) == 0) &&
               (nrfSettings.CNS_Port == newSettings.CNS_Port))) {
                    /// Error, class was create
                    return;
            }
        }
        else {
            freeClass = i;
        }
    }
    
    if(NRF24_MAX_COUNT == freeClass) {
        return;
    }
    
    SetInterfaceSettings(newSettings);
    
    // Config GPIO(CE, CNS)
    InitGpio(newSettings);
        
    Nrf24l01[freeClass] = this;
}


/**
 * @brief Destructor
 */
Nrf::~Nrf()
{
    if(InterfaceSettings) {
        DeInitGpio(InterfaceSettings);
        DeInitSpi(InterfaceSettings);
        for(size_t i = 0; i < NRF24_MAX_COUNT; i++) {
            if(Nrf24l01[i]) {
                InterfaceSettings_t nrfSettings;
                Nrf24l01[i]->GetInterfaceSettings(&nrfSettings);
                if(((memcmp(&nrfSettings.CE_Pin, &InterfaceSettings->CE_Pin, sizeof(GPIO_InitTypeDef)) == 0) &&
                   (nrfSettings.CE_Port == InterfaceSettings->CE_Port)) ||
                   ((memcmp(&nrfSettings.CNS_Pin, &InterfaceSettings->CNS_Pin, sizeof(GPIO_InitTypeDef)) == 0) &&
                   (nrfSettings.CNS_Port == InterfaceSettings->CNS_Port))) {
                        delete [] InterfaceSettings;
                        Nrf24l01[i]  = nullptr;
                        break;
                }
            }
        }
    }
}


void Nrf::Init()
{
    uint8_t state = 0xFF;
    state = ReadReg(nRF24_REG_CONFIG);
    state = 0x08;
    WriteReg(nRF24_REG_CONFIG, state);
    state = ReadReg(nRF24_REG_CONFIG);
    
    ClearIRQFlags();
}


/**
 * @brief Clear any pending IRQ flags
 */
void Nrf::ClearIRQFlags()
{
    
}


/**
 * @brief Get alls settings
 * @param [out] settings - alls interface settings, GPIO
 */
void Nrf::GetInterfaceSettings(InterfaceSettings_t* const settings) const
{
    memcpy(settings, InterfaceSettings, sizeof(InterfaceSettings_t));
}


/**
 * @brief Set alls settings
 * @param [in] settings - alls interface settings, GPIO
 */
void Nrf::SetInterfaceSettings(InterfaceSettings_t& settings)
{
    /* Copy Settings */
    InterfaceSettings = new InterfaceSettings_t;
    memcpy(&InterfaceSettings->CE_Pin, &settings.CE_Pin, sizeof(GPIO_InitTypeDef));
    memcpy(&InterfaceSettings->CNS_Pin, &settings.CNS_Pin, sizeof(GPIO_InitTypeDef));
    InterfaceSettings->CE_Port = settings.CE_Port;
    InterfaceSettings->CNS_Port = settings.CNS_Port;
}


/**
 * @brief Deinitialisation GPIO
 * @param [in] settings - settings GPIO
 */
void Nrf::DeInitGpio(InterfaceSettings_t* settings) const
{
    GPIO_InitTypeDef pin;
    pin.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    pin.GPIO_Speed = GPIO_Speed_2MHz;
    
    pin.GPIO_Pin = settings->CE_Pin.GPIO_Pin;
    GPIO_Init(InterfaceSettings->CE_Port, &pin);
    
    pin.GPIO_Pin = settings->CNS_Pin.GPIO_Pin;
    GPIO_Init(InterfaceSettings->CE_Port, &pin);
}


/**
 * @brief Initialisation GPIO
 * @param [in] settings - settings GPIO
 */
void Nrf::InitGpio(InterfaceSettings_t& settings) const
{
    // Enable Clock Port Output
    Board::GpioClock(InterfaceSettings->CE_Port, ENABLE);
    Board::GpioClock(InterfaceSettings->CNS_Port, ENABLE);
    
    /* Config GPIO CE & CNS for nRF24 */
    GPIO_Init(InterfaceSettings->CE_Port, &InterfaceSettings->CE_Pin);
    GPIO_Init(InterfaceSettings->CNS_Port, &InterfaceSettings->CNS_Pin);
    
    RxOff();
}


/**
 * @brief Returns the status of class creation.
 * @retval true -  Success;
 *         false - Fail.
 */
bool Nrf::CreateClass() const
{
    return (InterfaceSettings != nullptr);
}


void Nrf::RxOn() const
{
    GPIO_SetBits(InterfaceSettings->CE_Port, InterfaceSettings->CE_Pin.GPIO_Pin);
}


void Nrf::RxOff() const
{
    GPIO_ResetBits(InterfaceSettings->CE_Port, InterfaceSettings->CE_Pin.GPIO_Pin);
}


/**
 * @brief Check if the nRF24L01 present
 * @retval true -  Success;
 *         false - Fail.
 */
bool Nrf::Check() const
{
    /// Fake address to test transceiver presence (5 bytes long)
    enum { SIZE_TEST_BUF = 5 };
    const uint8_t request[SIZE_TEST_BUF] = { 'n', 'R', 'F', '2', '4'};
    WriteBuffer(nRF24_CMD_W_REGISTER | nRF24_REG_TX_ADDR, request, SIZE_TEST_BUF);
    
    uint8_t answer[SIZE_TEST_BUF];
    ReadBuffer(nRF24_CMD_R_REGISTER | nRF24_REG_TX_ADDR, answer, SIZE_TEST_BUF);
    
    if(memcmp(request, answer, SIZE_TEST_BUF)) {
        return false;
    }
    
    return true;
}


void Nrf::CsnLow() const
{
    GPIO_ResetBits(InterfaceSettings->CNS_Port, InterfaceSettings->CNS_Pin.GPIO_Pin);
}


void Nrf::CsnHigh() const
{
    GPIO_SetBits(InterfaceSettings->CNS_Port, InterfaceSettings->CNS_Pin.GPIO_Pin);
}

 
/**
 * @brief Write a multi-byte register
 * @param [in] reg - number of register to write
 * @param [in] buf - pointer to the buffer with data to write
 * @param [in] count - number of bytes to write
 */
void Nrf::WriteBuffer(uint8_t reg, const uint8_t* buf, uint8_t count) const
{
    CsnLow();
    
    SpiSendReceiveData(reg);
    
	while(count--) {
        SpiSendReceiveData(*buf++);
	}
    
    CsnHigh();
}


/**
 * @brief Read a multi-byte register
 * @param [in] reg - number of register to write
 * @param [out] buf - pointer to the buffer with data to write
 * @param [in] count - number of bytes to write
 */
void Nrf::ReadBuffer(uint8_t reg, uint8_t* buf, uint8_t count) const
{
    CsnLow();
    
    SpiSendReceiveData(reg);
    
	while(count--) {
        *buf++ = SpiSendReceiveData(Nrf24Registers_t::nRF24_CMD_NOP);
	}
    
    CsnHigh();
}


uint8_t Nrf::ReadReg(uint8_t address)
{
    CsnLow();

    uint8_t byte = address & nRF24_MASK_REG_MAP;
    VPort->Transmit((uint8_t*)&byte, sizeof(uint8_t));
    
    byte = nRF24_CMD_NOP;
    while(VPort->GetLen() == 0);
    VPort->Receive((uint8_t*)&byte, sizeof(uint8_t));
    
    VPort->Transmit((uint8_t*)&byte, sizeof(uint8_t));
    
    while(VPort->GetLen() == 0);
    VPort->Receive((uint8_t*)&byte, sizeof(uint8_t));

    CsnHigh();
    
    return byte;
}


void Nrf::WriteReg(uint8_t address, uint8_t data)
{
    CsnLow();
    
    if(address < nRF24_CMD_W_REGISTER) {
        // This is a register access
        SpiSendReceiveData(nRF24_CMD_W_REGISTER | (address & nRF24_MASK_REG_MAP));
    
        SpiSendReceiveData(data);
    }
    else {
        // This is a single byte command or future command/register
        SpiSendReceiveData(address);
        
        if ((address != nRF24_CMD_FLUSH_TX) && (address != nRF24_CMD_FLUSH_RX) && 
			(address != nRF24_CMD_REUSE_TX_PL) && (address != nRF24_CMD_NOP)) {
                SpiSendReceiveData(data);
            }
    }

    CsnHigh();
}


void Nrf::SetPowerMode(PowerControl_t mode)
{
    uint8_t state = ReadReg(nRF24_REG_CONFIG);
    if(mode == nRF24_PWR_UP) {
        // Set the PWR_UP bit of CONFIG register to wake the transceiver
        // It goes into Stanby-I mode with consumption about 26uA
        state |= nRF24_CONFIG_PWR_UP;
    }
    else {
        // Clear the PWR_UP bit of CONFIG register to put the transceiver
        // into power down mode with consumption about 900nA
        state &= ~nRF24_CONFIG_PWR_UP;
    }
    WriteReg(nRF24_REG_CONFIG, state);
}


/**
 * @brief SPI transmit/receive function (hardware depended)
 * @param [in]  data - value to transmit via SPI
 * @retval value received from SPI
 */
uint8_t Nrf::SpiSendReceiveData(uint8_t byte) const
{
    VPort->Transmit((uint8_t*)&byte, sizeof(uint8_t));
    
    byte = nRF24_CMD_NOP;
    while(VPort->GetLen() == 0);
    
    VPort->Receive((uint8_t*)&byte, sizeof(uint8_t));
    return byte;
}