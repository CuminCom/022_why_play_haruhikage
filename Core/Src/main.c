/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "tim.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
typedef struct
{
    uint16_t freq_hz;      /* 0 表示休止符 */
    uint16_t duration_16;  /* 时值，单位：十六分音符 */
} MusicNote;

typedef struct
{
    const MusicNote *notes;
    uint16_t count;
    uint16_t bpm;          /* 每分钟四分音符数 */
} Song;
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define NOTE_REST 0U

#define NOTE_FS3 185U
#define NOTE_B3  247U

#define NOTE_C4  262U
#define NOTE_CS4 277U
#define NOTE_D4  294U
#define NOTE_DS4 311U
#define NOTE_E4  330U
#define NOTE_F4  349U
#define NOTE_FS4 370U
#define NOTE_G4  392U
#define NOTE_GS4 415U
#define NOTE_A4  440U
#define NOTE_AS4 466U
#define NOTE_B4  494U

#define NOTE_C5  523U
#define NOTE_CS5 554U
#define NOTE_D5  587U
#define NOTE_DS5 622U
#define NOTE_E5  659U
#define NOTE_F5  698U
#define NOTE_FS5 740U
#define NOTE_G5  784U
#define NOTE_GS5 831U
#define NOTE_A5  880U
#define NOTE_AS5 932U
#define NOTE_B5  988U

#define NOTE_C6  1047U

/* 时值以十六分音符为单位：四分音符 = 4，二分音符 = 8。 */
#define LEN_16TH 1U
#define LEN_8TH  2U
#define LEN_D8TH 3U
#define LEN_4TH  4U
#define LEN_D4TH 6U
#define LEN_2ND  8U
#define LEN_D2ND 12U
#define LEN_1ST  16U

#define ARRAY_LEN(a) ((uint16_t)(sizeof(a) / sizeof((a)[0])))

/* 启动时播放的歌曲。新增歌曲时，在 g_song_library 中登记并增加一个编号。 */
#define SONG_LITTLE_STAR 0U
#define SONG_HARUHIKAGE  1U
#define STARTUP_SONG     SONG_HARUHIKAGE
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
/* 歌曲数据只描述“音高 + 时值”，播放器不关心具体是哪一首歌。 */
static const MusicNote g_little_star_notes[] =
{
    {NOTE_C5, LEN_4TH}, {NOTE_C5, LEN_4TH}, {NOTE_G5, LEN_4TH}, {NOTE_G5, LEN_4TH},
    {NOTE_A5, LEN_4TH}, {NOTE_A5, LEN_4TH}, {NOTE_G5, LEN_2ND}, {NOTE_REST, LEN_8TH},

    {NOTE_F5, LEN_4TH}, {NOTE_F5, LEN_4TH}, {NOTE_E5, LEN_4TH}, {NOTE_E5, LEN_4TH},
    {NOTE_D5, LEN_4TH}, {NOTE_D5, LEN_4TH}, {NOTE_C5, LEN_2ND}, {NOTE_REST, LEN_8TH},

    {NOTE_G5, LEN_4TH}, {NOTE_G5, LEN_4TH}, {NOTE_F5, LEN_4TH}, {NOTE_F5, LEN_4TH},
    {NOTE_E5, LEN_4TH}, {NOTE_E5, LEN_4TH}, {NOTE_D5, LEN_2ND}, {NOTE_REST, LEN_8TH},

    {NOTE_G5, LEN_4TH}, {NOTE_G5, LEN_4TH}, {NOTE_F5, LEN_4TH}, {NOTE_F5, LEN_4TH},
    {NOTE_E5, LEN_4TH}, {NOTE_E5, LEN_4TH}, {NOTE_D5, LEN_2ND}, {NOTE_REST, LEN_8TH},

    {NOTE_C5, LEN_4TH}, {NOTE_C5, LEN_4TH}, {NOTE_G5, LEN_4TH}, {NOTE_G5, LEN_4TH},
    {NOTE_A5, LEN_4TH}, {NOTE_A5, LEN_4TH}, {NOTE_G5, LEN_2ND}, {NOTE_REST, LEN_8TH},

    {NOTE_F5, LEN_4TH}, {NOTE_F5, LEN_4TH}, {NOTE_E5, LEN_4TH}, {NOTE_E5, LEN_4TH},
    {NOTE_D5, LEN_4TH}, {NOTE_D5, LEN_4TH}, {NOTE_C5, LEN_2ND}, {NOTE_REST, LEN_8TH}
};

static const Song g_little_star_song =
{
    g_little_star_notes,
    ARRAY_LEN(g_little_star_notes),
    120U
};

/* 《春日影》括号内前奏及图中其后可见的主旋律。1=B，6/8，约 37.4 秒。 */
static const MusicNote g_haruhikage_notes[] =
{
    /* 括号内前奏：两小节动机重复四次 */
    {NOTE_DS5, LEN_4TH},  {NOTE_CS5, LEN_8TH},  {NOTE_B4, LEN_4TH},   {NOTE_CS5, LEN_8TH},
    {NOTE_DS5, LEN_D8TH}, {NOTE_E5, LEN_16TH},  {NOTE_DS5, LEN_8TH},  {NOTE_CS5, LEN_D4TH},

    {NOTE_DS5, LEN_4TH},  {NOTE_CS5, LEN_8TH},  {NOTE_B4, LEN_4TH},   {NOTE_CS5, LEN_8TH},
    {NOTE_DS5, LEN_D8TH}, {NOTE_E5, LEN_16TH},  {NOTE_DS5, LEN_8TH},  {NOTE_CS5, LEN_D4TH},

    {NOTE_DS5, LEN_4TH},  {NOTE_CS5, LEN_8TH},  {NOTE_B4, LEN_4TH},   {NOTE_CS5, LEN_8TH},
    {NOTE_DS5, LEN_D8TH}, {NOTE_E5, LEN_16TH},  {NOTE_DS5, LEN_8TH},  {NOTE_CS5, LEN_D4TH},

    {NOTE_DS5, LEN_4TH},  {NOTE_CS5, LEN_8TH},  {NOTE_B4, LEN_4TH},   {NOTE_CS5, LEN_8TH},
    {NOTE_DS5, LEN_D8TH}, {NOTE_E5, LEN_16TH},  {NOTE_DS5, LEN_8TH},  {NOTE_CS5, LEN_D4TH},

    /* 前奏之后：图中可见部分 */
    {NOTE_B3, LEN_16TH},  {NOTE_CS4, LEN_16TH},

    {NOTE_DS4, LEN_8TH},  {NOTE_DS4, LEN_8TH},  {NOTE_CS4, LEN_8TH},
    {NOTE_E4, LEN_8TH},   {NOTE_DS4, LEN_8TH},  {NOTE_CS4, LEN_8TH},

    {NOTE_CS4, LEN_8TH},  {NOTE_CS4, LEN_8TH},  {NOTE_B3, LEN_16TH},
    {NOTE_B3, LEN_16TH},  {NOTE_E4, LEN_8TH},   {NOTE_DS4, LEN_8TH},  {NOTE_CS4, LEN_8TH},

    {NOTE_CS4, LEN_4TH},  {NOTE_B3, LEN_16TH},  {NOTE_CS4, LEN_16TH}, {NOTE_DS4, LEN_D4TH},
    {NOTE_REST, LEN_D4TH},{NOTE_DS4, LEN_8TH},  {NOTE_FS4, LEN_8TH},
    {NOTE_B4, LEN_16TH},  {NOTE_AS4, LEN_16TH},

    {NOTE_AS4, LEN_4TH},  {NOTE_B4, LEN_8TH},   {NOTE_AS4, LEN_4TH},  {NOTE_B4, LEN_8TH},
    {NOTE_AS4, LEN_16TH}, {NOTE_GS4, LEN_16TH},{NOTE_FS4, LEN_4TH},
    {NOTE_FS4, LEN_8TH},  {NOTE_CS4, LEN_8TH},  {NOTE_E4, LEN_8TH},

    {NOTE_E4, LEN_4TH},   {NOTE_DS4, LEN_8TH},  {NOTE_DS4, LEN_4TH},  {NOTE_FS3, LEN_8TH},
    {NOTE_E4, LEN_8TH},   {NOTE_DS4, LEN_8TH},  {NOTE_CS4, LEN_8TH},
    {NOTE_DS4, LEN_4TH},  {NOTE_FS4, LEN_8TH},

    {NOTE_B3, LEN_D4TH},  {NOTE_REST, LEN_4TH}, {NOTE_B3, LEN_8TH},
    {NOTE_CS4, LEN_8TH},  {NOTE_B3, LEN_D8TH},  {NOTE_B3, LEN_16TH},
    {NOTE_B3, LEN_8TH},   {NOTE_FS4, LEN_8TH},  {NOTE_B3, LEN_8TH},

    {NOTE_E4, LEN_4TH},   {NOTE_DS4, LEN_8TH},  {NOTE_CS4, LEN_8TH},
    {NOTE_B3, LEN_8TH},   {NOTE_B3, LEN_8TH},

    {NOTE_B3, LEN_D4TH},  {NOTE_REST, LEN_4TH}
};

static const Song g_haruhikage_song =
{
    g_haruhikage_notes,
    ARRAY_LEN(g_haruhikage_notes),
    97U
};

/* 多首歌曲共用同一个播放器，通过编号选择。 */
static const Song *const g_song_library[] =
{
    &g_little_star_song,
    &g_haruhikage_song
};
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
static void buzzer_set_freq(uint32_t freq_hz);
static void music_play(const Song *song);
static void music_task(void);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_TIM3_Init();
  /* USER CODE BEGIN 2 */
music_play(g_song_library[STARTUP_SONG]);
/* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
    music_task();
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
#define MUSIC_NOTE_GAP_MS 10U

static const Song *g_current_song = 0;
static uint16_t g_note_index = 0U;
static uint32_t g_note_start_ms = 0U;
static uint8_t g_music_state = 0U; /* 0=空闲，1=准备音符，2=发声，3=音符间隙 */

static void buzzer_set_freq(uint32_t freq_hz)
{
    uint32_t period;

    if (freq_hz == 0U)
    {
        __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, 0U);
        return;
    }

    /* TIM3 计数频率为 1 MHz，因此 ARR + 1 = 1 MHz / 目标频率。 */
    period = 1000000UL / freq_hz;

    if (period < 2U)
    {
        period = 2U;
    }
    else if (period > 65536U)
    {
        period = 65536U;
    }

    __HAL_TIM_SET_AUTORELOAD(&htim3, period - 1U);
    __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, period / 2U);
    __HAL_TIM_SET_COUNTER(&htim3, 0U);
}

static uint32_t music_note_duration_ms(const MusicNote *note, const Song *song)
{
    /* 十六分音符时长 = 60000 / BPM / 4。 */
    return ((uint32_t)note->duration_16 * 60000U) /
           ((uint32_t)song->bpm * 4U);
}

static void music_stop(void)
{
    buzzer_set_freq(0U);
    HAL_TIM_PWM_Stop(&htim3, TIM_CHANNEL_1);
    g_current_song = 0;
    g_music_state = 0U;
}

static void music_play(const Song *song)
{
    buzzer_set_freq(0U);

    if ((song == 0) || (song->notes == 0) ||
        (song->count == 0U) || (song->bpm == 0U))
    {
        g_current_song = 0;
        g_music_state = 0U;
        return;
    }

    g_current_song = song;
    g_note_index = 0U;
    g_note_start_ms = HAL_GetTick();
    g_music_state = 1U;

    /* 重复调用 music_play() 时，PWM 已经在运行时返回 HAL_BUSY，可忽略。 */
    (void)HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_1);
}

static void music_task(void)
{
    uint32_t now;
    uint32_t duration_ms;
    uint32_t tone_ms;

    if (g_current_song == 0)
    {
        return;
    }

    now = HAL_GetTick();

    switch (g_music_state)
    {
        case 1U:
            if (g_note_index >= g_current_song->count)
            {
                music_stop();
                break;
            }

            buzzer_set_freq(g_current_song->notes[g_note_index].freq_hz);
            g_note_start_ms = now;
            g_music_state = 2U;
            break;

        case 2U:
            duration_ms = music_note_duration_ms(
                &g_current_song->notes[g_note_index], g_current_song);

            tone_ms = (duration_ms > MUSIC_NOTE_GAP_MS)
                    ? (duration_ms - MUSIC_NOTE_GAP_MS)
                    : 0U;

            if ((uint32_t)(now - g_note_start_ms) >= tone_ms)
            {
                buzzer_set_freq(0U);
                g_note_start_ms = now;
                g_music_state = 3U;
            }
            break;

        case 3U:
            if ((uint32_t)(now - g_note_start_ms) >= MUSIC_NOTE_GAP_MS)
            {
                g_note_index++;
                g_music_state = 1U;
            }
            break;

        default:
            break;
    }
}
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
