#include "LED_Blink_1.h"
#include "stm32f1xx_hal.h"

void LED_YELLOW_Blink()
{
	HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
	HAL_Delay(LED_BLINK_TIME);

}
