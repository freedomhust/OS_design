#include <fcntl.h>
#include <stdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <dirent.h>
#include <gtk/gtk.h>
#include <gdk/gdk.h>
#include <sys/stat.h>
#include <gdk/gdkkeysyms.h>
#include <sys/statfs.h>


#define DENSITY 100         //图形点密度
#define PMAX    100         //最大进程数目

enum
{
    NAME_COLUMN,  //进程名列
    PID_COLUMN,    //进程PID列
    STATUS_COLUMN, //进程状态列
    CPU_COLUMN,  //CPU信息列
    MEMORY_COLUMN,//内存信息列
    NP_COLUMNS
};

enum
{
    DEV_COLUMN, //磁盘名称列
    DIR_COLUMN,  //相应目录列
    TYPE_COLUMN, //类型列
    TOTAL_COLUMN,//内存总量列
    USED_COLUMN, //已用量列
    FREE_COLUMN, //可用量列
    RATE_COLUMN, //已用百分比
    ND_COLUMNS
};

enum
{
    MOD_COLUMN,
    DEPEND_COLUMN,
    NM_COLUMNS
};

//Declare of GtkWidget
GtkWidget *vbox;      //垂直布局控件
GtkWidget *hbox;      //水平布局
GtkWidget *label;     //标签
GtkWidget *frame;     //框架
GtkWidget *window;    //窗口
GtkWidget *menubar;   //菜单栏
GtkWidget *notebook;  //笔记本
GtkWidget *main_vbox; 
GtkWidget *button_one;
GtkWidget *button_two;
GtkWidget *box;
GtkWidget *mem_label;
GtkWidget *swap_label;
GtkWidget *info_label;
GtkWidget *status_bar;
GtkWidget *ptree_view;
GtkWidget *mtree_view;
GtkWidget *cpu_draw_area;
GtkWidget *mem_draw_area;
GtkWidget *scrolled_window;
GtkWidget *prefresh_button , *pkill_button; //Refresh & Kill Button

GString   *info;

// Global Varities
gint cpu_graph[DENSITY];                    //CPU Graphic Density
gint mem_graph[DENSITY];                    //RAM Graphic Density
gdouble rate = 0;                           //cpu Use_Rate
gdouble total = 0;                          //当前进程总数
gdouble fuser = 0;                          //当前CPU使用信息
gdouble ftotal = 0;                         //当前CPU使用总数l
gdouble old_cutime,old_cstime,old_utime,old_stime;
GString   *info;
GdkPixmap *cgraph = NULL;                   //pixmap_Pointer_CPU
GdkPixmap *mgraph = NULL;                   //pixmap_Pointer_RAM

//Others
GtkListStore *process_store;
GtkListStore *memory_store;
GtkCellRenderer *renderer;//用于显示tree view中的每个列标题
GtkTreeViewColumn *column;//tree view控件列数

void create_process_page();
void create_page_two();
void create_sysinfo_page();
void create_memory_page();

void get_process_info (GtkListStore *store);
void get_cpu_info(GString *string);
void get_os_info (GString *string);
void get_memory_info (GtkListStore *store);

void destroy_window (GtkWidget *, gpointer);

void prefresh_button_clicked (gpointer data);
void pkill_button_clicked(gpointer data);

gboolean handle_timeout (gpointer data);
gboolean get_cpu_rate (gpointer data);

void show_dialog (gchar *, gchar *);

int main(int argc, char **argv)
{
    gtk_set_locale();         //为本地环境读入本地化数据库,必须在gtk_init前调用
    gtk_init (&argc, &argv);  //初始化
    
    //新建主窗口
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    //使窗口获得伸缩功能
    gtk_window_set_resizable(GTK_WINDOW(window),TRUE);
    //设置窗口大小
    gtk_window_set_default_size (GTK_WINDOW (window), 400, 300);
    //窗口显示位置
    gtk_window_set_position(GTK_WINDOW(window),GTK_WIN_POS_CENTER_ALWAYS);
    //退出时调用的函数
    g_signal_connect(G_OBJECT(window), "destroy",G_CALLBACK(destroy_window), NULL);
    //窗口标题
    gtk_window_set_title(GTK_WINDOW(window),"monitor");


    //设置分窗口
    notebook = gtk_notebook_new ();
    //显示notebook
    gtk_widget_show (notebook);
    // 笔记本放进窗口
    gtk_container_add(GTK_CONTAINER(window), notebook); 
    //笔记本标签的位置，上方
    gtk_notebook_set_tab_pos(GTK_NOTEBOOK (notebook), GTK_POS_TOP);

    gtk_notebook_set_scrollable(GTK_NOTEBOOK (notebook),TRUE);

    //2ms进行一次刷新
    gtk_timeout_add (2000, (GtkFunction)handle_timeout, NULL);
    gtk_timeout_add (1000, (GtkFunction)get_cpu_rate, NULL); //CPUrate refresh per second

    create_process_page();
    create_page_two();
    create_sysinfo_page();
    create_memory_page();

    //显示所有控件
    // gtk_widget_show_all(window); 
    //显示窗口
    gtk_widget_show(window);

    // // 设置起始页(第2页)，从0开始算，相当于设置第2个页面显示
	// gtk_notebook_set_current_page(GTK_NOTEBOOK(notebook), 1); 
    //初始化
    gtk_main();
    return 0;
}

void destroy_window (GtkWidget *widget, gpointer data)//退出窗口管理程序
{
    gtk_main_quit ();
}

void create_process_page()
{
    int i;
    gchar *col_name[5] = { "NAME", "PID" , "STATUS", "CPU" , "MEMORY"};

    vbox = gtk_vbox_new (FALSE, 0);//添加纵向盒子
    gtk_widget_show (vbox);
    gtk_container_add (GTK_CONTAINER (notebook), vbox);

    scrolled_window = gtk_scrolled_window_new (NULL, NULL);//添加滚动窗口控件
    gtk_widget_set_size_request (scrolled_window, 600, 400);
    gtk_widget_show (scrolled_window);
    gtk_box_pack_start (GTK_BOX (vbox), scrolled_window, TRUE, TRUE, 0);

    //进程信息,创建了一个5列的链表，且数据的形式都是string
    process_store = gtk_list_store_new (NP_COLUMNS,
                                        G_TYPE_STRING,
                                        G_TYPE_STRING,
                                        G_TYPE_STRING,
                                        G_TYPE_STRING,
                                        G_TYPE_STRING);

    ptree_view = gtk_tree_view_new_with_model (GTK_TREE_MODEL (process_store));
    //添加tree view控件,显示进程信息树

    g_object_unref (G_OBJECT (process_store));//减少引用次数

    gtk_widget_show (ptree_view);
    gtk_container_add (GTK_CONTAINER (scrolled_window), ptree_view);//Tree in scrolled_window
    //将进程信息树加到滚动窗口中pdelete_button_clicked

    for (i = 0; i < 5; i++)
    {
        renderer = gtk_cell_renderer_text_new ();//添加一个cell_renderer_text用于显示文字
        // g_object_set(G_OBJECTpdelete_button_clicked(renderer),"foreground","blue",NULL);
        column = gtk_tree_view_column_new_with_attributes (col_name[i],
                 renderer,"text",i,NULL);//新建一列
        gtk_tree_view_append_column (GTK_TREE_VIEW (ptree_view), column);//将该列加到树中 
    }

    get_process_info (process_store); //获取进程信息并存取显示到列表

    hbox = gtk_hbox_new (FALSE, 0);//添加横排列盒子
    gtk_widget_show (hbox);
    gtk_box_pack_start (GTK_BOX (vbox), hbox, TRUE, FALSE, 0);//增加到竖排列盒子中

    prefresh_button = gtk_button_new ();//创建刷新进程信息按钮
    gtk_widget_show (prefresh_button);
    gtk_widget_set_size_request (prefresh_button, 70, 30);//Set Button Size
    gtk_button_set_label (GTK_BUTTON (prefresh_button), "refresh");//设置刷新按钮上的文字
    g_signal_connect (G_OBJECT (prefresh_button),"clicked",
                      G_CALLBACK(prefresh_button_clicked),
                      NULL);//刷新按钮点击后执行prefresh_button_clicked
    gtk_box_pack_start (GTK_BOX (hbox), prefresh_button, TRUE, FALSE, 0);//增加该按钮到横排列盒子中

    pkill_button = gtk_button_new ();//创建杀死进程按钮
    gtk_widget_show (pkill_button);
    gtk_widget_set_size_request (pkill_button, 70, 30);
    gtk_button_set_label (GTK_BUTTON (pkill_button), "kill");
    g_signal_connect (G_OBJECT (pkill_button),"clicked",
                      G_CALLBACK(pkill_button_clicked),
                      NULL);//该按钮点击后执行pdelete_button_clicked
    gtk_box_pack_start (GTK_BOX (hbox), pkill_button, TRUE, FALSE, 0);//增加该按钮到横排列盒子中


    label = gtk_label_new ("processes");
    gtk_widget_show (label);
    gtk_notebook_set_tab_label (GTK_NOTEBOOK (notebook),
                                gtk_notebook_get_nth_page (GTK_NOTEBOOK (notebook), 0), label);

}

void create_page_two()
{
    // 第二个页面
	label = gtk_label_new("CPU");	//创建标签
	box = gtk_hbox_new(TRUE, 5);	// 创建布局
	button_one = gtk_button_new_with_label("我是第二个页面");
	button_two = gtk_button_new_with_label("我是第二个页面的按钮");
	gtk_container_add(GTK_CONTAINER(box), button_one);
	gtk_container_add(GTK_CONTAINER(box), button_two);
	gtk_notebook_append_page(GTK_NOTEBOOK(notebook), box, label); 
    gtk_widget_show(box);
    gtk_widget_show(label);
    gtk_widget_show(button_one);
    gtk_widget_show(button_two);
}

void create_sysinfo_page()
{
    // // 第三个页面
	// label = gtk_label_new("系统信息");	//创建标签
	// box = gtk_hbox_new(TRUE, 5);	// 创建布局
	// button_one = gtk_button_new_with_label("我是第三个页面");
	// button_two = gtk_button_new_with_label("我是第三个页面的按钮");
	// gtk_container_add(GTK_CONTAINER(box), button_one);
	// gtk_container_add(GTK_CONTAINER(box), button_two);
	// gtk_notebook_append_page(GTK_NOTEBOOK(notebook), box, label); 

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
    gtk_widget_show (label);
    //notebook标签标题
    gtk_notebook_set_tab_label (GTK_NOTEBOOK (notebook),
                                gtk_notebook_get_nth_page (GTK_NOTEBOOK (notebook),2), label);
}

void create_memory_page()
{
    int i;
    gchar *col_name[7] = { "DEV", "DIR" , "TYPE", "TOTAL" , "USED", "FREE", "RATE"};

    vbox = gtk_vbox_new (FALSE, 0);//添加纵向盒子
    gtk_widget_show (vbox);
    gtk_container_add (GTK_CONTAINER (notebook), vbox);

    scrolled_window = gtk_scrolled_window_new (NULL, NULL);//添加滚动窗口控件
    gtk_widget_set_size_request (scrolled_window, 300, 300);
    gtk_widget_show (scrolled_window);
    gtk_box_pack_start (GTK_BOX (vbox), scrolled_window, TRUE, TRUE, 0);

    //进程信息,创建了一个7列的链表，且数据的形式都是string
    memory_store = gtk_list_store_new (ND_COLUMNS,
                                        G_TYPE_STRING,
                                        G_TYPE_STRING,
                                        G_TYPE_STRING,
                                        G_TYPE_STRING,
                                        G_TYPE_STRING,
                                        G_TYPE_STRING,
                                        G_TYPE_STRING);
    
    mtree_view = gtk_tree_view_new_with_model (GTK_TREE_MODEL (memory_store));
    //添加tree view控件,显示进程信息树

    g_object_unref (G_OBJECT (memory_store));//减少引用次数

    gtk_widget_show (mtree_view);
    gtk_container_add (GTK_CONTAINER (scrolled_window), mtree_view);//Tree in scrolled_window
    //将进程信息树加到滚动窗口中

    for (i = 0; i < 7; i++)
    {
        renderer = gtk_cell_renderer_text_new ();//添加一个cell_renderer_text用于显示文字
        // g_object_set(G_OBJECT(renderer),"foreground","blue",NULL);
        column = gtk_tree_view_column_new_with_attributes (col_name[i],
                 renderer,"text",i,NULL);//新建一列
        gtk_tree_view_append_column (GTK_TREE_VIEW (mtree_view), column);//将该列加到树中 
    }

    get_memory_info(memory_store);

    label = gtk_label_new ("memory");
    gtk_widget_show (label);
    gtk_notebook_set_tab_label (GTK_NOTEBOOK (notebook),
                                gtk_notebook_get_nth_page (GTK_NOTEBOOK (notebook),3), label);
}

void get_process_info (GtkListStore *store)//获取进程信息
{
    DIR *dir;
    int fd,i,num = 0;
    GtkTreeIter iter;
    struct dirent *entry;
    gdouble mem;
    gdouble srate;
    gdouble now_cutime,now_cstime,now_utime,now_stime;
    gchar *info[26];
    gchar buffer[128];
    gchar dir_buf[256];
    gchar *delim = " ";
    gchar mem_buffer[16];
    gchar rate_buffer[16];
    gchar state[3][10]= {"running", "sleeping", "else"};

    //opendir打开一个与给定目录名相对应的目录流,返回指向该目录流的指针
    dir = opendir ("/proc");// 打开/proc文件目录,dir为返回指针
    while ((entry = readdir (dir)) != NULL )
    {
        if ((entry->d_name[0] >= '0') && (entry->d_name[0] <= '9'))
        {
            sprintf (dir_buf, "/proc/%s/stat", entry->d_name);//读文件到dir_buf中
            fd = open (dir_buf, O_RDONLY);//只读打开
            read (fd, buffer, sizeof (buffer));
            close (fd);

            //strtok用于分割字符串
            info[0] =  strtok (buffer, delim);//以delim分隔符切割buffer的一部分到info[0]
            for (i = 1; i < 26 ; i++){
            	     info[i] = strtok(NULL, delim);//查找整个buffer字符串
            }
            info[1]++;
            info[1] = strtok(info[1], ")");

            switch(info[2][0])
            {
            case 'R':
                info[2] = state[0];
                break;
            case 'S':
                info[2] = state[1];
                break;
            default :
                info[2] = state[2];
            }

            //该任务在用户态运行的时间
            now_utime = atoi (info[13]);
            //该任务在核心态运行的时间
            now_stime = atoi (info[14]);
            //累计的该任务的所有的waited-for进程曾经在用户态运行的时间
            now_cutime = atoi (info[15]);
            //累计的该任务的所有的waited-for进程曾经在核心态运行的时间
            now_cstime = atoi (info[16]);

            srate = ((now_utime + now_stime + now_cutime + now_cstime) - 
                     (old_utime + old_stime + old_cutime + old_cstime)) / total;

            if(srate<0||srate>1)
            {
                srate=0;
            }
            sprintf (rate_buffer, "%.2f%%",100 * srate);

            mem = atoi (info[22]);//该进程的内存使用量
            mem = mem / (1024 * 1024);
            sprintf (mem_buffer, "%-.2f MB",mem);

            gtk_list_store_append (store, &iter);//增加到列表
            gtk_list_store_set (store, &iter,
                                NAME_COLUMN,info[1],
                                PID_COLUMN,info[0],
                                STATUS_COLUMN,info[2],
                                CPU_COLUMN,rate_buffer,
                                MEMORY_COLUMN,mem_buffer,
                                -1);
            old_utime = now_utime;
            old_stime = now_stime;
            old_cutime = now_cutime;
            old_cstime = now_cstime;
        }
    }
    closedir (dir);
}

void get_cpu_info(GString *string){//获得cpu信息
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
		cpu_info[i] = strtok (NULL,delim);//分割字符串
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
    int fd,i,j;
    gchar buffer[128];
    gchar *tmp,*start,*stop;
    gchar *os_info[8];
    gchar *delim = " ";

    gchar buffer1[128];
    gchar *os_info1[3];
    gchar *delim1 = ".";

    fd = open ("/proc/version", O_RDONLY);//读取操作系统相关信息的文件
    read(fd, buffer, sizeof (buffer));

    start = buffer;
    stop = strstr (buffer, "#");//找到"#"在buffer中第一次出现的位置,返回该位置的指针
    stop--;
    stop--;
    os_info[0] = strtok (buffer, delim);
    for (i = 1; i < 8; i++)
    {
        os_info[i] = strtok (NULL, delim);//分割字符串

    }

    for(j=0;j<7;j++){
    	buffer1[j] = os_info[2][j];
    }
    os_info1[0] = strtok(buffer1,delim1);
    for(j = 1; j<3; j++)
    {
    	os_info1[j] = strtok(NULL,delim1);
    }

    //字符串串接显示相关信息
    g_string_append (string, "Os\t\t\t\t:");
    g_string_append (string, os_info[0]);
    g_string_append_c (string, '\n');
    g_string_append (string, "Release\t\t:");
    g_string_append (string, os_info[2]);
    /*
    g_string_append (string, os_info1[0]);
    g_string_append (string, ".");
    g_string_append (string, os_info1[1]);
    //g_string_append (string, ".");
    //g_string_append (string, os_info1[2]);
    */
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

void get_memory_info (GtkListStore *store)
{
    GtkTreeIter iter;
    FILE *fp;
    gint i;
    gchar s[1024];
    gchar *mem[9];
    gdouble rate;
    gchar dev_rate[15];
    struct statfs diskInfo; 
    unsigned long long blocksize;
    unsigned long long totalsize;
    gchar total[15];
    unsigned long long freeDisk;
    unsigned long long availableDisk;
    gchar available[15];
    unsigned long long usedDisk;
    gchar used[15];
      
    if((fp = fopen("/etc/mtab","r")) == NULL){
        printf("error!\n");
    }
    else{
        // printf("设备  目录  类型\n");
        while(fgets(s,1024,fp) != NULL){
            i = 0;
            if(s[0] == '/'){
                mem[0] = strtok (s," ");
                for(i = 1; i < 9; i++) mem[i] = strtok(NULL," ");
                statfs(mem[1],&diskInfo);
                // printf("%s\n",mem[0]);
                blocksize = diskInfo.f_bsize; //每个block里包含的字节数 
                totalsize = blocksize * diskInfo.f_blocks; //总的字节数，f_blocks为block的数目

                if((totalsize>>20) < 1024)   sprintf(total,"%llu MB", totalsize>>20);
                else sprintf(total , "%llu GB", totalsize>>30);

                // printf("Total_size = %llu B = %llu KB = %llu MB = %llu GB\n", 
                //     totalsize, totalsize>>10, totalsize>>20, totalsize>>30); 

                freeDisk = diskInfo.f_bfree * blocksize; //剩余空间的大小 
                availableDisk = diskInfo.f_bavail * blocksize; //可用空间大小
                if((availableDisk>>20) < 1024)   sprintf(available,"%llu MB", availableDisk>>20);
                else sprintf(available , "%llu GB", availableDisk>>30);

                // printf("Disk_free = %llu MB = %llu GB\nDisk_available = %llu MB = %llu GB\n", 
                //     freeDisk>>20, freeDisk>>30, availableDisk>>20, availableDisk>>30);

                usedDisk = totalsize - availableDisk;

                if((usedDisk>>20) < 1024)   sprintf(used,"%llu MB", usedDisk>>20);
                else sprintf(used , "%llu GB", usedDisk>>30);

                rate = (double)usedDisk / totalsize;
                sprintf (dev_rate, "%.2f%%",100 * rate);
                // printf("Disk_used = %llu MB = %llu GB\nDisk_rate = %.1lf \n", 
                //     usedDisk>>20, usedDisk>>30, rate * 100);

            gtk_list_store_append (store, &iter);//增加到列表
            gtk_list_store_set (store, &iter,
                                DEV_COLUMN,mem[0],
                                DIR_COLUMN,mem[1],
                                TYPE_COLUMN,mem[2],
                                TOTAL_COLUMN,total,
                                USED_COLUMN,used,
                                FREE_COLUMN,available,
                                RATE_COLUMN,dev_rate,
                                -1);
            
            }
        }
    }
}

void prefresh_button_clicked (gpointer data)
{
    // printf("1");
    gtk_list_store_clear (process_store);
    get_process_info (process_store);
}

void pkill_button_clicked(gpointer data)//kill按钮被点击,停止一个进程的运行
{
    GtkTreeSelection *selection;
    GtkTreeModel *model;
    GtkTreeIter iter;
    gchar *pid;
    pid_t pid_num;

    // printf("1");
    selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(ptree_view));//获得当前选择的项
    if (gtk_tree_selection_get_selected(selection, &model, &iter))
    {
        gtk_tree_model_get (model, &iter, PID_COLUMN, &pid, -1);//在书的相应列中获得该进程的PID
        pid_num = atoi (pid);//字符串转换成长整数
        // printf("%s\n",pid);
        if(kill (pid_num, SIGTERM) == -1 )//根据PID结束该进程
        {
            gchar *title = "ERROR";
            gchar *content = "Termination Failed,Check UID";
            show_dialog (title, content);//结束进程失败信息输出
        }
        gtk_list_store_clear (process_store);//刷新进程信息
        get_process_info (process_store);
    }
    else printf("2");
}

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
    int fd,i;
    gchar buffer[256];
    gchar *cpu_time[9];
    gchar *tmp;
    gchar *delim =  " ";//分隔字符串
    gdouble cuser;
    gdouble ctotal;
    gdouble csys,fsys;
    gdouble cnice,fnice;

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
    cuser = atoi (cpu_time[1]);//Change str to long
    //从系统启动开始累计到当前时刻，核心时间
    csys = atoi(cpu_time[3]);
    //从系统启动开始累计到当前时刻，nice值为负的进程所占用的CPU时间
    cnice = atoi(cpu_time[2]);
    //CPU总时间
    ctotal = (atoi(cpu_time[1]) + atoi(cpu_time[2]) + atoi(cpu_time[3]) + atoi(cpu_time[4]));
    //cpu usage=[(user_2 +sys_2+nice_2) - (user_1 + sys_1+nice_1)]/(total_2 - total_1)*100
    total = ctotal - ftotal;
    rate = ((cuser + csys + cnice) - (fuser + fsys + fnice)) / (total);
    fuser = cuser;
    fsys = csys;
    fnice = cnice;
    ftotal = ctotal;
    
    return TRUE;
}

void show_dialog (gchar *title, gchar *content)//显示一个对话框,标题title,内容为content
{
    GtkWidget *dialog;
    GtkWidget *label;
    dialog = gtk_dialog_new_with_buttons (title,
                                          GTK_WINDOW (window),
                                          GTK_DIALOG_DESTROY_WITH_PARENT,
                                          GTK_STOCK_CLOSE,
                                          GTK_RESPONSE_NONE,
                                          NULL);//创建一个对话框控件
    gtk_window_set_resizable (GTK_WINDOW (dialog), FALSE);//不可改变大小
    g_signal_connect_swapped (dialog,
                              "response",
                              G_CALLBACK (gtk_widget_destroy),
                              dialog);//事件回调函数为退出

    label = gtk_label_new (content);//创建一个标签,显示content的内容
    gtk_widget_show (label);
    gtk_container_add (GTK_CONTAINER (GTK_DIALOG(dialog)->vbox),
                       label);
    gtk_widget_show (dialog);
}