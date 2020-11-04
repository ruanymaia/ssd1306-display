#include "TWI.h"

// Functions
// Setup TWI hardware.
void TWI_Setup(void)
{
	// Initialize TWI prescaler and bit rate
	TWSR = 0x00;	// Prescaler = 1
	TWBR = (uint8_t)(((F_CPU / F_SCL) - 16) / 2);

	// Enable TWI module
	TWCR = 1<<TWEN;
}

// Get TWI status. 
uint8_t TWI_Status(void)
{
	return (TWSR & 0xF8);
}

// Begin TWI transmission. 
uint8_t TWI_BeginTransmission(void)
{
	// Transmit START condition
	TWCR = (1<<TWINT) | (1<<TWSTA) | (1<<TWEN);

	// Wait for TWINT Flag set. This indicates that
	// the START condition has been transmitted.
	while (!(TWCR & (1<<TWINT)));

	// Return status register
	return TWI_Status();
}

// End TWI transmission. 
void TWI_EndTransmission(void)
{
	// Transmit STOP condition
	TWCR = (1<<TWINT) | (1<<TWEN) | (1<<TWSTO);

	// Wait for stop condition to be executed on bus
	// TWINT is not set after a stop condition!
	while (TWCR & (1<<TWSTO));
}

// Transmit data. 
uint8_t TWI_Transmit(const uint8_t data)
{
	TWDR = data; // Load DATA into TWDR Register
	TWCR = (1<<TWINT) | (1<<TWEN); // Clear TWINT bit in TWCR to start transmission of data
	
	// Wait for TWINT Flag Set. This indicates that the DATA has been transmitted 
	// and ACK/NACK has been received
	while (!(TWCR & (1<<TWINT)));  

	// Return TWI Status Register
	return TWI_Status();
}

// Wait until ACK received. 
uint8_t TWI_ReceiveACK(void)
{
	TWCR = (1<<TWINT) | (1<<TWEA) | (1<<TWEN);
	
	// Wait till reception is
	while (!(TWCR & (1<<TWINT)));

	// Return received data
	return TWDR;
}

// Wait until NACK received. 
uint8_t TWI_ReceiveNACK(void)
{
	TWCR = (1<<TWINT) | (1<<TWEN);

	// Wait till reception is
	while (!(TWCR & (1<<TWINT)));

	// Return received data
	return TWDR;
}

// Transmit packet to specific slave address. 
uint8_t TWI_PacketTransmit(const uint8_t SLA, const uint8_t SubAddress, uint8_t *Packet, const uint8_t Length)
{
	uint8_t i, status;
	
	// Transmit START signal
	status = TWI_BeginTransmission();
	if ((status != 0x08) && ((status != 0x10)))
	{
		return 1;
	}

	// Transmit SLA+W
	status = TWI_Transmit(SLA<<1);
	if ((status != 0x18) && (status != 0x20))
	{
		return 1;
	}

	// Transmit write address
	status = TWI_Transmit(SubAddress);
	if ((status != 0x28) && (status != 0x30))
	{
		return 1;
	}

	// Transmit DATA
	for (i = 0 ; i < Length ; i++)
	{
		status = TWI_Transmit(Packet[i]);
		if ((status != 0x28) && (status != 0x30))
		{
			return 1;
		}
	}
	
	// Transmit STOP signal
	TWI_EndTransmission();

	return 0;
}

// Receive packet from specific slave address. 
uint8_t TWI_PacketReceive(const uint8_t SLA, const uint8_t SubAddress, uint8_t *Packet, const uint8_t Length)
{
	uint8_t i, status;

	// Transmit START signal
	status = TWI_BeginTransmission();
	if ((status != 0x08) && (status != 0x10))
	{
		return 1;
	}
	// Transmit SLA+W
	status = TWI_Transmit(SLA<<1);
	if ((status != 0x18) && (status != 0x20))
	{
		return 1;
	}
	// Transmit read address
	status = TWI_Transmit(SubAddress);
	if ((status != 0x28) && (status != 0x30))
	{
		return 1;
	}
	// Transmit START signal
	status = TWI_BeginTransmission();
	if ((status != 0x08) && (status != 0x10))
	{
		return 1;
	}
	// Transmit SLA+R
	status = TWI_Transmit((SLA<<1) | (1<<0)); 
	if ((status != 0x40) && (status != 0x48))
	{
		return 1;
	}
	// Receive DATA
	// Read all the bytes, except the last one, sending ACK signal
	for (i = 0 ; i < (Length - 1) ; i++)
	{
		Packet[i] = TWI_ReceiveACK();
		status = TWI_Status();
		if ((status != 0x50) && (status != 0x58))
		{
			return 1;
		}
	}
	// Receive last byte and send NACK signal
	Packet[i] = TWI_ReceiveNACK();
	status = TWI_Status();
	if ((status != 0x50) && (status != 0x58))
	{
		return 1;
	}
	// Transmit STOP signal
	TWI_EndTransmission();
	
	return 0;
}

// Set self slave address.
void TWI_SetAddress(const uint8_t address)
{
	// Set TWI slave address (upper 7 bits)
	TWAR = address<<1;
} 