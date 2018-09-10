#include "monitor.h"

gdouble total = 0;
gdouble rate = 0;
gdouble cpu_rate[4] = {0,0,0,0};


gboolean handle_timeout (gpointer data)//a series of operations by time
{
    gint page_num;

    page_num = gtk_notebook_get_current_page (GTK_NOTEBOOK (notebook));//Get Curr Page
    switch (page_num)
    {
    case 0:
        gtk_list_store_clear (process_store);//Clear treeview
        get_process_info (process_store);//Re-read Process_info
        break;
    default:
        break;
    }
    return TRUE;
}

gboolean get_cpu_rate (gpointer data)//Get CPU UseRate
{
    int fd,i,j;
    gchar buffer[512];
    gchar *cpu_time[9];
    gchar *Scpu_time[9];
    gchar cpu_info[100] = {0};
    gchar *tmp;
    gchar *delim =  " ";//分隔字符串


    gdouble ctotal[5] = {0};
    gdouble cuser[5] = {0};
    gdouble cnice[5] = {0};
    gdouble csys[5] = {0};
    gdouble cidle[5] = {0};
    static gdouble fidle[5]  = {0};
    static gdouble ftotal[5] = {0};
    

    fd = open ("/proc/stat", O_RDONLY);//ReadFile
    read (fd, buffer, sizeof (buffer));
    close (fd);
    tmp = strstr (buffer, "cpu0");//Buffer ended with "cpu0"
    tmp--;
    *tmp = '\0';
    cpu_time[0] = strtok (buffer, delim);//split
    //strtok()函数用来将字符串分割成一个个片段
    for (i = 1; i < 9 ; i++)  cpu_time[i] = strtok (NULL, delim);

    //从系统启动开始累计到当前时刻，用户态的CPU时间
    cuser[0] = atoi (cpu_time[1]);//Change str to long
    //从系统启动开始累计到当前时刻，核心时间
    csys[0] = atoi(cpu_time[3]);
    //从系统启动开始累计到当前时刻，nice值为负的进程所占用的CPU时间
    cnice[0] = atoi(cpu_time[2]);
    //从系统启动开始累计到当前时刻，除硬盘IO等待时间以外其它等待时间
    cidle[0] = atoi(cpu_time[4]);
    //CPU总时间
    ctotal[0] = (cuser[0] + csys[0] + cnice[0] + cidle[0] + 
                 atoi(cpu_time[5]) + atoi(cpu_time[6]) + atoi(cpu_time[7]));
    //cpu usage=[(user_2 +sys_2+nice_2) - (user_1 + sys_1+nice_1)]/(total_2 - total_1)*100
    //cpu usage=((cpu2-cpu1) - (idle2-idle1))/(cpu2-cpu1)*100
    total = ctotal[0] - ftotal[0];
    // rate = ((cuser + csys + cnice) - (fuser + fsys + fnice)) / (total);
    rate = ((ctotal[0] - ftotal[0]) - (cidle[0] - fidle[0])) / (ctotal[0] - ftotal[0]);

    // fuser[0] = cuser[0];  
    // fsys[0] = csys[0]; 
    // fnice[0] = cnice[0]; 
    fidle[0] = cidle[0];
    ftotal[0] = ctotal[0];


    j = 1;
//CPU0 利用率
    fd = open ("/proc/stat", O_RDONLY);//ReadFile
    read (fd, buffer, sizeof (buffer));
    close (fd);
    char *p1, *p2;
    p1 = strstr(buffer, "cpu0 ");
    p2 = strstr(buffer, "cpu1 ");
    if (p1 == NULL || p2 == NULL || p1 > p2) {
    printf("Not found\n");
    } 
    else {
    p1 += strlen("cpu0 ");
    memcpy(cpu_info, p1, p2 - p1 - 1);
    // printf("0: %s\n", cpu_info);
    }
    Scpu_time[0] = strtok (cpu_info, " ");//split
    //strtok()函数用来将字符串分割成一个个片段
    for (i = 1; i < 8 ; i++)  Scpu_time[i] = strtok (NULL, " ");

    //从系统启动开始累计到当前时刻，用户态的CPU时间
    cuser[j] = atoi (Scpu_time[0]);//Change str to long
    //从系统启动开始累计到当前时刻，核心时间
    csys[j] = atoi(Scpu_time[2]);
    //从系统启动开始累计到当前时刻，nice值为负的进程所占用的CPU时间
    cnice[j] = atoi(Scpu_time[1]);
    //从系统启动开始累计到当前时刻，除硬盘IO等待时间以外其它等待时间
    cidle[j] = atoi(Scpu_time[3]);
    //CPU总时间
    ctotal[j] = (cuser[j] + csys[j] + cnice[j] + cidle[j] +
                 atoi(Scpu_time[4]) + atoi(Scpu_time[5]) + atoi(Scpu_time[6]));

    cpu_rate[j-1] = (((ctotal[j] - ftotal[j]) - (cidle[j] - fidle[j])) / (ctotal[j] - ftotal[j])) * 100;

    fidle[j] = cidle[j];
    ftotal[j] = ctotal[j];



    j = 2;
//CPU1 利用率
    fd = open ("/proc/stat", O_RDONLY);//ReadFile
    read (fd, buffer, sizeof (buffer));
    close (fd);
    p1 = strstr(buffer, "cpu1 ");
    p2 = strstr(buffer, "cpu2 ");
    if (p1 == NULL || p2 == NULL || p1 > p2) {
    printf("Not found\n");
    } 
    else {
    p1 += strlen("cpu1 ");
    memcpy(cpu_info, p1, p2 - p1 - 1);
    // printf("1: %s\n", cpu_info);
    }
    Scpu_time[0] = strtok (cpu_info, " ");//split
    //strtok()函数用来将字符串分割成一个个片段
    for (i = 1; i < 8 ; i++)  Scpu_time[i] = strtok (NULL, " ");

    //从系统启动开始累计到当前时刻，用户态的CPU时间
    cuser[j] = atoi (Scpu_time[0]);//Change str to long
    //从系统启动开始累计到当前时刻，核心时间
    csys[j] = atoi(Scpu_time[2]);
    //从系统启动开始累计到当前时刻，nice值为负的进程所占用的CPU时间
    cnice[j] = atoi(Scpu_time[1]);
    //从系统启动开始累计到当前时刻，除硬盘IO等待时间以外其它等待时间
    cidle[j] = atoi(Scpu_time[3]);
    //CPU总时间
    ctotal[j] = (cuser[j] + csys[j] + cnice[j] + cidle[j] + 
                 atoi(Scpu_time[4]) + atoi(Scpu_time[5]) + atoi(Scpu_time[6]));
    cpu_rate[j-1] = (((ctotal[j] - ftotal[j]) - (cidle[j] - fidle[j])) / (ctotal[j] - ftotal[j])) * 100;

    fidle[j] = cidle[j];
    ftotal[j] = ctotal[j];

    j = 3;
//CPU2 利用率
    fd = open ("/proc/stat", O_RDONLY);//ReadFile
    read (fd, buffer, sizeof (buffer));
    close (fd);
    p1 = strstr(buffer, "cpu2 ");
    p2 = strstr(buffer, "cpu3 ");
    if (p1 == NULL || p2 == NULL || p1 > p2) {
    printf("Not found\n");
    } 
    else {
    p1 += strlen("cpu2 ");
    memcpy(cpu_info, p1, p2 - p1 - 1);
    // printf("%s\n", cpu_info);
    }
    Scpu_time[0] = strtok (cpu_info, " ");//split
    //strtok()函数用来将字符串分割成一个个片段
    for (i = 1; i < 8 ; i++)  Scpu_time[i] = strtok (NULL, " ");

    //从系统启动开始累计到当前时刻，用户态的CPU时间
    cuser[j] = atoi (Scpu_time[0]);//Change str to long
    //从系统启动开始累计到当前时刻，核心时间
    csys[j] = atoi(Scpu_time[2]);
    //从系统启动开始累计到当前时刻，nice值为负的进程所占用的CPU时间
    cnice[j] = atoi(Scpu_time[1]);
    //从系统启动开始累计到当前时刻，除硬盘IO等待时间以外其它等待时间
    cidle[j] = atoi(Scpu_time[3]);
    //CPU总时间
    ctotal[j] = (cuser[j] + csys[j] + cnice[j] + cidle[j] + 
                 atoi(Scpu_time[4]) + atoi(Scpu_time[5]) + atoi(Scpu_time[6]));
    cpu_rate[j-1] = (((ctotal[j] - ftotal[j]) - (cidle[j] - fidle[j])) / (ctotal[j] - ftotal[j])) * 100;

    fidle[j] = cidle[j];
    ftotal[j] = ctotal[j];


    j = 4;
//CPU3 利用率
    fd = open ("/proc/stat", O_RDONLY);//ReadFile
    read (fd, buffer, sizeof (buffer));
    close (fd);
    p1 = strstr(buffer, "cpu3 ");
    p2 = strstr(buffer, "intr ");
    if (p1 == NULL || p2 == NULL || p1 > p2) {
    printf("Not found\n");
    } 
    else {
    p1 += strlen("cpu3 ");
    memcpy(cpu_info, p1, p2 - p1 - 1);
    // printf("%s\n", cpu_info);
    }
    Scpu_time[0] = strtok (cpu_info, " ");//split
    //strtok()函数用来将字符串分割成一个个片段
    for (i = 1; i < 8 ; i++)  Scpu_time[i] = strtok (NULL, " ");

    //从系统启动开始累计到当前时刻，用户态的CPU时间
    cuser[j] = atoi (Scpu_time[0]);//Change str to long
    //从系统启动开始累计到当前时刻，核心时间
    csys[j] = atoi(Scpu_time[2]);
    //从系统启动开始累计到当前时刻，nice值为负的进程所占用的CPU时间
    cnice[j] = atoi(Scpu_time[1]);
    //从系统启动开始累计到当前时刻，除硬盘IO等待时间以外其它等待时间
    cidle[j] = atoi(Scpu_time[3]);
    //CPU总时间
    ctotal[j] = (cuser[j] + csys[j] + cnice[j] + cidle[j] +
                 atoi(Scpu_time[4]) + atoi(Scpu_time[5]) + atoi(Scpu_time[6]));

    cpu_rate[j-1] = (((ctotal[j] - ftotal[j]) - (cidle[j] - fidle[j])) / (ctotal[j] - ftotal[j])) * 100;

    fidle[j] = cidle[j];
    ftotal[j] = ctotal[j];

    return TRUE;
}

gboolean load_graph_refresh (GtkWidget *widget)
{
    draw_cpu_load_graph ();
    draw_mem_load_graph ();
    get_network_info ();
    return TRUE;
}

