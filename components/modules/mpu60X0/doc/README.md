# MPU60X0

## Descrição

Utilização acelerômetro, giroscópio e também contém um sensor de temperatura. A biblioteca é feita para os dispositivos MPU6000 e MPU6050. A diferença é que o dispositivo MPU6000 aceita também conexão SPI (não implementado).

## Configuração

As configuração do dispositivo é feita pela *struct* **settings**, que conta com os seguintes campos:
* **uint8_t pwr_mgmt_1** - configura o *clock source*, desabilita o sensor de temperatura, modo cíclico, modo _sleep_ e _resetar_ o dispositvo. Ver _enum_ **Power_Mgmt_1** e registrador 0x6B no [Mapa de registradores][mp];
* **uint8_t pwr_mgmt_2** - configura a frequência que o dispositivo acorda no modo cíclico, e desabilita individualmente qualquer um dos eixos do giroscópio e/ou acelerômetro. Ver *enum* **Power_Mgmt_2** e registrador 0x6C no [Mapa de registradores][mp];
* **uint8_t config** - configura o filtro passa baixa do dispositivo. Configurando para frequências mais baixas, suaviza as variações do dispositivo. Modificando do valor padrão (*Low_Pass_260Hz*) faz com que haja variação do cálculo do *sample rate* do dispositivo. Ver _enum_ **Config** e registrador 0x1A no [Mapa de registradores][mp];
* **uint8_t sample_rate_div** - Define um fator que será usado no cálculo do *sample rate* do dispositvo. Ver registrador 0x19 no [Mapa de registradores][mp];
* **uint8_t fifo_en** - Habilita quais dados serão copiados para o *buffer* do dispositivo. . Habilitando qualquer um dos parâmetros, também será habilitado a resetado o buffer. Os dados que podem ser copiados são todos os eixos do acelerômetro, a temperatura, e cada um dos eixos do giroscópio. Ver *enum* **FIFO_Enable** e registrador 0x23 no [Mapa de registradores][mp];
* **Acc_Scale acc** - Define a escala do acelerômetro, podendo ser +-2g, +-4g, +-8g, +-16g, onde *g* é aceleração da gravidade. Ver *enum* **Acc_Scale** e registrador 0x1C no [Mapa de registradores][mp];
* **Gyro_Scale gyro** - Define a escala do giroscópio, podendo ser +-250 dec/s, +-500 dec/s, +-1000 dec/s, +-2000 dec/s, onde *dec/s* é graus por segundos. Ver *enum* **Gyro_Scale** e registrador 0x1B no [Mapa de registradores][mp];

### Cálculo do *sample rate*

O cálculo do *sample rate* (taxa de frequência de amostragem) é feito pela seguinte fórmula:

> Sample Rate = Gyroscope Output Rate / (1 + SMPLRT_DIV)

E "Gyroscope Output Rate":
* 8 khz para **config** == Low_Pass_260Hz;
* 1 khz para **config** != Low_Pass_260Hz

Assim para 8khz, alguns valores:

| SMPLRT_DIV | Sample Rate (hz) |
|----:|----:|
| 0  | 8000  |
| 10  |  727 |
| 100  |  79 |
| 200  |  39 |
| 255  |  31 |

E para 1khz:

| SMPLRT_DIV | Sample Rate (hz) |
|----:|----:|
| 0  | 1000  |
| 10  |  90 |
| 100  |  9 |
| 200  |  4 |
| 255  |  3 |

### Exemplos de configuração

A _struct settings_ após configurada como mostrado abaixo, deve ser chamado pelo método *config*.

	MPU60X0 sensor(&i2c);
	sensor.init();
	MPU60X0::settings s;
	
	<configurações demostradas abaixo>
	
	sensor.config(&s);

#### Configuração padrão
	MPU60X0::settings s;
	s.acc = MPU60X0::ACC_2G;			//Escala do acelerômetro +-2g
	s.gyro = MPU60X0::GYRO_2000;			//Escala do giroscópio +- 2000 dec/s
	s.pwr_mgmt_1 = MPU60X0::Internal_Osc_8Mhz;	//Fonte do oscilador interno de 8Mhz 
	s.pwr_mgmt_2 = MPU60X0::Stand_By_Acc_X		//Desabilitando acelerômetro no eixo X 
					| MPU60X0::Stand_By_Acc_Z 	//Desabilitando acelerômetro no eixo Z
					| MPU60X0::Stand_By_Gyro_Y;	//Desabilitando giroscópio no eixo Y
	s.config = MPU60X0::Low_Pass_5Hz;		//Habilitando filtro passa baixa de 5Hz
	s.sample_rate_div = 255;			//Divisor da taxa de amostragem = 255 
													//(taxa de amostragem = 3hz, ver sessão anterior)

#### Utilizando o *buffer* FIFO
	MPU60X0::settings s;
	s.acc = MPU60X0::ACC_8G;			//Escala do acelerômetro +-8g
	s.gyro = MPU60X0::GYRO_250;			//Escala do giroscópio +- 250 dec/s
	//s.pwr_mgmt_1 = MPU60X0::Internal_Osc_8Mhz;	//Já é a configuração padrão
	//s.pwr_mgmt_2 = 0;					//Já é a configuração padrão (habilitado todos os sensores)
	s.config = MPU60X0::Low_Pass_260Hz;		//Habilitando filtro passa baixa de 260Hz
	s.sample_rate_div = 100;			//Divisor da taxa de amostragem = 100 
													//(taxa de amostragem = 79hz, ver sessão anterior)
	s.fifo_en = MPU60X0::acc_en		//Habilita dados do acelerômetro na FIFO 
				| MPU60X0::gyro_en 	//Habilita dados do giroscópio na FIFO 
										//(os eixos do giroscópio podem ser habilitados individualmente;
				| MPU60X0::temp_en;	//Habilita dados de temperatura na FIFO
#### Modo cíclico

Modo cíclico ainda não está funcioando a contento. Em teoria, devia ser adicionado a uma das configurações acima:

	s.pwr_mgmt_1 = MPU60X0::Internal_Osc_8Mhz 
					| MPU60X0::Cycle; 		// Configuração do modo cíclico
	s.pwr_mgmt_2 = MPU60X0::LP_Wake_5Hz;		// Frequência que o dispositivo acordaria

## Referência

* [pp]: <https://www.invensense.com/products/motion-tracking/6-axis/mpu-6050/>
* [dt]: <https://www.invensense.com/wp-content/uploads/2015/02/MPU-6000-Datasheet1.pdf> 	
* [mp]: <https://www.invensense.com/wp-content/uploads/2015/02/MPU-6000-Register-Map1.pdf>

