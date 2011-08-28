#ifndef __PCI_H__
#define __PCI_H__

#include "inc/types.h"

#define PCI_VENDOR_ID		0x00
#define PCI_DEVICE_ID		0x02
#define PCI_COMMAND		0x04
#define PCI_STATUS		0x06
#define PCI_REVISION		0x08
#define PCI_INTERFACE		0x09
#define PCI_SUB_CLASS		0x0A
#define PCI_CLASS_CODE		0x0B
#define PCI_LATENCY		0x0D
#define PCI_HEADER_TYPE		0x0E
#define PCI_BASE_REGISTERS	0x10
#define PCI_INTERRUPT_LINE	0x3C
#define PCI_INTERRUPT_PIN	0x3D
#define PCI_MIN_GRANT		0x3E
#define PCI_MAX_LATENCY		0x3F

#define PCI_COMMAND_IO		0x001
#define PCI_COMMAND_MEMORY	0x002
#define PCI_COMMAND_MASTER	0x004

#define PCI_ADDRESS_SPACE	0x01
#define PCI_ADDRESS_IO_MASK	0xFFFFFFFC
#define PCI_ADDRESS_MEMORY_32_MASK	0xFFFFFFF0

#define PCI_HEADER_BRIDGE	0x01
#define PCI_MULTIFUNCTION	0x80

#define PCI_BUS_PRIMARY	0x18
#define PCI_BUS_SECONDARY	0x19
#define PCI_BUS_SUBORDINATE	0x1A

#define PCI_MASS_STORAGE	0x01
#define PCI_IDE			0x01

#define MAX_PCI_DEVICES		255
#define PCI_NOT_PRESENT		0
#define PCI_PRESENT		1

#define PCI_SERIAL_BUS		0x0C

#define PCI_USB			0x03

typedef struct {
    int		Bus;
    int		Device;
    int		Function;
    unsigned short	VendorID;
    unsigned short	DeviceID;
    unsigned short	Command;
    unsigned short	Status;
    unsigned char	RevisionID;
    unsigned char	Interface;
    unsigned char	ClassCode;
    unsigned char	SubClass;
    unsigned char	CacheLineSize;
    unsigned char	Latency;
    unsigned char	HeaderType;
    unsigned char	SelfTestResult;
    union {
	struct {
	    unsigned long	BaseAddress0;
	    unsigned long	BaseAddress1;
	    unsigned long	BaseAddress2;
	    unsigned long	BaseAddress3;
	    unsigned long	BaseAddress4;
	    unsigned long	BaseAddress5;
	    unsigned long	BaseAddressSize0;
	    unsigned long	BaseAddressSize1;
	    unsigned long	BaseAddressSize2;
	    unsigned long	BaseAddressSize3;
	    unsigned long	BaseAddressSize4;
	    unsigned long	BaseAddressSize5;
	    unsigned long	CardBusCIS;
	    unsigned short	SubSystemVendorID;
	    unsigned short	SubSystemDeviceID;
	    unsigned long	ExpansionROM;
	    unsigned char	CapabilityPointer;
	    unsigned char	Reserved1[ 3 ];
	    unsigned long	Reserved2[ 1 ];
	    unsigned char	InterruptLine;
	    unsigned char	InterruptPIN;
	    unsigned char	MinDMATime;
	    unsigned char	MaxDMALatency;
	} NonBridge;
    } u;
} PCIEntry;

typedef struct {
    int		Bus;
    int		Device;
    int		Function;
    unsigned short	VendorID;
    unsigned short	DeviceID;
    unsigned char	RevisionID;
    unsigned char	Interface;
    unsigned char	SubClass;
    unsigned char	ClassCode;
    union {
	struct {
	    unsigned long	BaseAddress0;
	    unsigned long	BaseAddress1;
	    unsigned long	BaseAddress2;
	    unsigned long	BaseAddress3;
	    unsigned long	BaseAddress4;
	    unsigned long	BaseAddress5;
	    unsigned long	BaseAddressSize0;
	    unsigned long	BaseAddressSize1;
	    unsigned long	BaseAddressSize2;
	    unsigned long	BaseAddressSize3;
	    unsigned long	BaseAddressSize4;
	    unsigned long	BaseAddressSize5;
	    unsigned char	InterruptLine;
	    unsigned char	InterruptPIN;
	    unsigned char	MinDMATime;
	    unsigned char	MaxDMALatency;
	} NonBridge;
    } u;
} PCIInfo;

unsigned long ReadPCIConfig( int bus, int device, int function, int offset, int size );
unsigned long WritePCIConfig( int bus, int device, int function, int offset, int size, unsigned long value );
int InitPCI(void );
int lspci( void );


#endif
