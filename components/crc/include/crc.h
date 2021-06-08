#ifndef CRC_H__
#define CRC_H__

#include <stdint.h>

class CRC{
	public:
		// Compute a Dallas Semiconductor 8 bit CRC, these are used in the
	    // ROM and scratchpad registers.
	    /**
	     * \brief Cálculo do CRC de 8 bits.
	     *
	     * Faz o calculo do CRC de 8 bits baseado no algorítimo da Dallas Semiconductors.
	     *
	     * @param [in] addr Bytes que será calculado o CRC.
	     * @param [in] len Número de bytes que será computado.
	     * @return O valor do CRC cálculado.
	     */
	    static uint8_t crc8Dallas(const uint8_t *addr, uint8_t len);
	    // Compute the 1-Wire CRC16 and compare it against the received CRC.
	    // Example usage (reading a DS2408):
	    //    // Put everything in a buffer so we can compute the CRC easily.
	    //    uint8_t buf[13];
	    //    buf[0] = 0xF0;    // Read PIO Registers
	    //    buf[1] = 0x88;    // LSB address
	    //    buf[2] = 0x00;    // MSB address
	    //    WriteBytes(net, buf, 3);    // Write 3 cmd bytes
	    //    ReadBytes(net, buf+3, 10);  // Read 6 data bytes, 2 0xFF, 2 CRC16
	    //    if (!CheckCRC16(buf, 11, &buf[11])) {
	    //        // Handle error.
	    //    }
	    //
	    // @param input - Array of bytes to checksum.
	    // @param len - How many bytes to use.
	    // @param inverted_crc - The two CRC16 bytes in the received data.
	    //                       This should just point into the received data,
	    //                       *not* at a 16-bit integer.
	    // @param crc - The crc starting value (optional)
	    // @return True, iff the CRC matches.
	    /**
	     * \brief Faz a checagem do CRC de 16 bits
	     *
	     * @param [in] input Bytes que será calculado o CRC.
	     * @param [in] len Tamanho dos bytes que serão computados
	     * @param [in] inverted_crc Deve apontar para o bytes do CRC nos dados recebidos
	     * @param [in] crc Começo dos dados que será computado.
	     * @return Sucesso ou não no cálculo do CRC
	     * @retval true CRC calculado corresponde.
	     * @retval false CRC calculado não corresponde
	     */
	    static bool check_crc16Dallas(const uint8_t* input, uint16_t len, const uint8_t* inverted_crc, uint16_t crc = 0);

	    // Compute a Dallas Semiconductor 16 bit CRC.  This is required to check
	    // the integrity of data received from many 1-Wire devices.  Note that the
	    // CRC computed here is *not* what you'll get from the 1-Wire network,
	    // for two reasons:
	    //   1) The CRC is transmitted bitwise inverted.
	    //   2) Depending on the endian-ness of your processor, the binary
	    //      representation of the two-byte return value may have a different
	    //      byte order than the two bytes you get from 1-Wire.
	    // @param input - Array of bytes to checksum.
	    // @param len - How many bytes to use.
	    // @param crc - The crc starting value (optional)
	    // @return The CRC16, as defined by Dallas Semiconductor.
	    /**
	     * \brief Cálculo do CRC de 16 bits
	     *
	     * Faz o cálculo do CRC16 definido pela Dallas Semiconductor.
	     *
	     * @param [in] input Bytes que será calculado o CRC
	     * @param [in] len Número de bytes que será realizado o cálculo.
	     * @param [in] crc Valor que deve-se iniciar o cálculo do CRC
	     * @return O valor do CRC cálculado.
	     */
	    static uint16_t crc16Dallas(const uint8_t* input, uint16_t len, uint16_t crc = 0);
};

#endif /* CRC_H__ */
