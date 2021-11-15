//===================================================================================
//Data: 01/11/2021
//Autora: Marina Lourenço Borges
//Descrição: Lab_1 - Microcontrolados S23
//===================================================================================

//1. Declarações para o Software
//Declaração da definição da Porta F
#include "TExaS.h"
#define GPIO_PORTF_DATA_R       (*((volatile unsigned long *)0x400253FC))
#define GPIO_PORTF_DIR_R        (*((volatile unsigned long *)0x40025400))
#define GPIO_PORTF_AFSEL_R      (*((volatile unsigned long *)0x40025420))
#define GPIO_PORTF_PUR_R        (*((volatile unsigned long *)0x40025510))
#define GPIO_PORTF_DEN_R        (*((volatile unsigned long *)0x4002551C))
#define GPIO_PORTF_LOCK_R       (*((volatile unsigned long *)0x40025520))
#define GPIO_PORTF_CR_R         (*((volatile unsigned long *)0x40025524))
#define GPIO_PORTF_AMSEL_R      (*((volatile unsigned long *)0x40025528))
#define GPIO_PORTF_PCTL_R       (*((volatile unsigned long *)0x4002552C))
// Declaração da definição da Porta D
#define SYSCTL_RCGC2_R          (*((volatile unsigned long *)0x400FE108))
#define GPIO_PORTD_AMSEL_R      (*((volatile unsigned long *)0x40007528))
#define GPIO_PORTD_PCTL_R       (*((volatile unsigned long *)0x4000752C))
#define GPIO_PORTD_DIR_R        (*((volatile unsigned long *)0x40007400))
#define GPIO_PORTD_AFSEL_R      (*((volatile unsigned long *)0x40007420))
#define GPIO_PORTD_DEN_R        (*((volatile unsigned long *)0x4000751C))
#define GPIO_PORTD_DATA_R       (*((volatile unsigned long *)0x400073FC))	


//2. Variaveis Utilizadas no projeto
unsigned long In;  				// Input para PF4
unsigned long Out; 				// Visor para PD3
unsigned long OutGreen;	 	// Visor para LED green
unsigned long OutBlue; 		// Visor para LED blue
unsigned long OutRed; 		// Visor para LED red
unsigned long OutLed; 		// Visor para LED red
unsigned int flash_Green = 0;		// Looping para LED Verde
unsigned int flash_Red = 0;			// Looping para LED Vermelho
unsigned int flash_Red2 = 0;		// Looping para LED Vermelho frequência 2.5 Hz
unsigned int flash_All = 0;			// Looping para todos os LEDs
signed long Est;					// switch case para a maquina de estados



//Declaração de subrotinas para a Main
void PortF_Init(void);
void Delay(void);
void Delay_1(void);
//void EnableInterrupts(void); 


//3. Código
int main(void){    
while(1){

	TExaS_Init(SW_PIN_PF40,LED_PIN_PF321); //Iniciação da Placa
  PortF_Init(); //Chama a subrotina para iniciar a PF4 e PF2   
  //EnableInterrupts();
								
	switch(Est){
				
		// Estado 0 - Apagar os LEDs, aplicar nível lógico zero na saída PD3 
		case 0:
		GPIO_PORTF_DATA_R = 0x00;  // 0b0000_0000 LED off
		OutLed = GPIO_PORTF_DATA_R&0x0E; // Indicar nível no LED 
		GPIO_PORTD_DATA_R = 0x00;  // inicializa a PD3 como LOW 0x0000_0000
		Out = GPIO_PORTD_DATA_R&0x08;// indicar o que tem na porta PD3

// Estado 1 - LED  azul pisca em 2.5 Hz e PD3 desligado
		case 1:
				GPIO_PORTF_DATA_R = 0x04;  // 0b0000_0100 LED acende Azul
				OutBlue = GPIO_PORTF_DATA_R&0x04; //Saída para o Led Azul
				Delay_1(); //Espera para mudança de nivel
				GPIO_PORTF_DATA_R = 0x00;  // 0b0000_0000 LED off
				OutBlue = GPIO_PORTF_DATA_R&0x04; //Saída para o Led Azul
				Delay_1(); //Espera para mudança de nivel	
		
				GPIO_PORTD_DATA_R = 0x00;  //Inicializa a PD3 como LOW 0x0000_0000
				Out = GPIO_PORTD_DATA_R&0x08;//Indicar o que tem na porta PD3
		
//Estado 2 - Se chave SW2 pressionada, vá para case 4. Caso contrário, ir para case 3
		case 2:
			In = GPIO_PORTF_DATA_R&0x01; // read PF0 into In - SW2				
			if(In == 0x00) //Ler a Chave2, se 0 = precionada
			{              
				Est = Est + 1;     //vai para o estado 4
			} 
			else{ // Se 1 = não precionada, continue o codigo.
			
// Estado 3 - LED verde pisca 5 vezes
			case 3:
				while (flash_Green < 5) 
				{
					GPIO_PORTF_DATA_R = 0x08;  // 0b0000_1000 LED acende Verde
					OutGreen = GPIO_PORTF_DATA_R&0x08; // Indicar nível no LED green
					Delay();
					GPIO_PORTF_DATA_R = 0x00;  // 0b0000_0000 LED is black
					OutGreen = GPIO_PORTF_DATA_R&0x08; // Indicar nível no LED green
					Delay();	
					flash_Green = flash_Green + 1;
				}	
					Est = 1;
					break;	
			}

// Estado 4 - LED red piscar 2 vezes
		case 4:
			while (flash_Red < 2)
			{
				GPIO_PORTF_DATA_R = 0x02;  // 0b0000_0010 LED is red
				OutRed = GPIO_PORTF_DATA_R&0x02; // Indicar nível no LED red
				Delay();
				GPIO_PORTF_DATA_R = 0x00;  // 0b0000_0000 LED is black
				OutRed = GPIO_PORTF_DATA_R&0x02; // Indicar nível no LED red
				Delay();
				flash_Red = flash_Red + 1;
			}


//Estado 5 - Fazer LED red piscar na frequência 2.5 Hz => T = 0.4 (s 2 vezes)
		case 5:
		while (flash_Red2 < 2) 
		{
				GPIO_PORTF_DATA_R = 0x02;  // 0b0000_0010 LED is red 2.5 Hz
				Delay_1();
				GPIO_PORTF_DATA_R = 0x00;  // 0b0000_0000 LED is black
				Delay_1();
				flash_Red2 = flash_Red2 + 1;
			}
		
//Estado 6 -  Piscar os 3 LEDs em sequência, 4 vezes e PD3 high
		case 6:
		GPIO_PORTD_DATA_R = 0x08;  // 0x0000_0000 PD3 is high
		Out = GPIO_PORTD_DATA_R&0x08;// indicar o que tem na porta PD3
		Delay_1();
		
		while (flash_All < 4)
		{
			GPIO_PORTF_DATA_R = 0x02;  // 0b0000_0010 LED Vermelho
			Delay();
			GPIO_PORTF_DATA_R = 0x04;  // 0b0000_0100 LED Azul
			Delay();
			GPIO_PORTF_DATA_R = 0x08;  // 0b0000_1000 LED Verde
			Delay();
			flash_All = flash_All + 1;
		}
		
//Retorna para a Estado 0
		default: Est = 0;
		break;
		  }
    }
  }	

//4. SubRotinas
//Subrotina Padrão de iniciação
void PortF_Init(void){ volatile unsigned long delay;
// PF4 and PF0 are input SW1 and SW2 respectively
// PF3,PF2,PF1 are outputs to the LED
  SYSCTL_RCGC2_R |= 0x00000028;     // 1) 0x0010_1000 F and D clock
  delay = SYSCTL_RCGC2_R;           // delay   
  GPIO_PORTF_LOCK_R = 0x4C4F434B;   // 2) unlock PortF PF0, adress define by Datasheet  
  GPIO_PORTF_CR_R = 0x1F;           // allow changes to PF4-0       
  GPIO_PORTF_AMSEL_R = 0x00;        // 3) disable analog function
  GPIO_PORTF_PCTL_R = 0x00000000;   // 4) GPIO clear bit PCTL  
  GPIO_PORTF_DIR_R = 0x0E;          // 5) PF4,PF0 input, PF3,PF2,PF1 output   
  GPIO_PORTF_AFSEL_R = 0x00;        // 6) no alternate function
  GPIO_PORTF_PUR_R = 0x11;          // enable pullup resistors on PF4,PF0       
  GPIO_PORTF_DEN_R = 0x1F;          // 7) enable digital pins PF4-PF0   
	GPIO_PORTD_AMSEL_R &= ~0x0F;      // 3) disable analog functionality on PD3-0
  GPIO_PORTD_PCTL_R &= ~0x0000FFFF; // 4) GPIO
  GPIO_PORTD_DIR_R |= 0x0F;         // 5) make PD3-0 out
  GPIO_PORTD_AFSEL_R &= 0x0F;      // 6) regular port function
  GPIO_PORTD_DEN_R |= 0x0F;         // 7) enable digital I/O on PD3-0
}

//Subrotina Delay para 0.2 segundos
void Delay(void){unsigned long volatile time;
  time = 727240*200/91;  // 0.2sec
  while(time){
		time--;
  }
}

//SubRotina Delay para fequencia 2.5Hz
void Delay_1(void){unsigned long volatile time;
  time = 727240*200*2/97;  // 0.4sec = T => f = 2.5 Hz
  while(time){
		time--;
  }
}
