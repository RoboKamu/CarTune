/*!
  @brief      increment the clock by 1 minute 
  @param      pHour: pointer to initial hour
  @param      pMinute: pointer to initial minute
*/
void incrementClock(int *pHour, int *pMinute);

/*!
  @brief      decrement the clock by 1 minute
  @param      pHour: pointer to initial hour
  @param      pMinute: pointer to initial minute
*/
void decrementClock(int *pHour, int *pMinute);

/*!
  @brief      increment the clock by 1 millisecond 
  @param      pMs: pointer to initial millisecond
  @param      pHour: pointer to initial hour
  @param      pMinute: pointer to initial minute
*/
void tick(int *pMs, int *pHour, int *pMinute);