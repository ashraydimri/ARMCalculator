/*
   P0.4-P0.11 for Keypad at CNA
   P0.23-PO.28 for LCD at CND

   P0.27 == RESET  (O/P)
	 P0.28 == ENABLE  (O/P)
	 P0.23 TO P0.26 == DATA LINES (I/P)
	 P0.8 TO P0.11 == ROWS  (O/P)
	 P0.4 TO P0.7 == COLS  (I/P) */

//=== HEADER FILES ===

#include <lpc17xx.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

#define rs  0x08000000  //P0.27
#define en  0x10000000  //P0.28
#define dt  0x07800000  //P0.23 to P0.26 data lines

#define size 50
int top = -1;
char expr[size],infix_new[100],element[20];

void fact(int);
void oct(int);
void bin(int);
void hex(int);  

 void pushh(float symbol);
 float popp();
 void infix_to_postfix();
 void infix_to_newinfix();//added
 int priority(char symbol);
 int isEmpty();
 void eval_post();
 char myCharPointer[100];
 int len = 0,y,op,difmod,num1=0,value=0;
 float temps,result,please;
    float a,b;
    
 char postfix[100];
 float stack[100],key[100];
 char answer[100];
 int topp=-1,tope=-1;

void push(char t)
{
	expr[++top] = t;
}
void pushe(char t)
{
	element[++tope] = t;
}
char pope()
{
	return element[tope--];
}

char pop()
{
	return expr[top--];
}

//=== FUNCTION PROTOTYPE ===

void scan(void);
void lcd_init(void);
void wr_cn(void);
void wr_dn(void);
void lcd_data(void);
void clear_ports(void);
void delay(unsigned int);
void lcd_com(void);
void lcd_puts(char *buf1);
void clr_disp(void);
void EINT3_IRQHandler(void);
void EINT2_IRQHandler(void);

//=== VARIABLE DECLARATION  ===

int c,r,sum=0,mode;
unsigned long int d,flag,m,n,pos,first=1,var=0,count=0,i=0,temp,temp1,temp2,temp3;
char cnum[4],ans[5],num[4],check,msg[]={"aman"};
unsigned temp5;
unsigned int key_val[4][4]={'7','8','9','/','4','5','6','*','1','2','3','-','b','0','=','+'};
//unsigned int key_val2[4][4]={'^','r','l','s','c','t','(',')',NULL,NULL,'b',NULL,'f','h','z','o'};
unsigned int key_val2[4][4]={'s','c','t','(','^','r','l',')','n','e','f','i','b','y','z','o'};
unsigned char seven_dec[16] = {0x30,0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x41,0x42,0x43,0x44,0x45,0x46};



//=== LCD CONFIGURATION ===

void lcd_init()
{
  LPC_PINCON->PINSEL1 &=0xFC003FFF;        //P0.23 - P0.28 io lines
 	LPC_GPIO0->FIOMASK = 0xE07FFFFF;         //mask all except P0.23 - P0.28
	LPC_GPIO0->FIODIR |= dt;
	LPC_GPIO0->FIODIR |= rs;
	LPC_GPIO0->FIODIR |= en;
	clear_ports();
  delay(3200);

  temp2=(0x30<<19);
	wr_cn();
	delay(3000);

	temp2=(0x30<<19);
	wr_cn();
	delay(3000);

  temp2=(0x30<<19);
	wr_cn();
	delay(3000);

	temp2=(0x20<<19);
	wr_cn();
	delay(1000);

	temp1=(0x28);
	lcd_com();
	delay(3000);

	temp1=(0x0c);
	lcd_com();
	delay(800);

	temp1=(0x06);
	lcd_com();
	delay(800);

	temp1=(0x01);
	lcd_com();
	delay(1000);

	temp1=(0x80);
	lcd_com();
	delay(800);
	return;
}

void lcd_data()
{
	LPC_GPIO0->FIOMASK = 0xE07FFFFF;
	temp2=temp1&0Xf0;
	temp2=temp2<<19;
	wr_dn();
	temp2=temp1&0x0f;
	temp2=temp2<<23;
	wr_dn();
	delay(1000);
	return;
}

//=== FOR WRITING CMDS ===

void wr_cn()
{
	LPC_GPIO0->FIOMASK = 0xE07FFFFF;
	clear_ports();
	LPC_GPIO0->FIOPIN=temp2;
	LPC_GPIO0->FIOCLR=rs;
	LPC_GPIO0->FIOSET=en;
	delay(25);
	LPC_GPIO0->FIOCLR=en;
	return;
}

//=== FOR WRITING DATA ===

void wr_dn()
{
	LPC_GPIO0->FIOMASK = 0xE07FFFFF;
	clear_ports();
	LPC_GPIO0->FIOPIN=temp2;
	LPC_GPIO0->FIOSET=rs;
	LPC_GPIO0->FIOSET=en;
	delay(25);
	LPC_GPIO0->FIOCLR=en;
	return;
}

void lcd_com()
{
	LPC_GPIO0->FIOMASK = 0xE07FFFFF;
	temp2=temp1&0Xf0;
	temp2=temp2<<19;
	wr_cn();
  temp2=temp1&0x0f;
	temp2=temp2<<23;
	wr_cn();
	delay(1000);
	return;
}

void clear_ports(void)
{
	LPC_GPIO0->FIOMASK = 0xE07FFFFF;
  LPC_GPIO0->FIOCLR = dt;
  LPC_GPIO0->FIOCLR = rs;
  LPC_GPIO0->FIOCLR = en;
	return;
}

void delay(unsigned int f)
{
  unsigned int r;
  for(r=0;r<f;r++);
  return;
}

void clr_disp(void)
{
	LPC_GPIO0->FIOMASK = 0xE07FFFFF;
  temp1 = 0x01;
  lcd_com();
  delay(300);
  return;
}

void lcd_puts(char *buf1)
{
	clr_disp();                  //clear previous display
  i=0;
  while(buf1[i]!='\0')
  {
		temp1 = buf1[i];
    lcd_data();
		i++;
    if(i==16)
	  {
      temp1 = 0xc0;
		  lcd_com();
		}
  }
  for(m=0;m<35000;m++);
  return;
}

//=== INTERRUPT HANDLER ===

void  EINT3_IRQHandler(void)
{
	for(i=0;i<1000;i++);
	LPC_GPIO0->FIOMASK = 0XFFFFF0FF;		 //masking P0.8 to P0.11
	LPC_GPIO0->FIOCLR = 0xF00; 					 //clearing rows
	for(r=0;r<4;r++)
	{
		if(r==0)
			temp = 0x00000100;		//for row1
		else if(r==1)
  		temp = 0x00000200;		//for row2
		else if(r==2)
			temp = 0x00000400;		//for row3
		else if(r==3)
			temp = 0x00000800;		//for row4
		LPC_GPIO0->FIOMASK = 0XFFFFF0FF;
		LPC_GPIO0->FIOPIN = temp;
		flag = 0;
    scan();
    temp3 &= 0x000000F0;
		if(flag==1)							//when column has been detected
		{
			/*if(key_val[r][c]=='k')    //accept key
			{
				var=1;
				pos=0;
				while(top>=0)
				{
					expr[top]='\0';
					pop();

				}
				//lcd_puts(expr);
			  if(!first)
					sum+=atoi(num);
				else
				{
				  first=0;
	        n=atoi(num);
				}
        for (i=0;i<4;i++)
	         num[i]=' ';
        LPC_GPIO0->FIOMASK = 0xFFFFF0FF;        //setting rows to 1
				LPC_GPIO0->FIOPIN = 0xF00;
				LPC_GPIOINT->IO0IntClr=0xF0;						//clearing interrupt
				for(m=0;m<35000;m++);
				clr_disp();															//clearing display
				return;
			}*/
			if(mode==1)
				check=key_val[r][c];
			else if(mode==0)
				check=key_val2[r][c];
			if(check=='=')
			{
				if(difmod>=1)
				{
					switch(op)
					{
						//case 1 : hex(value);break;
						case 2 : oct(value);break;
						case 3 : bin(num1);break;
						case 4 : fact(num1);break;
					}
				}
				else
				//lcd_puts(expr);
				{infix_to_newinfix();//added
				
				infix_to_postfix();
				eval_post();
				for(i=0;i<500000;i++);
				while(top!=-1)
				{
					expr[top]='\0';
					pop();
				}
			}
				for(i=0;i<500000;i++);
				clr_disp();
				temp1 = 0x80;			//1st message on LCD 1st line
				lcd_com();
				delay(800);
			}



      else if (check=='b')						//cancel key
			{
				expr[top]='\0';
				pop();
				pos--;
				num[pos]=' ';
			  clr_disp();
        lcd_puts(expr);
				break;
			}
			else if(check=='f' || check=='o' || check=='z')
			{
				difmod++; 
				pushe(check);
				lcd_puts(element);
				switch(check)
				{
					//case 'h' : op=1;break;
					case 'o' : op=2;break;
					case 'z' : op=3;break;
					case 'f' : op=4;break;
				}
			}		
		  else
		  {
					
				if(difmod>=1)
				{
					
					for(i=0;i<10;i++)
					{
						if(check==seven_dec[i])
						break;
					}
					num1=i;
					//if(difmod==1)
					//	value += num1*10;
					//else if(difmod==2)
					//	value += num1;
					pushe(check);
					lcd_puts(element);
					//difmod++;
				}
				else if(mode==1)
				{
		  	num[pos]=key_val[r][c];
				temp5 = key_val[r][c];
				push(temp5);
				lcd_puts(expr);
				pos++;
				break;
				}
					else if(mode==0)
					{ num[pos]=key_val2[r][c];
						temp5 = key_val2[r][c];
						push(temp5);
						lcd_puts(expr);
						pos++;
						break;

					}
		  }
    }
	}
	LPC_GPIO0->FIOMASK = 0xFFFFF0FF;       //setting rows to 1
	LPC_GPIO0->FIOPIN = 0xF00;
	LPC_GPIOINT->IO0IntClr=0xF0;					//clearing interrupt
}

//=== SCAN FUNCTION ===

void scan(void)
{
	LPC_GPIO0->FIOMASK = 0XFFFFFF0F;
	temp3 = LPC_GPIO0->FIOPIN;
	temp3 &= 0xF0;
  if(temp3 != 0x00000000)
	{
	  flag = 1;
		if (temp3 ==0x00000010)
			c=0;												 //for column1
		else if (temp3==0x00000020)
			c=1;												 //for column2
		else if (temp3==0x00000040)
			c=2;												 //for column3
		else if (temp3==0x00000080)
			c=3;												 //for column4
	}
}

void EINT2_IRQHandler()
{
	LPC_SC->EXTINT = (1<<2);
	LPC_GPIO0->FIOMASK = 0xFFFE7FFF;
    LPC_LPC_GPIO0->FIOCLR = 0xFFFFFFFF;
	if(mode==0)
	{
		mode = 1;
		LPC_GPIO0->FIOPIN = (1<<15)|(0<<16);
	}
	else
	{
		mode = 0;
		LPC_GPIO0->FIOPIN = (1<<16)|(0<<15);
	}
}



//=== MAIN FUNCTION ===

int main()
{
	SystemInit();
	SystemCoreClockUpdate();
	lcd_init();																 //initialising LCD
	LPC_GPIO0->FIOMASK = 0XFFFE700F;
	LPC_PINCON->PINSEL0&=~(0xFF<<4);           //configuring rows and columns as GPIO
	LPC_GPIO0->FIODIR |= 0xF<<8;               //configuring rows as output
	LPC_GPIO0->FIOPIN |= 0xF<<8;               //setting all row to 1
  lcd_puts("Enter Expression");	                     //prompting user to enter total number of nos.
	NVIC_EnableIRQ(EINT3_IRQn);
	LPC_GPIO0->FIODIR |= (1<15)|(1<<16);
	
	LPC_PINCON->PINSEL4 = (1<<24); // EINT2 on P2.12
	LPC_SC->EXTINT = (1<<2);       // clear pending requests
	LPC_SC->EXTMODE = (1<<2);	   // edge triggered
	LPC_SC->EXTPOLAR = (1<<2); 	   // +ve edge
  NVIC_EnableIRQ(EINT2_IRQn);
	LPC_GPIOINT->IO0IntEnR = (0xF<<4);         //enabling columns as gpio interrupt
  while(1)
  {
	  if(var==1)
	  {
	    first=0;
		  break;
    }
  }
  var=0;
  clr_disp();							//clearing the display
  delay(900);
  while(count<n) 					//n = total number of BCD numbers
  {
    while(1)
    {
      if(var==1) 					//var is set when ACCEPT is pressed
        break;
    }
    count++;
    var=0;
	  clr_disp();
	  temp1=(0x80);
    lcd_com();
    delay(800);
  }
  //lcd_puts("SUM:");
  //d=sprintf(ans,"%d%",sum);   //get string value of sum
  //lcd_puts(ans);
}




void infix_to_postfix()
{
		int i,p=0;
    char next;
    char symbol;
    for(i=0; i<strlen(infix_new); i++)
    {
        symbol=infix_new[i];
        //if(!white_space(symbol))
        //{
            switch(symbol)
            {
            case '(':
                pushh(symbol);
                break;
            case ')':
                while((next=popp())!='(')
                    postfix[p++] = next;
                break;
            case '+':
            case '-':
            case '*':
            case '/':
            case '^':
            case 'r':
			case 'l':
            case 's':
            case 'c':
            case 't':
            case 'n':
            case 'e':
            case 'i':
            case 'y':
            
                while( !isEmpty( ) &&  priority(stack[topp])>= priority(symbol) )
                    postfix[p++]=popp();
                    //if(symbol!='\n')
                pushh(symbol);
                break;
            default: /*if an operand comes*/
                postfix[p++]=symbol;
            }
        //}
    }
    while(!isEmpty( )){
        postfix[p++]=popp();
    }

    postfix[p]='\0'; /*End postfix with'\0' to make it a string*/
		//lcd_puts(postfix);
}

/*This function returns the priority of the operator*/
int priority(char symbol)
{
    switch(symbol)
    {
    case '(':
        return 0;
    case '+':
    case '-':
        return 1;
    case '*':
    case '/':
    case '%':
        return 2;
    case '^':
    case 'r':
        //return 3;
    
    case 's':
	case 'l':
    case 'c':
    case 't':
    case 'n':
    case 'i':
    case 'e':
    case 'y':
        return 4;
    default :
        return 0;
    }
}

void pushh(float symbol)
{
      /*if (! isEmpty() && stack[top] >= 0 && stack[top] <= 9) {
      stack[top] *= 10;
      stack[top] += symbol;
    }
    else {*/
      stack[++topp]=symbol;
    //}
}

float popp()
{
    return (stack[topp--]);
}

int isEmpty()
{
    if(topp==-1)
        return 1;
    else
        return 0;
}

void eval_post()
{
	char chec_log[40];
    float temp,result;
    float a,b;
    int i;
    for(i=0; i<strlen(postfix); i++)
    {

        if(postfix[i]<='9' && postfix[i]>='0'){
            float please = key[postfix[i]-'0'];
            pushh(please);
        }
        else
        {
            if(infix_new[0]=='l')
            {
                a=popp();
                infix_new[0]='a';
                //printf("\n%f\n",a);
                //printf("\n%d\n",1);
                temp = log(a)/log(2);
							  //sprintf(chec_log,"%f",temp);
								//lcd_puts(chec_log);
                //printf("\n%f\n",temp);
                pushh(temp);
            }
            else if(infix_new[0]=='i')
            {
                a=popp();
                infix_new[0]='a';
                //printf("\n%f\n",a);
                //printf("\n%d\n",1);
                temp = log(a)/log(10);
                              //sprintf(chec_log,"%f",temp);
                                //lcd_puts(chec_log);
                //printf("\n%f\n",temp);
                pushh(temp);
            }
            else if(infix_new[0]=='n')
            {
                a=popp();
                infix_new[0]='a';
                //printf("\n%f\n",a);
                //printf("\n%d\n",1);
                temp = log(a);
                              //sprintf(chec_log,"%f",temp);
                                //lcd_puts(chec_log);
                //printf("\n%f\n",temp);
                pushh(temp);
            }
            else if(infix_new[0]=='s')
            {

                a=popp();
               infix_new[0]='a';
                temp = sin((a*3.141)/180);
                pushh(temp);
            }
            else if(infix_new[0]=='c')
            {
                //printf("nikal");
                a=popp();
                infix_new[0]='a';
                temp = cos((a*3.141)/180);
                pushh(temp);
            }
            else if(infix_new[0]=='t')
            {
                //printf("nikal");
                a=popp();
                infix_new[0]='a';
                temp = tan((a*3.141)/180);
                pushh(temp);
            }
            else if(infix_new[0]=='e')
            {
                //printf("nikal");
                a=popp();
                infix_new[0]='a';
               temp=pow(2.71828,a);
                
                pushh(temp);
            }
        else
        {
            a=popp();
            b=popp();
            switch(postfix[i])
            {
            case '+':
                temp=b+a;
                pushh(temp);
                break;
            case '-':
                temp=b-a;
                pushh(temp);
                break;
            case '*':
                temp=b*a;
                pushh(temp);
                break;
            case '/':
                temp=b/a;
                //printf("%f",temp);
                pushh(temp);
                break;
            case '^':
                temp=pow(b,a);
                pushh(temp);
                break;
            case 'y':
                temp=log(b)/log(a);
                pushh(temp);
                break;
            case 'r':
                //r2float c = 1/a;
                temp=pow(b,1/a);
                pushh(temp);
                break;
            case 'l':
                temp = log(a)/log(2);
                pushh(b);
                pushh(temp);
                break;
            case 's':
                //printf("nikal");
                //float convert = (a*3.141)/180;
                temp = sin((a*3.141)/180);
                pushh(b);
                pushh(temp);
                break;
            case 'c':
               // printf("nikal");
                //float convert = (a*3.141)/180;
                temp = cos((a*3.141)/180);
                pushh(b);
                pushh(temp);
                break;
            case 't':
                //printf("nikal");
                //float convert = (a*3.141)/180;
                temp = tan((a*3.141)/180);
                pushh(b);
                pushh(temp);
                break;
            case 'i':
                temp = log(a)/log(10);
                pushh(b);
                pushh(temp);
                break;
            case 'n':
                temp = log(a);
                pushh(b);
                pushh(temp);
                break;
            case 'e':
                //printf("nikal");
                //float convert = (a*3.141)/180;
                temp = pow(2.71828,a);
                pushh(b);
                pushh(temp);
                break;
            }
        }
    }
}
    result=popp();

    sprintf(myCharPointer, "%f", result);
    //printf("%s",myCharPointer);
    len = strlen(myCharPointer);
    len-=4;
    y = 0;
    for(y=0;y<len;y++)
       answer[y] = myCharPointer[y];
       answer[y]='\0';

    lcd_puts(answer);
       //printf("%s",answer);
    //return result;
}






void infix_to_newinfix()
{
    int count = 1;
    int i = 0;
    int val = 0;
    int j = 0;
    int k = 0;
    int co = 0;
    int reverse=0;
    int flag = 0;
    for(i=0;i<=strlen(expr);i++)
    {
        //printf("%s\n", infix_new);

        if(expr[i]>=48 && expr[i]<=57)
        {
            int tempVal = expr[i]-'0';
            if(tempVal==0)
            {
                val = val*count;
                flag = 1;
            }
            else
                val += tempVal * count;
            count = count*10;
            //printf("%d\n",val );
        }
        else if(expr[i] == '+' || expr[i] == '-' || expr[i] == '*' || expr[i] == 'y' || expr[i] == '/' || expr[i] == '^' || expr[i] == 'r' || expr[i] == '\0')
        {
            count = 1;
            if(expr[i-1] != ')'){
            if(flag!=1)
            {
                    while (val != 0)
                    {
                        reverse = reverse * 10;
                        reverse = reverse + val%10;
                        val = val/10;
                    }
                    key[j] = reverse;
                    infix_new[k++] = j+'0';
                    infix_new[k++] = expr[i];
            }
            //printf("%d\t",reverse);
           else{
                    key[j] = val;
                    infix_new[k++] = j+'0';
                     infix_new[k++] = expr[i];
            }
            j++;
            val = 0;
            reverse = 0;
            flag = 0;
        }
        else{
            infix_new[k++] = expr[i];
            j++;
            val = 0;
            reverse = 0;
            flag = 0;
        }
        }
        else if(expr[i]=='(')
        {
            infix_new[k++] = expr[i];
        }
        else if(expr[i] == ')')
        {
            key[j] = val;
            infix_new[k++] = j+'0';
            infix_new[k++] = expr[i];
            j++;
            val = 0;
            reverse = 0;
            flag = 0;

        }
        else
        {

        	infix_new[k++] = expr[i];
        	i++;
        	count=1;
        	while(expr[i]>=48 && expr[i]<=57)
        	{
        		int tempVal = expr[i]-'0';
        		if(tempVal==0)
                {
                    val = val*count;
                    flag = 1;
                }
                else
                    val += tempVal * count;
        		count = count*10;
        		i++;
        	}
        	if(flag!=1)
            {
                while(val != 0)
                {
                    reverse = reverse * 10;
                    reverse = reverse + val%10;
                    val = val/10;
                }
                key[j] = reverse;
                infix_new[k++] = j+'0';
                infix_new[k++] = expr[i];
            }
            else
            {
                key[j] = val;
                infix_new[k++] = j+'0';
                infix_new[k++] = expr[i];
            }
        	j++;
        	reverse = 0;
        	val = 0;
        	flag = 0;
        }
    }
}



void bin(int n)
{
    int binaryNumber = 0;
    int remainder, i = 1;
    char bin[20];
    while (n!=0)
    {
        remainder = n%2;
       // printf("Step %d: %d/2, Remainder = %d, Quotient = %d\n", step++, n, remainder, n/2);
        n /= 2;
        binaryNumber += remainder*i;
        i *= 10;
    }
	sprintf(bin,"%d",binaryNumber);
	lcd_puts(bin);
}


void oct(int decimalNumber)
{
    int octalNumber = 0, i = 1;
    char octa[20];
    while (decimalNumber != 0)
    {
        octalNumber += (decimalNumber % 8) * i;
        decimalNumber /= 8;
        i *= 10;
    }
    sprintf(octa,"%d",octalNumber);
    lcd_puts(octa);
}



void fact(int n)
{
	int fa=1;
	char fac[20];
	for(i=1; i<=n; ++i)
     		  fa *= i;              
        
	sprintf(fac,"%d",fa);
	lcd_puts(fac);
}