#include <stddef.h>
#include <stdint.h>

// Function to write a byte to the specified I/O port
static void outb(uint16_t port, uint8_t value)
{	
	// Assembly instruction to output a byte to the specified port
	asm("out %0,%1" : /* empty */ : "a"(value), "Nd"(port) : "memory");
}

// Function to write a 32-bit value to the specified I/O port
static inline void outb_32(uint16_t port, uint32_t value)
{	
	// Assembly instruction to output a 32-bit value to the specified port
	asm("outl %0,%1" : /* empty */ : "a"(value), "Nd"(port) : "memory");
}

// Function to read a byte from the specified I/O port
static inline uint32_t inb(uint16_t port)
{
	uint32_t ret;
	// Assembly instruction to input a byte from the specified port
	asm("in %1, %0" : "=a"(ret) : "Nd"(port) : "memory");
	return ret;
}

// Function to print an 8-bit value via hypercall
void HC_print8bit(uint8_t val)
{
	outb(0xE9, val);	// Output the 8-bit value to I/O port 0xE9
}

// Function to print a 32-bit value via hypercall
void HC_print32bit(uint32_t val)
{
	// Fill in here
	outb_32(0xEA, val);		// Output the 32-bit value to I/O port 0xEA
}

// Function to get the number of exits via hypercall
uint32_t HC_numExits()
{
	// Fill in here
	return inb(0xEB);	// Read the number of exits from I/O port 0xEB
}

// Function to print a string via hypercall
void HC_printStr(char *str)
{
	// Fill in here
	uintptr_t str_addr = (uintptr_t)str;	// Get the address of the string
	outb_32(0xEC,str_addr);
	//asm("out %0,%1" : /* empty */ : "a"((uint32_t)str_addr), "Nd"(0xEC) : "memory");	// Output the string address to I/O port 0xEC
}

// Function to get the number of exits by type via hypercall
char *HC_numExitsByType()
{
	uint32_t ret;	
	asm("in %1, %0" : "=a"(ret) : "Nd"(0xED) : "memory");	// Read the number of exits by type from I/O port 0xED
	char *ptr = (char *)(uintptr_t)ret;	// Cast the return value to a character pointer

	return ptr;
}

// Function to translate guest virtual address to host virtual address via hypercall
uint32_t HC_gvaToHva(uint32_t gva)
{
	outb_32(0xEE, (uint32_t)gva); // Output the guest virtual address to I/O port 0xEE

	uint32_t hva = inb(0xEF); // Read the host virtual address from I/O port 0xEF
	
	return hva;
}

void
	__attribute__((noreturn))
	__attribute__((section(".start")))
	_start(void)
{
	const char *p;

	for (p = "Hello 695!\n"; *p; ++p)
		HC_print8bit(*p);

	/*----------Don't modify this section. We will use grading script---------*/
	/*---Your submission will fail the testcases if you modify this section---*/
	HC_print32bit(2048);
	HC_print32bit(4294967295);

	uint32_t num_exits_a, num_exits_b;
	num_exits_a = HC_numExits();

	char *str = "CS695 Assignment 2\n";
	HC_printStr(str);

	num_exits_b = HC_numExits();

	HC_print32bit(num_exits_a);
	HC_print32bit(num_exits_b);

	char *firststr = HC_numExitsByType();
	uint32_t hva;
	hva = HC_gvaToHva(1024);
	HC_print32bit(hva);
	hva = HC_gvaToHva(4294967295);
	HC_print32bit(hva);
	char *secondstr = HC_numExitsByType();

	HC_printStr(firststr);
	HC_printStr(secondstr);
	/*------------------------------------------------------------------------*/

	*(long *)0x400 = 42;

	for (;;)
		asm("hlt" : /* empty */ : "a"(42) : "memory");
}