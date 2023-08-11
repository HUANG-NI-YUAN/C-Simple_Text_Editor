#include "graphics.h"
#include "extgraph.h"
#include "genlib.h"
#include "simpio.h"
#include "conio.h"
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>

#include <windows.h>
#include <olectl.h>
#include <mmsystem.h>
#include <wingdi.h>
#include <ole2.h>
#include <ocidl.h>
#include <winuser.h>

#include "imgui.h"

#define DEMO_MENU
#define DEMO_BUTTON
#define DEMO_ROTATE
#define DEMO_EDITBOX


// ȫ�ֱ���
static double winwidth, winheight;   // ���ڳߴ�
static float  rot_angle = 0;         // ��������ת�Ƕ�
static int    enable_rotation = 1;   // ������ת
static int    show_more_buttons = 0; // ��ʾ���ఴť
static char word[80][80]={0};        // �ַ������� 
static int Is_open = 0; //������Ǵ˿����޴����е��ļ���1����� 
static char path[80] = {0};   //��ַ·��

// ����������provided in libgraphics
void DisplayClear(void); 
// ��ʱ������������provided in libgraphics
void startTimer(int id,int timeinterval);

// �û�����ʾ����
void display(void); 

// �û����ַ��¼���Ӧ����
void CharEventProcess(char ch)
{
	uiGetChar(ch); // GUI�ַ�����
	display(); //ˢ����ʾ
}

// �û��ļ����¼���Ӧ����
void KeyboardEventProcess(int key, int event)
{
	uiGetKeyboard(key,event); // GUI��ȡ����
	display(); // ˢ����ʾ
}

// �û�������¼���Ӧ����
void MouseEventProcess(int x, int y, int button, int event)
{
	uiGetMouse(x,y,button,event); //GUI��ȡ���
	display(); // ˢ����ʾ
}

// ��ת��ʱ��
#define MY_ROTATE_TIMER  1

// �û��ļ�ʱ��ʱ����Ӧ����
void TimerEventProcess(int timerID)
{
	if( timerID==MY_ROTATE_TIMER && enable_rotation ) 
	{
		rot_angle += 10; // ȫ�ֱ���
		display(); // ˢ����ʾ
	}
}
//�������word��ֵ��debug�ã�
void insect_word()
{
	int i,j;
	for(i=0;i<80;i++)
        {
                for(j=0;j<80;j++)
                {
                    printf("%c",word[i][j]);    
                }
				printf("\n");     
        }
} 

//��text�ļ��е��������ж���word[80][80],��������
void read_file(FILE *fp)
{
	int i, j;
	for(i=0;i<80;i++)
        {
                for(j=0;j<80;j++)
                {
                        fscanf(fp,"%c",&word[i][j]);
                        if(word[i][j]=='\0'||word[i][j]=='\n') break;
                        
                }
                fscanf(fp,"\n");
        }
} 

//word��ʼ��
void init_word()
{
	int i, j;
	for(i=0;i<80;i++){
		for(j=0;j<80;j++){
			word[i][j] = '\0';
		}
	}
} 

//��word�е�����������ļ�
void print_word(char *pFileName)
{
	FILE *fp = NULL;
	fp = fopen(pFileName, "w");
	int i, j;
	if(fp==NULL){
		exit(0);
	}else{
		for(i=0;i<80;i++){
			for(j=0;j<80;j++){
				if(word[i][j]=='\0'||word[i][j]=='\n') break;
				fprintf(fp,"%c",word[i][j]);
			}
			fprintf(fp,"\n");
		}
	}
	fclose(fp);
} 

// �û����������
// ����ʼ��ִ��һ��
void Main() 
{
	// ��ʼ�����ں�ͼ��ϵͳ
	SetWindowTitle("Graphics User Interface Demo");
	//SetWindowSize(10, 10); // ��λ - Ӣ��
	//SetWindowSize(15, 10);
	//SetWindowSize(10, 20);  // �����Ļ�ߴ粻�����򰴱�����С
    InitGraphics();

	// ��ô��ڳߴ�
    winwidth = GetWindowWidth();
    winheight = GetWindowHeight();

	// ע��ʱ����Ӧ����
	registerCharEvent(CharEventProcess); // �ַ�
	registerKeyboardEvent(KeyboardEventProcess);// ����
	registerMouseEvent(MouseEventProcess);      // ���
	registerTimerEvent(TimerEventProcess);      // ��ʱ��

	// ������ʱ��
	startTimer(MY_ROTATE_TIMER, 50);            

	// �򿪿���̨���������������Ϣ�����ڵ���
	// InitConsole(); 

}

#if defined(DEMO_MENU)
// �˵���ʾ����
void drawMenu()
{ 
	static char * menuListFile[] = {"�ļ�",  
		"��  | Ctrl-O", // ��ݼ��������[Ctrl-X]��ʽ�������ַ����Ľ�β
		"�½�  | Ctrl-N",
		"����  | Ctrl-S",
		"���Ϊ",
		"�˳�  | Ctrl-E"};
	static char * menuListFormular[] = {"�༭",
		"����  | Ctrl-C",
		"ճ��  | Ctrl-V",
		"����  | Ctrl-F",
		"�滻  | Ctrl-H"};
	static char * menuListFormat[] = {"��ʽ",
		"����",
		"��ɫ"};
	static char * menuListCheck[] = {"�鿴",
		"�Ŵ�",
		"��С"};
	static char * menuListHelp[] = {"����",
		"ʹ��  | Ctrl-M",
		"���ڱ����"};
	static char * selectedLabel = NULL;//���ʹ�� 
	static char * wordsum = NULL;//���� 

	double fH = GetFontHeight();
	double x = 0; //fH/8;
	double y = winheight;
	double h = fH*1.5; // �ؼ��߶�
	double w = TextStringWidth(menuListHelp[0])*2; // �ؼ����
	double wlist = TextStringWidth(menuListFormular[3])*1.2;
	double xindent = winheight/20; // ����
	int    selection,k;
	FILE *fp = NULL;  //�ļ�ָ�� 
	int Is_recover; 
	
	// menu bar
	drawMenuBar(0,y-h,winwidth,h);
	// File �˵�
	selection = menuList(GenUIID(0), x, y-h, w, wlist, h, menuListFile, sizeof(menuListFile)/sizeof(menuListFile[0]));
	if( selection>0 ) selectedLabel = menuListFile[selection];
	switch(selection)
	{
		case 1:  //"��"���� 
		InitConsole();
		printf("������Ҫ���ı�����ȫ·��\n");
		while(1){
			gets(path);
			if((fp = fopen(path,"r"))!=NULL){
				printf("���ѳɹ���%s\n",path);
				Is_open = 1;
				break;
			}
			printf("·��������������û��Ȩ����ô����������һ��^_^��:\n");
		} 
		read_file(fp);       
        fclose(fp);
//		insect_word();
		FreeConsole();
        Is_open = 1;             

		break;
		
		case 2:  //�½����� 
		init_word();
		Is_open = 0;
		for(k=0;k<80;k++){ //��ʼ��path 
			path[k] = '\0';
		} 
		fp = NULL;
		display();
		break;
		
		case 3: //������� 
		if(Is_open == 1){
			print_word(path);
			InitConsole();
			printf("�����ļ��Ѿ��ɹ�����\n");
			FreeConsole();
		}else{
			InitConsole();
			printf("�����봢���ļ���������ַ��\n");
			gets(path);
			while((fp = fopen(path,"r"))!=NULL){
				printf("���ļ��Ѵ���,��ѡ���Ƿ񸲸ǣ�\n1)����\n2)������\n");
				Is_recover = GetInteger();
				if(Is_recover == 1){
					break;
				}else{
					printf("���������봢���ļ���������ַ��\n"); 
					gets(path);
				}	  
			}
			if(fp ==NULL|| Is_recover == 1){
				print_word(path);
				fclose(fp);
				printf("�����ļ��ѳɹ����棡"); 
				Is_open = 1;
			} 
			FreeConsole();
		}
		break;  
		case 4:  //���Ϊ���� 
		InitConsole();
		printf("�����봢���ļ���������ַ��\n");
		gets(path);
		while((fp = fopen(path,"r"))!=NULL){
			printf("���ļ��Ѵ���,��ѡ���Ƿ񸲸ǣ�\n1)����\n2)������\n");
			Is_recover = GetInteger();
			if(Is_recover == 1){
				break;
			}else{
				printf("���������봢���ļ���������ַ��\n"); 
				gets(path);
			}	  
		}
		if(fp ==NULL|| Is_recover == 1){
			print_word(path);
			fclose(fp);
			printf("�����ļ��ѳɹ����Ϊ��"); 
			Is_open = 1;
		} 
		FreeConsole();
		break;
		
		case 5:
		exit(-1);break;//�˳����� 
		default:break;
	}
	
	// Formular �˵�
	static char copyword[800];
	int i;
	void copy(int x1,int y1,int x2,int y2,char word[][80])//�������躯�� 
	{
		strcpy(copyword,"");
		int number_copy=0;
		if(x2>x1)
		{
			for(i=y1-1; i<80; i++)
			{
				if(word[x1-1][i]=='\0')
				{
					break;
				}
				copyword[number_copy]=word[x1-1][i];
				number_copy++;
			}
			copyword[number_copy]='\0';
			for(i=x1; i<x2-1; i++)
			{
				if(word[i][0]=='\0')
				{
					continue;
				}
				strcat(copyword,word[i]);
			}
				char flag[80];
				for(i=0; i<y2; i++)
				{
					flag[i]=word[x2-1][i];
				}
				flag[y2]='\0';
				strcat(copyword,flag);
		}
		else
		{
			for(i=y1-1; i<y2; i++)
			{
				copyword[number_copy]=word[x1-1][i];
				number_copy++;
			}
		}
	}
	
	void paste(int x0,int y0, char copyword[])//ճ�����躯�� 
	{
		char flag1[80],flag2[80];
		int i;
		for(i=0; i<y0; i++)
		{
			flag1[i]=word[x0-1][i];
		}
		flag1[y0]='\0';
		for(i=0; i<80; i++)
		{
			if(word[x0-1][i+y0]=='\0')
			{
				flag2[i]=word[x0-1][i+y0];
				break;
			}
			flag2[i]=word[x0-1][i+y0];
		}
		strcpy(word[x0-1],"");
		strcpy(word[x0-1],flag1);
		strcat(word[x0-1],copyword);
		strcat(word[x0-1],flag2);
	}
	
	void replace(char searchword[],char replaceword[])//�滻���躯�� 
	{
		double indent=GetFontAscent()/2;
		int flag=0, len=strlen(searchword), height=GetFontHeight, i, j, k;
		char compareword[80],flag1[80],flag2[80];
		while(word[flag][0]!='\0')
		{
			flag++;
		}
		for(i=0;i<flag;i++)
		{
			for(j=0;word[i][j]!='\0';j++)
			{
				for(k=j;k<j+len;k++)
				{
					compareword[k-j]=word[i][k];
				}
				compareword[len]='\0';
				if(strcmp(compareword,searchword)==0)
				{
					if(j!=0)
					{
						for(k=0;k<j;k++)
						{
							flag1[k]=word[i][k];
						}
						flag1[j]='\0';
					}
					for(k=j+len;k<80;k++)
					{
						if(word[i][k]=='\0')
						{
							flag2[k-j-len]=word[i][k];
							break;
						}
						flag2[k-j-len]=word[i][k];
					}
					strcpy(word[i],"");
					if(j!=0)
					{
						strcpy(word[i],flag1);
					}
					strcat(word[i],replaceword);
					strcat(word[i],flag2);
				}
			}
		}
	}
	
	selection = menuList(GenUIID(0),x+w,  y-h, w, wlist,h, menuListFormular,sizeof(menuListFormular)/sizeof(menuListFormular[0]));
	if( selection>0 ) selectedLabel = menuListFormular[selection];
	if( selection==1 )//���� 
	{
		InitConsole();
		int x1,y1,x2,y2;
		printf("�����븴����ʼ��ͽ�����:\t��ʽ��(��,��)(��,��)\tע��ȫ�����ǰ������\n");//���������յ� 
		scanf("(%d,%d)(%d,%d)",&x1,&y1,&x2,&y2);
		copy(x1,y1,x2,y2,word);
		printf("�Ѹ������ݣ�%s",copyword);
		FreeConsole();
	}
	if( selection==2 )//ճ�� 
	{
		InitConsole();
		int x0,y0;
		printf("�Ѿ����Ƶ������ǣ�\n%s\n",copyword);
		printf("������ճ����ʼ�㣺\t��ʽ��(��,��)\tע��ȫ�����ǰ������\n");//ճ����� 
		scanf("(%d,%d)",&x0,&y0);
		paste(x0,y0,copyword);
		FreeConsole();
	}
	if( selection==3 )//���� 
	{
		InitConsole();
		char searchword[80],replaceword[80];
		printf("��������Ҫ���ҵĵ��ʣ�\n");
		scanf("%s",searchword);
		strcpy(replaceword,"");
		strcpy(replaceword,"[");
		strcat(replaceword,searchword);
		strcat(replaceword,"]");
		char bug[2]="[";
		replace(searchword,bug);
		replace(bug,replaceword);
		printf("succeed!");
		FreeConsole();
	}
	if( selection==4 )//�滻 
	{
		InitConsole();
		char searchword[80],replaceword[80];
		printf("��������Ҫ���滻�ĵ��ʺ��滻�ʣ�\n��������Ҫ���滻�ĵ���:");
		scanf("%s",searchword);
		printf("�滻Ϊ��");
		scanf("%s",replaceword);
		replace(searchword,replaceword);
		printf("succeed!");
		FreeConsole();
	}
	
	// Format �˵�
	selection = menuList(GenUIID(0),x+2*w,  y-h, w, wlist,h, menuListFormat,sizeof(menuListFormat)/sizeof(menuListFormat[0]));
	if( selection>0 ) selectedLabel = menuListFormat[selection];
	if( selection==1)//�������� 
	{
		InitConsole();
		printf("��ѡ������: \n1)Times\n2)Courier\n");
		int Font; 
		Font=GetInteger();
		switch(Font)
		{
			case 1: SetFont("Times");break;
			case 2: SetFont("Courier");break;
			default: ;break;
		}
		FreeConsole();
	}
	if( selection==2)//��ɫ���� 
	{
		InitConsole();
		int color;
		printf("��ѡ����ɫ��\n1)��ɫ\n2)��ɫ\n3)��ɫ\n");
		color=GetInteger();
		switch(color)
		{
			case 1: setTextBoxColors("Blue", "Blue", "Red", "Blue", 0);break;
			case 2: setTextBoxColors("Blue", "Green", "Red", "Green", 0);break;
			case 3: setTextBoxColors("Blue", "Red", "Red", "Red", 0);break;
			default: ;break;
		}
		FreeConsole();
	}
	// Check �˵�
	selection = menuList(GenUIID(0),x+3*w,  y-h, w, wlist,h, menuListCheck,sizeof(menuListCheck)/sizeof(menuListCheck[0]));
	if( selection>0 ) selectedLabel = menuListCheck[selection];
	if( selection==1 )//�Ŵ� 
	{
		int size;
		size=GetPointSize();
		SetPointSize(size+10);
	}
	if( selection==2 )//��С 
	{
		int size;
		size=GetPointSize();
		SetPointSize(size-10);
	}
	
	// Help �˵�
	selection = menuList(GenUIID(0),x+4*w,y-h, w, wlist, h, menuListHelp,sizeof(menuListHelp)/sizeof(menuListHelp[0]));
	if( selection>0 ) selectedLabel = menuListHelp[selection];
	if( selection==1 )//ʹ��˵�� 
	{
		InitConsole();
		printf("************ʹ��˵��************\n");
		printf("�ļ���\n�򿪣����ⲿ�ı��ļ����в鿴�ͱ༭\n�½����½��ı��ļ����б༭\n���棺���༭���ݱ��浽��ǰ�ļ�\n���Ϊ�����༭���ݱ���Ϊ������ļ�\n�˳����˳������\n");
		printf("�༭��\n���ƣ���ѡ�е��ı����Ƶ����а�\nճ���������а��е��ı�ճ�����༭��\n���ң����ұ༭�����ض����ַ�\n�滻�����༭���������ض����ַ��滻Ϊ������ַ�\n");
		printf("��ʽ��\n���壺ѡ�������������\n��ɫ��ѡ��������ɫ\n"); 
		printf("�鿴��\n�Ŵ󣺽�����е���ʾȫ���Ŵ�\n��С��������е���ʾȫ����С\n");
		printf("������\nʹ�ã��������ʹ��˵��\n���ڱ�������������������Ϣ\n"); 
		FreeConsole();
	}
		if( selection==2 )//���� 
	{
		InitConsole();
		printf("************���ڱ����************\n");
		printf("������Ա������  ����  ����Զ\n");
		printf("����ʱ�䣺2022/5");
		FreeConsole();
	}

	// ��ʾ�˵����н��
	x = winwidth/25;
	y = winheight/16;
	SetPenColor("Blue");
	drawLabel(x, y-=h, "���ʹ��:");
	SetPenColor("Red");
	drawLabel(x+2.5*xindent, y, selectedLabel);
	SetPenColor("Blue");
	drawLabel(winwidth/2, y, "������:");
	int wordamount=0;
	for(i=0;i<=18;i++)
	{
		wordamount+=strlen(word[i]);
	}
	char sum[80];
	sprintf(sum,"%d",wordamount);
	wordsum=sum;
	drawLabel(winwidth/2+2.5*xindent, y, wordsum);
}
#endif // #if defined(DEMO_MENU)

#if defined(DEMO_EDITBOX)
// �ı��༭��ʾ����
void drawEditText()
{
	double fH = GetFontHeight();
	double h = fH*2; // �ؼ��߶�
	double w = winwidth; // �ؼ����
	double x = 0;
	double y = winheight/1.3;
	int i;

	for(i=0;i<=18;i++)
	{	
		y-=h*0.8;
		textbox(GenUIID(i), x, y, w, h, word[i], sizeof(word[0]));
	}
}
#endif // #if defined(DEMO_EDITBOX)

void display()
{
	// ����
	DisplayClear();

#if defined(DEMO_MENU)
	// ���ƺʹ���˵�
	drawMenu();
#endif

#if defined(DEMO_EDITBOX)
	// �༭�ı��ַ���
	drawEditText();
#endif

	// �����ƵĽ��put����Ļ��
	//UpdateDisplay();
}
