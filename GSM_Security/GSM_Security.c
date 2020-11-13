#include "main.h"

#define Blink_TimeOut 150

int i = 0;
int LampGood = 2;
int SecurStatus = 0;
int CallState = 0;


int main(void)
{
	
	
	
	Timer_Init();
	USART_ini();
	PortInit();
	sei();
	while (1)
	{
		
			if((PIND&0b00001000)) // "A" ����� �� ������� ������� "�" �� ������
		
			{
				SecurityON();
			}
		

			if((PIND&0b00000100)) // "B" �������� ������� ������� "�" �� ������
		
			{
				SecurityOFF();
			}
		
		
			if(SecurStatus == 0)  //��� �������� ������
			{
				Sirena_OFF();
				LampGood = 2;
			}
		
			if(SecurStatus == 1) //��� �������� ������.
			{
				//���� �������� 1 ���/���
				//� ��� ����������� SR 501 �� ���� PD4 ��������� �������	 �-��.
				if(PIND&0b00010000 || !(PINB&0b00000001)) //���� ��������� ������ ���� ��� ������ ������ ��� �������� ������
				{
				
				       
					   
					if(CallState == 0)
					{
						Sirena_ON();
					    LampGood = 1;
						
						cli();
						usart_putline("ATD+380972265447;\r");
						sei();
						CallState = 1;

					}
				}
			}
	}
}


void Sirena_ON(void)
{
	PORTC |= (1<<2);
}
void Sirena_OFF(void)
{
	PORTC &= ~(1<<2);
}
void Buzzer_ON(void)
{
	PORTC &= ~(1<<4);
}
void Buzzer_OFF(void)
{
	PORTC |=(1<<4);
}
void BuzzerVoice(int count)
{
	int i;
	for (i=0;i<count;i++)
	{
		Buzzer_ON();
		_delay_ms(100);
		Buzzer_OFF();
		_delay_ms(100);
	}
}
void StatusLed_ON(void)
{
	PORTC &= ~ (1<<5);
}
void StatusLed_OFF(void)
{
	PORTC |= (1<<5);
}
void StatusLed_BLINK_two(void)
{
	StatusLed_ON();
	_delay_ms(Blink_TimeOut);
	StatusLed_OFF();
	_delay_ms(Blink_TimeOut);
	
}
void StatusLed_BLINK_one(void)
{
	StatusLed_ON();
	_delay_ms(Blink_TimeOut);
	StatusLed_OFF();
	_delay_ms(Blink_TimeOut);
}
void Start(void)
{
	for(i=0; i<30;i++)
	{
		StatusLed_ON();
		Buzzer_ON();
		_delay_ms(150);
		StatusLed_OFF();
		Buzzer_OFF();
		_delay_ms(260);
		if(i>30) break;
	}
}
void SignalLampOFF(void)
{
	PORTC &= ~(1<<3);
}
void SignalLampBlink(void)
{
	PORTC = PORTC ^ (1<<PC3);
}
void StatusLedBlink(void)
{
	PORTC ^= (1<<PC5);
}
void Timer_Init(void)
{
	TCCR1B |= (1<<WGM12); // ������������� ����� ��� (����� �� ����������)
	TIMSK |= (1<<OCIE1A); //������������� ��� ��������� ��������� 1��� �������� �� ���������� � OCR1A(H � L)
	
	OCR1AH = 0x7A; //����������� �� ������������ ��������������� ��� �� �������
	OCR1AL = 0x11;
	
	TCCR1B |= (1<<CS12);//������ �� 256.
}
void PortInit(void)
{
	
	DDRC |= (1<<2)|(1<<3)|(1<<4)|(1<<5);          //������
	PORTC |= (0<<2)|(0<<3)|(1<<4)|(1<<5);
	DDRD &= ~(1<<2)|(1<<3)|(1<<4);                //�����
	PORTD |= (0<<2)|(0<<3)|(0<<4);
	DDRB &= ~(1<<0);                              //���� ������� �������� ������.
	PORTB |= (1<<0);                              // ϳ������ +5� + �� ���� � �������� �������
}
void SecurityON(void)
{
	BuzzerVoice(1);
	_delay_ms(5000);
	SecurStatus = 1;
	LampGood = 0; // ���� ��������
	BuzzerVoice(2);
	StatusLed_BLINK_two();
}
void SecurityOFF(void)
{
	SecurStatus = 0;
	CallState = 0;
	LampGood = 2; // �� ���� �����
	StatusLed_OFF();
	BuzzerVoice(1);
	StatusLed_BLINK_one();
}
ISR (TIMER1_COMPA_vect)
{
	if(LampGood == 1)
	SignalLampBlink();
	else if(LampGood == 0)
	StatusLedBlink();
	else
	asm("nop");
}