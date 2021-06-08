#ifndef __DALLASTEMPERATURE_H__
#define __DALLASTEMPERATURE_H__

/**
 * \file
 * \brief Biblioteca para utilização dos sensores de temperatura da Dallas Semicondutors
 *
 * Biblioteca para utilização dos sensores DS1820, DS18S20 e DS18B20. Utilizam protocolo
 * ONEWIRE, necessitando de apenas uma porta digital para comunicação com vários dispositivos.
 *
 * Os diferentes sensores tem diferentes resoluções, mas funcionam de maneira semelhante. O
 * protocolo onewire permite que seja identificado o tipo de sensor com que se está comunicando.
 *
 * A maioria dos métodos consiste em achar/identificar os dispositivos por um endereçamento
 * único de 8 bytes, e fazer a leitura dos dados do ScratchPad (memória com as configurações
 * e status do dispositivo) e manipular estes dados.
 *
 * <b>Modo de usar:</b>
 * * Declarar o objeto DallasTemperature passando como argumento o ponteiro para o objeto do tipo ONEWIRE;
 * * Inciar o objeto com o método \b begin;
 * * Chamar o método \b requestTemperature para conversão da temperature;
 * * Chamar o método \b getTempC para receber o valor da temperatura convertida.
 *
 * Manual: <a href="https://datasheets.maximintegrated.com/en/ds/DS18B20.pdf">DS18B20</a>
 *
 * \author Rafael Cunha <rnascunha@gmail.com>
 * \date 06/2016
 */

/**
 * \defgroup dallastemperature Dallas Temperature
 *
 * \brief Definição da classe, tipos e macros referente aos sensores de tempetarura
 * da Dallas Semiconductors.
 * @{
 */

/*
 * TODO Revisar/refazer configurações e utilização de alarmes
 * TODO Criar classe CRC generica
 * TODO Fazer traramento para utilização ou não do PROGMEM
 */

#include "onewire.h"

// Model IDs
#define DS18S20MODEL 0x10	///< ID do sensor DS18S20
#define DS18B20MODEL 0x28	///< ID do sensor DS18B20
#define DS1822MODEL  0x22	///< ID do sensor DS1820

// OneWire commands
#define STARTCONVO      0x44  ///< Comando para iniciar uma conversão e colocar no scratchpad
#define COPYSCRATCH     0x48  ///< Copiar Scratchpad para EEPROM
#define READSCRATCH     0xBE  ///< Ler Scratchpad
#define WRITESCRATCH    0x4E  ///< Escrever no Scratchpad
#define RECALLSCRATCH   0xB8  ///< Reler valores da EEPROM para o Scratchpad
#define READPOWERSUPPLY 0xB4  ///< Verifica se o dispositivo está em modo parasita
#define ALARMSEARCH     0xEC  ///< Questiona no barramento se algum dispositivo entrou na situaçã de alarme

// Scratchpad locations
#define TEMP_LSB        0		///< Byte menos significativo da temperatura convertida
#define TEMP_MSB        1		///< Byte mais significativo da temperatura convertida
#define HIGH_ALARM_TEMP 2		///< Alarme para temperaturas acima
#define LOW_ALARM_TEMP  3		///< Alarme para temperaturas abaixo
#define CONFIGURATION   4		///< Byte de configuração
#define INTERNAL_BYTE   5		///< Uso interno
#define COUNT_REMAIN    6		///< Uso interno
#define COUNT_PER_C     7		///< Uso interno
#define SCRATCHPAD_CRC  8		///< CRC calculado do scratchpad (para verificação de integridade dos dados)

// Device resolution
#define TEMP_9_BIT  0x1F 		///< Resolução de 9 bit
#define TEMP_10_BIT 0x3F 		///< Resolução de 10 bit
#define TEMP_11_BIT 0x5F 		///< Resolução de 11 bit
#define TEMP_12_BIT 0x7F 		///< Resolução de 12 bit

// Error Codes
#define DEVICE_DISCONNECTED -127	///< Valor para quando à erro na conversão

/**
 * \brief Tipo que define o endereço de um dispotivo
 */
typedef uint8_t DeviceAddress[8];

/**luna/index.jsp?topic=%2Forg.eclipse.cdt.doc.user%2Freference%2Fcdt_u_prop_general_pns_src.htm
 * \brief Classe que lida com os sensores da Dallas Semiconductors
 */
class Dallas_Temperature{
 	 public:
		/**
		 * \brief Construtor que define a porta digital que irá utilizar o protocolo onewire
		 *
		 * @param [in] _wire Porta digital configurada com o protocolo onewire
		 */
		Dallas_Temperature(OneWire* wire);

		// initalise bus
		/**
		 * \brief Incializa os membros da classe.
		 *
		 * Faz a leitura dos dispositivos (validando seus endereços), número de bits de resolução e
		 * modo de alimentação (parasita ou não).
		 */
		void begin(void);

		// returns the number of devices found on the bus
		/**
		 * \brief Retorna o número de dispositivos no barramento.
		 *
		 * @return Número de dispositivos no barramento.
		 */
		uint8_t getDeviceCount(void);

		// Is a conversion complete on the wire?
		bool isConversionComplete(void);

		// returns true if address is valid
		/**
		 * \brief Verifica se o dispositivo tem um endereço válido.
		 *
		 * Faz o cálculo do CRC8 para a validação do dispositivo.
		 *
		 * @param [in] deviceAddress Endereço do dispositivo que será validado.
		 * @return Sucesso ou não na validação do dispositivo.
		 * @retval true Endereço válido.
		 * @retval false Endereço inválido.
		 */
		bool validAddress(uint8_t* deviceAddress);

		// finds an address at a given index on the bus
		/**
		 * \brief Acha o endereço do dispositivo numa determinada posição no barramento.
		 *
		 * @param [out] deviceAddress Endereço do dispositivo encontrado.
		 * @param [in] index Posição do dispositivo que se quer o endereço.
		 * @return Sucesso ou não da operação.
		 * @retval true Achou o endereço do dispositivo.
		 * @retval false Não achou o dispositivo.
		 */
		bool getAddress(uint8_t* deviceAddress, const uint8_t index);

		// attempt to determine if the device at the given address is connected to the bus
		/**
		 * \brief Verifica se um dispositivo de um determinado endereço está conectado.
		 *
		 * @param [in] deviceAddress Endereço do dispositivo que deseja verificar
		 * @return Sucesso ou não na operação
		 * @retval true Dispositivo está conectado.
		 * @retval false Dispositivo não está conectado.
		 */
		bool isConnected(uint8_t* deviceAddress);

		// attempt to determine if the device at the given address is connected to the bus
		// also allows for updating the read scratchpad
		/**
		 * \brief Verifica se um dispositivo está conectado e lê o scratchpad.
		 *
		 * @param [in] deviceAddress Endereço do dispositivo que deseja verificar
		 * @param [out] scratchpad ScratchPad lido.
		 * @return Sucesso ou não da operação.
		 * @retval true Dispositivo está conectado.
		 * @retval false Dispositivo não está conectado.
		 */
		bool isConnected(uint8_t* deviceAddress, uint8_t* scratchpad);

		// read device's scratchpad
		/**
		 * \brief Lê o scratchPad de um dispositivo, dado seu endereço.
		 *
		 * @param [in] deviceAddress Endereço do dispositivo que deseja-se ler.
		 * @param [out] scratchPad ScratchPad lido.
		 */
		void readScratchPad(uint8_t* deviceAddress, uint8_t* scratchPad);

		// write device's scratchpad
		/**
		 * \brief Escreve os valores no ScratchPad do dispositivo e também na sua EEPROM.
		 *
		 * Os valores que podem ser escritos são as configurações de alarmes (byte HIGH_ALARM_TEMP e
		 * LOW_ALARM_TEMP do scratchpad), e para os dispositivos DS18B20 e DS1820 o byte de
		 * configuração (byte CONFIGURATION do scratchpad).
		 *
		 * @param [in] deviceAddress Endereço do dispositivo.
		 * @param [in] scratchPad Scratchpad com os valores que deseja escrever.
		 */
		void writeScratchPad(uint8_t* deviceAddress, const uint8_t* scratchPad);

		// read device's power requirements
		/**
		 * \brief Lê se o dispositivo está em modo parasita ou não.
		 *
		 * \note Modo parasita é quando a alimentação do dispositivo é feito pelo barramento de dados.
		 *
		 * @param [in] deviceAddress Endereço do dispositivo que será verificado.
		 * @return O modo de alimentação do dispositivo.
		 * @retval true Dispositivo em modo parasita.
		 * @retval false Dispositivo com alimentação normal.
		 */
		bool readPowerSupply(uint8_t* deviceAddress);

		// get global resolution
		/**
		 * \brief Retornar a resolução global do barramento.
		 *
		 * Resolução global é a resolução de maior valor dos dispositivos presentes no barramento.
		 *
		 * @return Valor da resolução global, entre 9 e 12, inclusive.
		 */
		uint8_t getResolution();

		// set global resolution to 9, 10, 11, or 12 bits
		/**
		 * \brief Configura resolução para todos os dispositivos no barramento.
		 *
		 * \note Modelos DS18S20 tem resolução fixa de 9 bits.
		 *
		 * @param [in] newResolution Resolução que será configurada. Deve ser um valor de 9 à 12. Caso não seja, será usado o valor mais próximo.
		 */
		void setResolution(uint8_t newResolution);

		// returns the device resolution, 9-12
		/**
		 * \brief Retorna a resolução de um dispositivo.
		 *
		 * @param [in] deviceAddress Endereço do dispositivo que deseja-se obter a resolução.
		 * @return Resolução configurarada no dispositivo.
		 * @retval 9-12 Valor da resolução lida.
		 * @retval 0 Dispositivo não encontrado.
		 */
		uint8_t getResolution(uint8_t* deviceAddress);

		// set resolution of a device to 9, 10, 11, or 12 bits
		/**
		 * \brief Configura a resolução para um dispositivo.
		 *
		 * Configura a resolução dado o endereço de um dispositivo.
		 *
		 * \note Modelos DS18S20 tem resolução fixa de 9 bits.
		 *
		 * @param [in] deviceAddress Endereço do dispositivo à configurar.
		 * @param [in] newResolution Resolução que será configurada.
		 * @return Sucesso ou não da operação.
		 * @retval true Sucesso ao configurar nova resolução.
		 * @retval false Erro ao configurar nova resolução.
		 */
		bool setResolution(uint8_t* deviceAddress, uint8_t newResolution);

		// sets/gets the waitForConversion flag
		/**
		 * \brief Configura se o dispositivo deve ou não esperar pela conversão da temperatura.
		 *
		 * Caso esteja configurado para esperar, a função só retorna ao terminar a conversão.
		 * No caso contrário, durante a conversão o programa principal pode realizar diversas
		 * operações, mas deve checar se o tempo de conversão já passou.
		 *
		 * @param [in] flag \b true Espera a convesão. \b false Retonar imediatamente.
		 */
		void setWaitForConversion(bool flag);
		/**
		 * \brief Retorna o valor da flag \a waitForConversion
		 *
		 * \see setResolution
		 *
		 * @return Valor da flag \a waitForConversion .
		 */
		bool getWaitForConversion(void);

		// sets/gets the checkForConversion flag
		/**
		 * \brief Configura a flag \a checkForConversion.
		 *
		 * Caso a flag esteja configurada com o valor \b true, verifica o barramento para
		 * para determinar se a conversão está completa. Caso o valor seja \b false, aguarda
		 * o tempo de conversão máximo.
		 *
		 * @param [in] flag \b true Verifica o barramento para determinar se a conversão está completa. \b false Aguarda o tempo máxima de conversão
		 */
		void setCheckForConversion(bool);

		/**
		 * \brief Retornar o valor da flag \a checkForConversion.
		 *
		 * \see setCheckForConversion
		 * @return Valor da flag \a checkForConversion configurada.
		 */
		bool getCheckForConversion(void);

		// sends command for all devices on the bus to perform a temperature conversion
		/**
		 * \brief Envia comando de conversão de temperatura para todos os dispositivos no barramento.
		 */
		void requestTemperatures(void);

		// sends command for one device to perform a temperature conversion by address
		/**
		 * \brief Requisita pelo endereço que um dispositivo realize a conversão de temperatura.
		 *
		 * \warning Dependendo de como a flag \a waitForConversion, o comportamento da função irá mudar.
		 * Caso seja \b true, a função irá retonar apenas quando a conversão estiver completa. Caso seja
		 * \b false, a função retornará imediamente.
		 *
		 * @param [in] deviceAddress Endereço do dispositivo que deseja que a convesão seja realizada.
		 * @return Sucesso ou não da operação.
		 * @retval true Requisição de temperatura feita com sucesso.
		 * @retval false Dispositivo não encontrado.
		 */
		bool requestTemperaturesByAddress(uint8_t* deviceAddress);

		// sends command for one device to perform a temperature conversion by index
		/**
		 * \brief Requisita a conversão da temperatura pela posição do dispositivo no barramento.
		 *
		 * \warning Dependendo de como a flag \a waitForConversion, o comportamento da função irá mudar.
		 * Caso seja \b true, a função irá retonar apenas quando a conversão estiver completa. Caso seja
		 * \b false, a função retornará imediamente.
		 *
		 * @param [in] index
		 * @return Sucesso ou não da operação.
		 * @retval true Requisição de temperatura feita com sucesso.
		 * @retval false Dispositivo não encontrado.
		 */
		bool requestTemperaturesByIndex(uint8_t index);

		// returns temperature in degrees C
		/**
		 * \brief Retorna o valor da temperatura convertida em Celsius.
		 *
		 * @param [in] deviceAddress Endereço do dispositivo que será lida a temperatura.
		 * @return Valor da temperatura lida.
		 * @retval DEVICE_DISCONNECTED Erro ao receber temperatura.
		 * @retval OUTROS Temperatura convertida.
		 */
		float getTempC(uint8_t* deviceAddress);

		// returns temperature in degrees F
		/**
		 * \brief Retorna o valor da temperatura convertida em Fahrenheit.
		 *
		 * @param [in] deviceAddress Endereço do dispositivo que será lida a temperatura.
		 * @return Valor da temperatura lida.
		 * @retval -196.6 Erro ao receber temperatura.
		 * @retval OUTROS Temperatura convertida.
		 */
		float getTempF(uint8_t* deviceAddress);

		// Get temperature for device index (slow)
		/**
		 * \brief Retonar o valor da temperatura em Celsius dado a posição do sensor no barramento.
		 *
		 * @param [in] index Posição do sensor.
		 * @return Valor da temperatura lida.
		 * @retval DEVICE_DISCONNECTED Erro ao receber temperatura.
		 * @retval OUTROS Temperatura convertida.
		 */
		float getTempCByIndex(uint8_t index);

		// Get temperature for device index (slow)
		/**
		 * \brief Retorna o valor da temperatura convertida em Fahrenheit dado a posição do sensor no barramento.
		 *
		 * @param [in] index Posição do sensor.
		 * @return Valor da temperatura lida.
		 * @retval -196.6 Erro ao receber temperatura.
		 * @retval OUTROS Temperatura convertida.
		 */
		float getTempFByIndex(uint8_t index);

		// returns true if the bus requires parasite power
		/**
		 * \brief Retorna \b true caso algum dispositivo no barramento está em parasite mode.
		 *
		 * @return Valor global do modo parasita.
		 * @retval true Ao menos um dispositivo no barramento está em modo parasita.
		 * @retval false Nenhnum dispositivo está em modo parasita.
		 */
		bool isParasitePowerMode(void);

		bool isConversionAvailable(uint8_t*);

		typedef void AlarmHandler(uint8_t*);

		// sets the high alarm temperature for a device
		// accepts a char.  valid range is -55C - 125C
		void setHighAlarmTemp(uint8_t*, const signed char);

		// sets the low alarm temperature for a device
		// accepts a char.  valid range is -55C - 125C
		void setLowAlarmTemp(uint8_t*, const signed char);

		// returns a signed char with the current high alarm temperature for a device
		// in the range -55C - 125C
		char getHighAlarmTemp(uint8_t*);

		// returns a signed char with the current low alarm temperature for a device
		// in the range -55C - 125C
		char getLowAlarmTemp(uint8_t*);

		// resets internal variables used for the alarm search
		void resetAlarmSearch(void);

		// search the wire for devices with active alarms
		bool alarmSearch(uint8_t*);

		// returns true if ia specific device has an alarm
		bool hasAlarm(uint8_t*);

		// returns true if any device is reporting an alarm on the bus
		bool hasAlarm(void);

		// runs the alarm handler for all devices returned by alarmSearch()
		void processAlarms(void);

		// sets the alarm handler
		void setAlarmHandler(AlarmHandler *);

		// The default alarm handler
		static void defaultAlarmHandler(uint8_t*);

	private:
		/**
		 * \brief Tipo que define o ScratchPad dos dispositivos.
		 */
		typedef uint8_t ScratchPad[9];

		// parasite power on or off
		/**
		 * \brief Determina o modo de operação do barramento (modo parasita ou não).
		 *
		 * Caso ao menos um dispositivo esteja em modo parasita, será configura \b true,
		 * caso contrário, \b false.
		 */
		bool parasite;

		// used to determine the delay amount needed to allow for the
		// temperature conversion to take place
		/**
		 * \brief Resolução do dispositivo de maior resolução no barramento.
		 */
		uint8_t bitResolution;

		// used to requestTemperature with or without delay
		/**
		 * \brief Determina como as funções requestTemperature devem se comportar.
		 *
		 * Caso seja \b true, as funções requestTemperature aguardam o fim da conversão para
		 * retonar, em caso contrário, \b false, retornam imediatamente.
		 */
		bool waitForConversion;

		// used to requestTemperature to dynamically check if a conversion is complete
		/**
		 * \brief Define o modo como será verificado o fim da conversão de temperatura.
		 *
		 * Caso seja \b true, irá verificar se a conversão foi completada pelo barramento, caso
		 * seja \b false, irá aguardar o tempo máximo para a conversão.
		 */
		bool checkForConversion;

		// count of devices on the bus
		/**
		 * \brief Define número de dispositivos no barramento.
		 */
		uint8_t devices;

		// Take a pointer to one wire instance
		/**
		 * \brief Determina a porta digital que será utilizada o protocolo ONEWIRE.
		 */
		OneWire* wire;

		// reads scratchpad and returns the temperature in degrees C
		/**
		 * \brief Faz o calculo da temperatura do dispositivo baseado em seu tipo e na resolução configurada.
		 *
		 * @param [in] deviceAddress Endereço do dispositivo.
		 * @param [in] scratchPad Scratchpad do dispositivo.
		 * @return Temperatura calculada.
		 */
		float calculateTemperature(uint8_t* deviceAddress, uint8_t* scratchPad);

		/**
		 * \brief Utilizado para bloquer a função requestTemperature.
		 *
		 * @param [in] bitResolution Resolução do dispositivo.
		 * @param [in] deviceAddress Endereço do dispositivo.
		 */
		void blockTillConversionComplete(uint8_t* bitResolution,uint8_t* deviceAddress);

		// required for alarmSearch
		uint8_t alarmSearchAddress[8];
		char alarmSearchJunction;
		uint8_t alarmSearchExhausted;

		// the alarm handler function pointer
		AlarmHandler *_AlarmHandler;

};

/**@}*/

#endif /* DALLASTEMPERATURE_H__ */
