#include "eclicw.h"
#include "gd32vf103.h"

static void (*pmtisr)(void)=NULL;
static void (*pu0tbeisr)(void)=NULL;

void eclicw_enable(int irqn, int level, int priority, void (*pISR)(void)){
   static uint32_t max_irqn=0;
   if (!max_irqn) {                             // Init Int vector table if not done!
        max_irqn = *( volatile uint32_t * )( ECLIC_ADDR_BASE + ECLIC_INFO_OFFSET );
        max_irqn &= ( 0x00001FFF );
        eclic_init( max_irqn );
        eclic_mode_enable();
    }
    
   eclic_set_vmode( irqn );                     // Manage the int through the vector table.
   eclic_enable_interrupt( irqn );              // Enable the selected interrupt...
   eclic_set_irq_lvl_abs( irqn, level );        // ...with selected level and...
   eclic_set_irq_priority( irqn, priority );    // ...selected priorit!

   switch (irqn) {
       case CLIC_INT_TMR : pmtisr=pISR;         // Save call-back to int's ISR.
       case USART0_IRQn  : pu0tbeisr=pISR;
   }
}

__attribute__( ( interrupt ) )
void eclic_mtip_handler( void ) {               // c-wrapper saves environment...
  (*pmtisr)();                                  // ...Call int's ISR...
}                                               // and restores environment (also (G)IE)!

__attribute__( ( interrupt ) )                  // !!! ALL USART0 INT GOES HERE !!!
void USART0_IRQHandler( void ) {                // c-wrapper saves environment...
  (*pu0tbeisr)();                               // ...Call int's ISR...
}                                               // and restores environment (also (G)IE)!

