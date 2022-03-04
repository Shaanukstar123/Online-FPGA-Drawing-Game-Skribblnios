/* A simple program that recognizes the characters 't' and 'v' */
#include <stdio.h>
#include <string.h>

int main () {
	char* msg = "Detected the character 't'.\n";
	FILE* fp;
	char prompt = 0;
	fp = fopen ("/dev/jtag_uart", "r+"); //Open file for reading and writing
	if (fp)	{
		while (prompt != 'v') { // Loop until we receive a 'v'.
			prompt = getc(fp); // Get a character from the JTAG UART.
			if (prompt == 't') { // Print a message if character is 't'.
				fwrite (msg, strlen (msg), 1, fp);
				alt_printf("Got t\n");
			}
			if (ferror(fp)) {// Check if an error occurred with the file
				clearerr(fp);// If so, clear it.
			}
		}
		fprintf(fp, "Closing the JTAG UART file handle.\n");
		fclose (fp);
	}
	return 0;
}
