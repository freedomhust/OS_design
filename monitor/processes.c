#include "monitor.h"

void create_process_page()
{
    int i;
    gchar *col_name[6] = { "NAME", "USER" ,"CPU" , "ID" , "STATUS" , "MEMORY"};

    vbox = gtk_vbox_new (FALSE, 0);//添加纵向盒子
    gtk_widget_show (vbox);
    //将vbox添加到notebook容器中
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
                                        G_TYPE_STRING,
                                        G_TYPE_STRING);

    ptree_view = gtk_tree_view_new_with_model (GTK_TREE_MODEL (process_store));
    //添加tree view控件,显示进程信息树

    g_object_unref (G_OBJECT (process_store));//减少引用次数

    gtk_widget_show (ptree_view);
    gtk_container_add (GTK_CONTAINER (scrolled_window), ptree_view);//Tree in scrolled_window
    //将进程信息树加到滚动窗口中pdelete_button_clicked

    for (i = 0; i < 6; i++)
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

    //为notebook设置标题
    label = gtk_label_new ("processes");
    gtk_widget_set_size_request (label, 120, 20);
    gtk_widget_show (label);
    //将本页面加入到notebook中，是第一页
    gtk_notebook_set_tab_label (GTK_NOTEBOOK (notebook),
                                gtk_notebook_get_nth_page (GTK_NOTEBOOK (notebook), 0), label);

}


void get_process_info (GtkListStore *store)//获取进程信息
{
    DIR *dir;
    int fd,i;
    FILE *fp;
    GtkTreeIter iter;
    struct dirent *entry;
    gdouble mem;
    gdouble srate;
    gdouble now_cutime,now_cstime,now_utime,now_stime;
    gint user_id;
    gchar *info[26];
    gchar *userinfo[5];
    gchar buffer[128];
    gchar s[1024];
    gchar dir_buf[256];
    gchar *delim = " ";
    gchar mem_buffer[16];
    gchar rate_buffer[16];
    gchar state[3][10]= {"running", "sleeping", "else"};
    struct passwd *pwd;

    static gdouble old_cutime = 0;
    static gdouble old_cstime = 0;
    static gdouble old_utime = 0;
    static gdouble old_stime = 0;

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

            //获得进程所属用户
            sprintf (dir_buf, "/proc/%s/status", entry->d_name);//读文件到dir_buf中
            // printf("%s ",dir_buf);
            if((fp = fopen (dir_buf, "r")) == NULL){
                printf("error!\n");//只读打开
            }
            else{
                while(fgets(s,1024,fp) != NULL)
                {
                    i = 0;
                    if(s[0] == 'U' && s[1] == 'i' && s[2] == 'd'){
                        userinfo[0] = strtok (s,"\t");
                        for(i = 1; i < 5; i++) userinfo[i] = strtok (NULL,"\t");
                        user_id = atoi(userinfo[1]);
                        //通过获得的Uid获得进程所属的用户名
                        pwd = getpwuid(user_id);
                    }
                }
                fclose(fp);
            }
            
            gtk_list_store_append (store, &iter);//增加到列表
            gtk_list_store_set (store, &iter,
                                NAME_COLUMN,info[1],
                                USER_COLUMN,pwd->pw_name,
                                CPU_COLUMN,rate_buffer,
                                PID_COLUMN,info[0],
                                STATUS_COLUMN,info[2],
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

void prefresh_button_clicked (gpointer data)
{
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