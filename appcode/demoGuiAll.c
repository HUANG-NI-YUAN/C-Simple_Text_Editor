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


// 全局变量
static double winwidth, winheight;   // 窗口尺寸
static float  rot_angle = 0;         // 三角形旋转角度
static int    enable_rotation = 1;   // 允许旋转
static int    show_more_buttons = 0; // 显示更多按钮
static char word[80][80]={0};        // 字符串储存 
static int Is_open = 0; //用来标记此刻有无打开已有的文件，1代表打开 
static char path[80] = {0};   //地址路径

// 清屏函数，provided in libgraphics
void DisplayClear(void); 
// 计时器启动函数，provided in libgraphics
void startTimer(int id,int timeinterval);

// 用户的显示函数
void display(void); 

// 用户的字符事件响应函数
void CharEventProcess(char ch)
{
	uiGetChar(ch); // GUI字符输入
	display(); //刷新显示
}

// 用户的键盘事件响应函数
void KeyboardEventProcess(int key, int event)
{
	uiGetKeyboard(key,event); // GUI获取键盘
	display(); // 刷新显示
}

// 用户的鼠标事件响应函数
void MouseEventProcess(int x, int y, int button, int event)
{
	uiGetMouse(x,y,button,event); //GUI获取鼠标
	display(); // 刷新显示
}

// 旋转计时器
#define MY_ROTATE_TIMER  1

// 用户的计时器时间响应函数
void TimerEventProcess(int timerID)
{
	if( timerID==MY_ROTATE_TIMER && enable_rotation ) 
	{
		rot_angle += 10; // 全局变量
		display(); // 刷新显示
	}
}
//检查数组word的值（debug用）
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

//将text文件中的内容逐行读入word[80][80],返回行数
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

//word初始化
void init_word()
{
	int i, j;
	for(i=0;i<80;i++){
		for(j=0;j<80;j++){
			word[i][j] = '\0';
		}
	}
} 

//将word中的内容输出到文件
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

// 用户主程序入口
// 仅初始化执行一次
void Main() 
{
	// 初始化窗口和图形系统
	SetWindowTitle("Graphics User Interface Demo");
	//SetWindowSize(10, 10); // 单位 - 英寸
	//SetWindowSize(15, 10);
	//SetWindowSize(10, 20);  // 如果屏幕尺寸不够，则按比例缩小
    InitGraphics();

	// 获得窗口尺寸
    winwidth = GetWindowWidth();
    winheight = GetWindowHeight();

	// 注册时间响应函数
	registerCharEvent(CharEventProcess); // 字符
	registerKeyboardEvent(KeyboardEventProcess);// 键盘
	registerMouseEvent(MouseEventProcess);      // 鼠标
	registerTimerEvent(TimerEventProcess);      // 定时器

	// 开启定时器
	startTimer(MY_ROTATE_TIMER, 50);            

	// 打开控制台，方便输出变量信息，便于调试
	// InitConsole(); 

}

#if defined(DEMO_MENU)
// 菜单演示程序
void drawMenu()
{ 
	static char * menuListFile[] = {"文件",  
		"打开  | Ctrl-O", // 快捷键必须采用[Ctrl-X]格式，放在字符串的结尾
		"新建  | Ctrl-N",
		"保存  | Ctrl-S",
		"另存为",
		"退出  | Ctrl-E"};
	static char * menuListFormular[] = {"编辑",
		"复制  | Ctrl-C",
		"粘贴  | Ctrl-V",
		"查找  | Ctrl-F",
		"替换  | Ctrl-H"};
	static char * menuListFormat[] = {"格式",
		"字体",
		"颜色"};
	static char * menuListCheck[] = {"查看",
		"放大",
		"缩小"};
	static char * menuListHelp[] = {"帮助",
		"使用  | Ctrl-M",
		"关于本软件"};
	static char * selectedLabel = NULL;//最近使用 
	static char * wordsum = NULL;//字数 

	double fH = GetFontHeight();
	double x = 0; //fH/8;
	double y = winheight;
	double h = fH*1.5; // 控件高度
	double w = TextStringWidth(menuListHelp[0])*2; // 控件宽度
	double wlist = TextStringWidth(menuListFormular[3])*1.2;
	double xindent = winheight/20; // 缩进
	int    selection,k;
	FILE *fp = NULL;  //文件指针 
	int Is_recover; 
	
	// menu bar
	drawMenuBar(0,y-h,winwidth,h);
	// File 菜单
	selection = menuList(GenUIID(0), x, y-h, w, wlist, h, menuListFile, sizeof(menuListFile)/sizeof(menuListFile[0]));
	if( selection>0 ) selectedLabel = menuListFile[selection];
	switch(selection)
	{
		case 1:  //"打开"操作 
		InitConsole();
		printf("请输入要打开文本的完全路径\n");
		while(1){
			gets(path);
			if((fp = fopen(path,"r"))!=NULL){
				printf("您已成功打开%s\n",path);
				Is_open = 1;
				break;
			}
			printf("路径输入错误或者您没有权限这么做（请再试一次^_^）:\n");
		} 
		read_file(fp);       
        fclose(fp);
//		insect_word();
		FreeConsole();
        Is_open = 1;             

		break;
		
		case 2:  //新建操作 
		init_word();
		Is_open = 0;
		for(k=0;k<80;k++){ //初始化path 
			path[k] = '\0';
		} 
		fp = NULL;
		display();
		break;
		
		case 3: //保存操作 
		if(Is_open == 1){
			print_word(path);
			InitConsole();
			printf("您的文件已经成功保存\n");
			FreeConsole();
		}else{
			InitConsole();
			printf("请输入储存文件的完整地址：\n");
			gets(path);
			while((fp = fopen(path,"r"))!=NULL){
				printf("该文件已存在,请选择是否覆盖：\n1)覆盖\n2)不覆盖\n");
				Is_recover = GetInteger();
				if(Is_recover == 1){
					break;
				}else{
					printf("请重新输入储存文件的完整地址：\n"); 
					gets(path);
				}	  
			}
			if(fp ==NULL|| Is_recover == 1){
				print_word(path);
				fclose(fp);
				printf("您的文件已成功保存！"); 
				Is_open = 1;
			} 
			FreeConsole();
		}
		break;  
		case 4:  //另存为操作 
		InitConsole();
		printf("请输入储存文件的完整地址：\n");
		gets(path);
		while((fp = fopen(path,"r"))!=NULL){
			printf("该文件已存在,请选择是否覆盖：\n1)覆盖\n2)不覆盖\n");
			Is_recover = GetInteger();
			if(Is_recover == 1){
				break;
			}else{
				printf("请重新输入储存文件的完整地址：\n"); 
				gets(path);
			}	  
		}
		if(fp ==NULL|| Is_recover == 1){
			print_word(path);
			fclose(fp);
			printf("您的文件已成功另存为！"); 
			Is_open = 1;
		} 
		FreeConsole();
		break;
		
		case 5:
		exit(-1);break;//退出操作 
		default:break;
	}
	
	// Formular 菜单
	static char copyword[800];
	int i;
	void copy(int x1,int y1,int x2,int y2,char word[][80])//复制所需函数 
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
	
	void paste(int x0,int y0, char copyword[])//粘贴所需函数 
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
	
	void replace(char searchword[],char replaceword[])//替换所需函数 
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
	if( selection==1 )//复制 
	{
		InitConsole();
		int x1,y1,x2,y2;
		printf("请输入复制起始点和结束点:\t格式：(行,列)(行,列)\t注意全部都是半角输入\n");//复制起点和终点 
		scanf("(%d,%d)(%d,%d)",&x1,&y1,&x2,&y2);
		copy(x1,y1,x2,y2,word);
		printf("已复制内容：%s",copyword);
		FreeConsole();
	}
	if( selection==2 )//粘贴 
	{
		InitConsole();
		int x0,y0;
		printf("已经复制的内容是：\n%s\n",copyword);
		printf("请输入粘贴起始点：\t格式：(行,列)\t注意全部都是半角输入\n");//粘贴起点 
		scanf("(%d,%d)",&x0,&y0);
		paste(x0,y0,copyword);
		FreeConsole();
	}
	if( selection==3 )//查找 
	{
		InitConsole();
		char searchword[80],replaceword[80];
		printf("请输入需要查找的单词：\n");
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
	if( selection==4 )//替换 
	{
		InitConsole();
		char searchword[80],replaceword[80];
		printf("请输入需要被替换的单词和替换词：\n请输入需要被替换的单词:");
		scanf("%s",searchword);
		printf("替换为：");
		scanf("%s",replaceword);
		replace(searchword,replaceword);
		printf("succeed!");
		FreeConsole();
	}
	
	// Format 菜单
	selection = menuList(GenUIID(0),x+2*w,  y-h, w, wlist,h, menuListFormat,sizeof(menuListFormat)/sizeof(menuListFormat[0]));
	if( selection>0 ) selectedLabel = menuListFormat[selection];
	if( selection==1)//字体设置 
	{
		InitConsole();
		printf("请选择字体: \n1)Times\n2)Courier\n");
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
	if( selection==2)//颜色设置 
	{
		InitConsole();
		int color;
		printf("请选择颜色：\n1)蓝色\n2)绿色\n3)红色\n");
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
	// Check 菜单
	selection = menuList(GenUIID(0),x+3*w,  y-h, w, wlist,h, menuListCheck,sizeof(menuListCheck)/sizeof(menuListCheck[0]));
	if( selection>0 ) selectedLabel = menuListCheck[selection];
	if( selection==1 )//放大 
	{
		int size;
		size=GetPointSize();
		SetPointSize(size+10);
	}
	if( selection==2 )//缩小 
	{
		int size;
		size=GetPointSize();
		SetPointSize(size-10);
	}
	
	// Help 菜单
	selection = menuList(GenUIID(0),x+4*w,y-h, w, wlist, h, menuListHelp,sizeof(menuListHelp)/sizeof(menuListHelp[0]));
	if( selection>0 ) selectedLabel = menuListHelp[selection];
	if( selection==1 )//使用说明 
	{
		InitConsole();
		printf("************使用说明************\n");
		printf("文件：\n打开：打开外部文本文件进行查看和编辑\n新建：新建文本文件进行编辑\n保存：将编辑内容保存到当前文件\n另存为：将编辑内容保存为另外的文件\n退出：退出本软件\n");
		printf("编辑：\n复制：将选中的文本复制到剪切板\n粘贴：将剪切板中的文本粘贴到编辑栏\n查找：查找编辑栏中特定的字符\n替换：将编辑栏中所有特定的字符替换为另外的字符\n");
		printf("格式：\n字体：选择软件所用字体\n颜色：选择文字颜色\n"); 
		printf("查看：\n放大：将软件中的显示全部放大\n缩小：将软件中的显示全部缩小\n");
		printf("帮助：\n使用：本软件的使用说明\n关于本软件：本软件的其他信息\n"); 
		FreeConsole();
	}
		if( selection==2 )//关于 
	{
		InitConsole();
		printf("************关于本软件************\n");
		printf("制作人员：张宇  姜毅  黄倪远\n");
		printf("制作时间：2022/5");
		FreeConsole();
	}

	// 显示菜单运行结果
	x = winwidth/25;
	y = winheight/16;
	SetPenColor("Blue");
	drawLabel(x, y-=h, "最近使用:");
	SetPenColor("Red");
	drawLabel(x+2.5*xindent, y, selectedLabel);
	SetPenColor("Blue");
	drawLabel(winwidth/2, y, "总字数:");
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
// 文本编辑演示程序
void drawEditText()
{
	double fH = GetFontHeight();
	double h = fH*2; // 控件高度
	double w = winwidth; // 控件宽度
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
	// 清屏
	DisplayClear();

#if defined(DEMO_MENU)
	// 绘制和处理菜单
	drawMenu();
#endif

#if defined(DEMO_EDITBOX)
	// 编辑文本字符串
	drawEditText();
#endif

	// 将绘制的结果put到屏幕上
	//UpdateDisplay();
}
