#include "monitor.h"

void create_sysinfo_page()
{
    vbox = gtk_vbox_new (FALSE, 0);//添加纵向盒子
    gtk_widget_show (vbox);
    gtk_container_add (GTK_CONTAINER(notebook), vbox);

    frame = gtk_frame_new ("CPU Information:");
    gtk_widget_show (frame);//显示CPU信息边框
    gtk_box_pack_start (GTK_BOX (vbox), frame, TRUE, TRUE, 10);//加到纵向盒子里

    info = g_string_new ("");
    get_cpu_info (info);//用info变量存储获得的CPU信息
    info_label = gtk_label_new (info->str);//用标签info_label显示info变量的字符串内容
    gtk_widget_show (info_label);
    gtk_container_add (GTK_CONTAINER(frame), info_label);//将该标签加入边框中

    frame = gtk_frame_new ("OS Information:");//显示操作系统信息边框
    gtk_widget_show (frame);
    gtk_box_pack_start (GTK_BOX (vbox), frame, TRUE, TRUE, 30);

    info = g_string_new ("");
    get_os_info (info);//用info变量存储获得的操作系统信息
    info_label = gtk_label_new (info->str);//用标签info_label显示info变量的字符串内容
    gtk_widget_show (info_label);
    gtk_container_add (GTK_CONTAINER(frame), info_label);


    label = gtk_label_new ("sys_information");
    gtk_widget_set_size_request (label, 120, 20);
    gtk_widget_show (label);
    //notebook标签标题
    gtk_notebook_set_tab_label (GTK_NOTEBOOK (notebook),
                                gtk_notebook_get_nth_page (GTK_NOTEBOOK (notebook),3), label);
}

void get_cpu_info(GString *string)//获得cpu信息
{
       int fd,i;
       gchar buffer[1024];								
       gchar *delim = "\n";
       gchar *cpu_info[17];
       gchar *tmp;
       fd = open ("/proc/cpuinfo", O_RDONLY);//读取cpu相关信息的文件
       //一口气将文件中所有信息读取出来
       read (fd, buffer, sizeof (buffer));
       close (fd);
       //将从cpuinfo文件中读取出来的字符串依据\n符号进行分割
       cpu_info[0] = strtok (buffer, delim);
       for (i=1; i < 17 ; i++) {
		cpu_info[i] = strtok (NULL,delim);//分割字符q串
       }	  
       for (i=0; i < 17; i++) {
  	  	tmp = strstr (cpu_info[i], ":");//忽略冒号:
		tmp += 2; 
        cpu_info[i] = tmp;	  
       }
       g_string_append (string, "CPU Model:");
       g_string_append (string, cpu_info[4]);
       g_string_append_c (string, '\n');
       g_string_append (string, "CPU cores\t:");
       g_string_append (string, cpu_info[12]);
       g_string_append_c (string, '\n');
       g_string_append (string, "CPU rate\t:");
       g_string_append (string, cpu_info[7]);
       g_string_append(string, "MHz");
       g_string_append_c (string, '\n');
       g_string_append (string, "Cache size\t:");
       g_string_append (string, cpu_info[8]);
       g_string_append_c (string, '\n');
}

void get_os_info (GString *string)//获得操作系统相关信息
{
    int fd,i;
    gchar buffer[128];
    // gchar *tmp,*start,
    gchar *stop;
    gchar *os_info[8];
    gchar *delim = " ";

    // gchar buffer1[128];
    // gchar *os_info1[3];
    // gchar *delim1 = ".";

    fd = open ("/proc/version", O_RDONLY);//读取操作系统相关信息的文件
    read(fd, buffer, sizeof (buffer));

    // start = buffer;
    stop = strstr (buffer, "#");//找到"#"在buffer中第一次出现的位置,返回该位置的指针
    stop--;
    stop--;
    os_info[0] = strtok (buffer, delim);
    for (i = 1; i < 8; i++)
    {
        os_info[i] = strtok (NULL, delim);//分割字符串
    }

    // for(j=0;j<7;j++){
    // 	buffer1[j] = os_info[2][j];
    // }
    // os_info1[0] = strtok(buffer1,delim1);
    // for(j = 1; j<3; j++)
    // {
    // 	os_info1[j] = strtok(NULL,delim1);
    // }

    //字符串串接显示相关信息
    g_string_append (string, "Os\t\t\t\t:");
    g_string_append (string, os_info[0]);
    g_string_append_c (string, '\n');
    g_string_append (string, "Release\t\t:");
    g_string_append (string, os_info[2]);
    
    // g_string_append (string, os_info1[0]);
    // g_string_append (string, ".");
    // g_string_append (string, os_info1[1]);
    // g_string_append (string, ".");
    // g_string_append (string, os_info1[2]);
    
    g_string_append_c (string, '\n');
    g_string_append (string, "Domain\t\t:");
    g_string_append (string, os_info[3]);
    g_string_append_c (string, '\n');
    g_string_append (string, "Gcc version\t:");
    g_string_append (string, os_info[6]);
    g_string_append_c (string, ' ');
    g_string_append (string, os_info[7]);
    g_string_append_c (string, '\n');
}