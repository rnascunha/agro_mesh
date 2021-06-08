#BME280

Bliblioteca feita em C++ baseada na API oficial da Bosch (<https://github.com/BoschSensortec/BME280_driver>)

##Configuracao

A biblioteca pode ser utilizada utilizando saída como *ponto flutuante* ou *inteiro*. Na opção de inteiro, ainda pode ser escolhida a opção de utilização de 64bits, aumentando a precisão do cálculo de pressão. Assim:

Ponto flutuante:

	#define BME280_FLOAT_ENABLE
	
Para uso utilização de 64bits no cálculo da pressão:

	//#define BME280_FLOAT_ENABLE
	#define BME280_64BIT_ENABLE		1
	
Para não usar números de 64bits:

	//#define BME280_FLOAT_ENABLE
	#define BME280_64BIT_ENABLE		0
	
##Saída

Usando saída de *ponto flutuante*:
- °C for temperature
- % relative humidity
- Pascal for pressure
In case if "BME280_FLOAT_ENABLE" is not enabled, then it is

Usando saída como *inteiro*:
- int32_t for temperature with the units 100 * °C
- uint32_t for humidity with the units 1024 * % relative humidity
- uint32_t for pressure
     If macro "BME280_64BIT_ENABLE" is enabled, which it is by default, the unit is 100 * Pascal
     If this macro is disabled, Then the unit is in Pascal