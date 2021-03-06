#include "mbed.h"
#include "AUDIO_DISCO_F746NG.h"
#include "SDRAM_DISCO_F746NG.h"
#include "math.h"

AUDIO_DISCO_F746NG audio;
// audio IN_OUT buffer is stored in the SDRAM, SDRAM needs to be initialized and FMC enabled
SDRAM_DISCO_F746NG sdram;

DigitalOut led_green(LED1);
DigitalOut led_red(LED2);
Serial pc(USBTX, USBRX);
uint16_t tab[1024] = {294 ,294,85,85,171,171 ,65305 ,65305 ,65517 ,65517 ,252 ,252 ,97 ,97 ,7 ,7 ,7 ,28 ,28 ,65461 ,65461 ,65424 ,65424 ,65429 ,65429 ,65127 ,2836 ,2836 ,2802 ,2802 ,64876 ,64876 ,62038 ,62038 ,1208 ,1208 };
//uint16_t tab[1024] = {5,65000,5,65000,5,65000,5,65000,5,65000,5,65000,5,65000,5,65000,5,65000,5,65000,5,65000,5,65000,5,65000,5,65000,5,65000,5,65000,5,65000,5,65000,5,65000,5,65000,5,65000,5,65000,5,65000,5,65000,5,65000,5,65000,5};
typedef enum
{
    BUFFER_OFFSET_NONE = 0,
    BUFFER_OFFSET_HALF = 1,
    BUFFER_OFFSET_FULL = 2,
}BUFFER_StateTypeDef;

#define AUDIO_BLOCK_SIZE   ((uint32_t)1024)
#define AUDIO_BUFFER_IN     SDRAM_DEVICE_ADDR     /* In SDRAM */
#define AUDIO_BUFFER_OUT   SDRAM_DEVICE_ADDR /* In SDRAM */
__IO uint32_t  audio_rec_buffer_state = BUFFER_OFFSET_NONE;
static uint8_t SetSysClock_PLL_HSE_200MHz();
int main()
{
    int i = 0;
    double samples[AUDIO_BLOCK_SIZE];
    uint16_t tabt[AUDIO_BLOCK_SIZE];
    double frequency =400;
                           
     double samples_length = AUDIO_BLOCK_SIZE;               
    for (i=0; i < samples_length ; i++) { 
        double t = i/samples_length;               
       //samples[i] = ((sin( frequency * 2*3.14159265359*i );
       samples[i] = (sin(frequency * (2 * 3.14159265359) * i / samples_length)+1)/2;
        pc.printf("S %d %f \r\n",i,samples[i]);
        tabt[i] = floor(samples[i]*65535);
        
        pc.printf("T %d \r\n",tabt[i]);
        
    }
    
    SetSysClock_PLL_HSE_200MHz();
    pc.baud(9600);

    pc.printf("\n\nAUDIO LOOPBACK EXAMPLE START:\n");
    led_red = 0;
  
    pc.printf("\nAUDIO RECORD INIT OK\n");
    pc.printf("Microphones sound streamed to headphones\n");
    
    /* Initialize SDRAM buffers */
    memset((uint16_t*)AUDIO_BUFFER_IN, 0, AUDIO_BLOCK_SIZE);
    memset((uint16_t*)AUDIO_BUFFER_OUT, 0, AUDIO_BLOCK_SIZE);
    memcpy((uint16_t *) AUDIO_BUFFER_IN , tabt, AUDIO_BLOCK_SIZE);
    audio_rec_buffer_state = BUFFER_OFFSET_FULL;

    /* Start Recording */
    
    //audio.IN_Record((uint16_t*)AUDIO_BUFFER_IN, AUDIO_BLOCK_SIZE);
    

    /* Start Playback */
    audio.OUT_SetAudioFrameSlot(CODEC_AUDIOFRAME_SLOT_02);
    audio.OUT_Play((uint16_t*)AUDIO_BUFFER_OUT, AUDIO_BLOCK_SIZE);
    audio.OUT_SetVolume(30);
    memcpy((uint16_t *)(AUDIO_BUFFER_OUT), (uint16_t *)(AUDIO_BUFFER_IN), AUDIO_BLOCK_SIZE);
    //memcpy((uint16_t *) (AUDIO_BUFFER_OUT + (AUDIO_BLOCK_SIZE)), (uint16_t *)(AUDIO_BUFFER_IN)  , AUDIO_BLOCK_SIZE);
    while (1) {
        /* Wait end of half block recording */
       //while(audio_rec_buffer_state == BUFFER_OFFSET_HALF) {
        //}
        //audio_rec_buffer_state = BUFFER_OFFSET_NONE;
        /* Copy recorded 1st half block */
        //memcpy((uint16_t *)(AUDIO_BUFFER_OUT), (uint16_t *)(AUDIO_BUFFER_IN), AUDIO_BLOCK_SIZE);
        //memcpy((uint16_t *) AUDIO_BUFFER_OUT, (uint16_t *)(tabt), AUDIO_BLOCK_SIZE);
        /* Wait end of one block recording */
        
        //while(audio_rec_buffer_state == BUFFER_OFFSET_FULL) {
        //}
        //audio_rec_buffer_state = BUFFER_OFFSET_NONE;
        /* Copy recorded 2nd half block */
        ///memcpy((uint16_t *)(AUDIO_BUFFER_OUT + (AUDIO_BLOCK_SIZE)), (uint16_t *)(AUDIO_BUFFER_IN + (AUDIO_BLOCK_SIZE)), AUDIO_BLOCK_SIZE);
         //memcpy((uint16_t *) (AUDIO_BUFFER_OUT + (AUDIO_BLOCK_SIZE)), (uint16_t *)(tabt)  , AUDIO_BLOCK_SIZE);
        
        //pc.printf("%d \r\n",tab[0]);
        //wait(0.5);
        //audio.OUT_SetVolume(0);
        //wait(1);
        //audio.OUT_SetVolume(30);
       
    }
}
/*-------------------------------------------------------------------------------------
       Callbacks implementation:
           the callbacks API are defined __weak in the stm32746g_discovery_audio.c file
           and their implementation should be done in the user code if they are needed.
           Below some examples of callback implementations.
  -------------------------------------------------------------------------------------*/
/**
  * @brief Manages the DMA Transfer complete interrupt.
  * @param None
  * @retval None
  */
void BSP_AUDIO_IN_TransferComplete_CallBack(void)
{
  audio_rec_buffer_state = BUFFER_OFFSET_FULL;
  return;
}

/**
  * @brief  Manages the DMA Half Transfer complete interrupt.
  * @param  None
  * @retval None
  */
void BSP_AUDIO_IN_HalfTransfer_CallBack(void)
{
  audio_rec_buffer_state = BUFFER_OFFSET_HALF;
  return;
}

static uint8_t SetSysClock_PLL_HSE_200MHz()
{
  RCC_ClkInitTypeDef RCC_ClkInitStruct;
  RCC_OscInitTypeDef RCC_OscInitStruct;

  // Enable power clock  
  __PWR_CLK_ENABLE();
  
  // Enable HSE oscillator and activate PLL with HSE as source
  RCC_OscInitStruct.OscillatorType      = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState            = RCC_HSE_ON; /* External xtal on OSC_IN/OSC_OUT */

  // Warning: this configuration is for a 25 MHz xtal clock only
  RCC_OscInitStruct.PLL.PLLState        = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource       = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM            = 25;            // VCO input clock = 1 MHz (25 MHz / 25)
  RCC_OscInitStruct.PLL.PLLN            = 400;           // VCO output clock = 400 MHz (1 MHz * 400)
  RCC_OscInitStruct.PLL.PLLP            = RCC_PLLP_DIV2; // PLLCLK = 200 MHz (400 MHz / 2)
  RCC_OscInitStruct.PLL.PLLQ            = 8;             // USB clock = 50 MHz (400 MHz / 8)
  
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    return 0; // FAIL
  }

  // Activate the OverDrive to reach the 216 MHz Frequency
  if (HAL_PWREx_EnableOverDrive() != HAL_OK)
  {
    return 0; // FAIL
  }
  
  // Select PLL as system clock source and configure the HCLK, PCLK1 and PCLK2 clocks dividers
  RCC_ClkInitStruct.ClockType      = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
  RCC_ClkInitStruct.SYSCLKSource   = RCC_SYSCLKSOURCE_PLLCLK; // 200 MHz
  RCC_ClkInitStruct.AHBCLKDivider  = RCC_SYSCLK_DIV1;         // 200 MHz
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;           //  50 MHz
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;           // 100 MHz
  
  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_7) != HAL_OK)
  {
    return 0; // FAIL
  }
  HAL_RCC_MCOConfig(RCC_MCO1, RCC_MCO1SOURCE_HSE, RCC_MCODIV_4);
  return 1; // OK
}
