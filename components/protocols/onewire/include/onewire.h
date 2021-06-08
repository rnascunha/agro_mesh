#ifndef ONEWIRE_H__
#define ONEWIRE_H__

/**
 * \file
 * \brief Biblioteca para utilização do protocolo onewire
 *
 * O protocolo OneWire é um protocolo onde pode-se ler/controlar inúmeros
 * dispositivos em um mesmo barramento. Utiliza apenas uma porta digital (GPIO)
 * para sinalização, podendo enviar comandos para todos os dispositivo no barramento,
 * ou para um específico, através do endereço do dispositivo.
 *
 * Criado pela <i>Dallas Semiconductor</i> (adquirida pela \a Maxim) para os sensores
 * de temperatura DS1820, DS18S20 e DS18B20.
 *
 * Referência: <a href="https://www.maximintegrated.com/en/products/digital/one-wire.html">ONEWIRE</a>
 *
 * \author Rafael Cunha <rnascunha@gmail.com>
 * \date 06/2016
 */

/**
 * \defgroup onewire ONEWIRE
 *
 * \brief Definição da classe, tipos e macros referente ao protocolo ONEWIRE.
 * @{
 */

/*
 * TODO Criar/Utilziar classe genérica CRC
 */

#define FALSE 0
#define TRUE  1

// Platform specific I/O definitions

#include <stdint.h>
#include "gpio.h"
#include "crc.h"

// ROM commands - Comandos para endereçar dispositivos
#define SEARCH_ROM			0xF0	//!< Comando para determinar todos os dispositivos e tipos de dispositivos conectados
#define READ_ROM			0x33	//!< Comando para ler a ROM code quando há apenas 1 dispositivo
#define MATCH_ROM			0x55	//!< Comando para selecionar apenas 1 dispositivo que irá receber um comando de função
#define SKIP_ROM			0xCC	//!< Comando para selecionar todos os dispositivos que irão receber um comando de função
#define ALARM_SEARCH		0xEC	//!< Comando para que todos os dispositivos verifiquem se houve uma condição de alarme na última conversão

/**
 * \brief Classe que define os membros e métodos para utilização do protocolo ONEWIRE
 *
 * Nesta classe é definido os comandos de leitura e escrita pelo protocolo, utilizando
 * uma porta digital. Também são definidos as operações de ROM do protocolo.
 */
class OneWire{
  public:
    /**
     * \brief Define a porta digital utilizada.
     *
     * @param [in] gpio Porta GPIO utilizada como barramento.
     */
    OneWire(GPIO_Basic *gpio);

    // Perform a 1-Wire reset cycle. Returns 1 if a device responds
    // with a presence pulse.  Returns 0 if there is no device or the
    // bus is shorted or otherwise held low for more than 250uS
    /**
     * \brief Realiza o ciclo de \a reset definido onewire
     *
     * O ciclo de \a reset é o primeiro passo antes de interagir com um dispositivo.
     * O dispositivo mestre sinaliza que será enviado comandos para os dispositivos
     * escravos no barramento
     *
     * @return Retorna o sucesso no ciclo
     * @retval 0 Não foi recebido um sinal de resposta \a pulse
     * @retval 1 Sinal de presença \a pulse recebido com sucesso
     */
    uint8_t reset(void);

    // Issue a 1-Wire rom select command, you do the reset first.
    /**
     * \brief Executa o comando para selecionar um dos dispositivos no barramento
     *#if ONEWIRE_SEARCH
     *
     * Executa o comando de rom MATCH_ROM. Após o envio do comando, é enviado 8 bytes
     * com o enderço do dispositivo desejado.
     *
     * @param [in] rom Endereço do dispositivo que deseja selecionar
     */
    void select(const uint8_t rom[8]);

    // Issue a 1-Wire rom skip command, to address all on bus.
    /**
     * \brief Endereça um comando a todos no barramento.
     *
     * Executa o comando SKIP_ROM, definindo o próximo comando de função para todos
     * os dipositivos que estão no barramento.
     */
    void skip(void);

    // Write a byte. If 'power' is one then the wire is held high at
    // the end for parasitically powered devices. You are responsible
    // for eventually depowering it by calling depower() or doing
    // another read or write.
    /**
     * \brief Escreve um byte no barramento.
     *
     * @param [in] v Byte que será escrito
     * @param [in] power Utilizar 1 para quando o dispositivo está em modo parasita. Valor padrão: \b 0.
     *
     * \note Modo parasita é quando a alimentação do dispositivo é feito pelo barramento de dados.
     */
    void write(uint8_t v, uint8_t power = 0);

    /**
     * \brief Escreve vários bytes no barramento.
     *
     * @param [in] buf Bytes que será enviados
     * @param [in] count Número de bytes que será enviados
     * @param [in] power Utilizar 1 para quando o dispositivo está em modo parasita. Valor padrão: \b 0.
     *
     * \note Modo parasita é quando a alimentação do dispositivo é feito pelo barramento de dados.
     */
    void write_bytes(const uint8_t *buf, uint16_t count, bool power = 0);

    // Read a byte.
    /**
     * \brief Lê um byte do barramento.
     *
     * @return Byte lido.
     */
    uint8_t read(void);

    /**
     * \brief Lê vários bytes do barramento.
     *
     * @param [out] buf Bytes lidos.
     * @param [in] count número de bytes que deve ser lidos
     */
    void read_bytes(uint8_t *buf, uint16_t count);

    // Write a bit. The bus is always left powered at the end, see
    // note in write() about that.
    /**
     * \brief Escreve um bit no modo definido pelo protocolo onewire
     *
     * @param [in] v Bit que será escrito.
     */
    void write_bit(uint8_t v);

    // Read a bit.
    /**
     * \brief Lê um bit no modo definido pelo protocolo onewire.
     *
     * @return Bit lido.
     */
    uint8_t read_bit(void);

    // Stop forcing power onto the bus. You only need to do this if
    // you used the 'power' flag to write() or used a write_bit() call
    // and aren't about to do another read or write. You would rather
    // not leave this powered if you don't have to, just in case
    // someone shorts your bus.
    /**
     * \brief Para o fornecimento de corrente no barramento, para dispositivos em modo parasita.
     *
     * \note Modo parasita é quando a alimentação do dispositivo é feito pelo barramento de dados.
     */
    void depower(void);

    // Clear the search state so that if will start from the beginning again.
    /**
     * \brief Limpa os valores de estado de busca de dispositivos.
     */
    void reset_search(void);

    // Setup the search to find the device type 'family_code' on the next call
    // to search(*newAddr) if it is present.
    /**
     * \brief Define uma família de dispositivos para realizar a procura no barramento.
     *
     * @param [in] family_code Família de dispositivos que deseja fazer a procura.
     */
    void target_search(uint8_t family_code);

    // Look for the next device. Returns 1 if a new address has been
    // returned. A zero might mean that the bus is shorted, there are
    // no devices, or you have already retrieved all of them.  It
    // might be a good idea to check the CRC to make sure you didn't
    // get garbage.  The order is deterministic. You will always get
    // the same devices in the same order.
    /**
     * \brief Procura o próximo dispositivo presente no barramento.
     *
     * Os dispositivos são encontrados de forma determinística (os dispositivos são
     * sempre encontrados na mesma ordem.
     *
     * @param [out] newAddr Endereço do dispositivo encontrado.
     * @return Sucesso ou não na procura do dispositivo.
     * @retval 1 Um novo dispositivo foi encontrado.
     * @retval 0 Foi encontrado todos os dispositivos, não há dispositivos ou o barramento está em curto.
     */
    uint8_t search(uint8_t *newAddr);

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
    static uint8_t crc8(const uint8_t *addr, uint8_t len){
    	return CRC::crc8Dallas(addr,len);
    }

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
    static bool check_crc16(const uint8_t* input, uint16_t len, const uint8_t* inverted_crc, uint16_t crc = 0){
    	return CRC::check_crc16Dallas(input,len,inverted_crc,crc);
    }

  private:
	/**
	 * \brief Define a porta digital que será utilizada para barramento dos dispositivos
	 */
    GPIO_Basic *gpio;

    // global search state
    unsigned char ROM_NO[8];
    uint8_t LastDiscrepancy;
    uint8_t LastFamilyDiscrepancy;
    uint8_t LastDeviceFlag;
};

/**@}*/

#endif /* ONEWIRE_H__ */
