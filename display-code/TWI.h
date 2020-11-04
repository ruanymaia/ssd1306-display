#ifndef TWI_H_
#define TWI_H_

// Headers
#include <inttypes.h>
#include <avr/io.h>

// Clock Definitions
#define F_CPU 8000000UL	// AVR Frequency
#define F_SCL 400000UL	// SCL Frequency

// Prototypes
void TWI_Setup(void);
uint8_t TWI_BeginTransmission(void);
void TWI_EndTransmission(void);
uint8_t TWI_Status(void);

uint8_t TWI_Transmit(uint8_t data);
uint8_t TWI_ReceiveACK(void);
uint8_t TWI_ReceiveNACK(void);

uint8_t TWI_PacketTransmit(const uint8_t SLA, const uint8_t SubAddress, uint8_t *Packet, const uint8_t Length);
uint8_t TWI_PacketReceive(const uint8_t SLA, const uint8_t SubAddress, uint8_t *Packet, const uint8_t Length);

void TWI_SetAddress(const uint8_t address);

#endif