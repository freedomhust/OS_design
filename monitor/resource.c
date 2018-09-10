#include "monitor.h"

GdkPixmap *cgraph = NULL;                   //pixmap_Pointer_CPU
GdkPixmap *mgraph = NULL;                   //pixmap_Pointer_RAM

//CPU的利用率显示
GtkWidget *cpu_lable  = NULL;
GtkWidget *cpu0_lable = NULL;
GtkWidget *cpu1_lable = NULL;
GtkWidget *cpu2_lable = NULL;
GtkWidget *cpu3_lable = NULL;

//内存显示
GtkWidget *mem_label = NULL;
//交换分区显示
GtkWidget *swap_label = NULL;

//接收信息显示
GtkWidget *receive_label = NULL;
//发送信息显示
GtkWidget *transmit_label = NULL;

void create_resource_page()
{
    vbox = gtk_vbox_new (FALSE, 0);//添加纵向盒子
    gtk_widget_show (vbox);
    gtk_container_add (GTK_CONTAINER (notebook), vbox);//Add it!

//CPU 历史

    frame = gtk_frame_new ("CPU");//添加用于显示CPU信息的边框构件
    gtk_widget_show (frame);
    gtk_box_pack_start (GTK_BOX (vbox), frame, FALSE, FALSE, 0);//加到盒子中

    cpu_draw_area = gtk_drawing_area_new ();//Add DrawPad
    gtk_widget_show (cpu_draw_area);
    gtk_drawing_area_size (GTK_DRAWING_AREA (cpu_draw_area), 400, 100);//Set DrawArea
    gtk_container_add (GTK_CONTAINER (frame), cpu_draw_area);

    g_signal_connect (cpu_draw_area, "expose_event",
                      G_CALLBACK (cpu_expose_event), NULL);
    g_signal_connect (cpu_draw_area, "configure_event",
                      G_CALLBACK (cpu_configure_event), NULL);
    //g_signal_connect()参数:发出信号的控件;信号名称;回调函数;传给回调函数的data

    //添加横向盒子用于显示CPU信息
	hbox = gtk_hbox_new (FALSE, 0);
	gtk_widget_show(hbox);
	gtk_box_pack_start (GTK_BOX (vbox), hbox, FALSE, FALSE, 0);

    //总CPU利用率显示
	cpu_lable = gtk_label_new ("CPU ");
	gtk_widget_set_size_request (cpu_lable, 120, 30);
	gtk_widget_show (cpu_lable);
    gtk_box_pack_start (GTK_BOX (hbox), cpu_lable, FALSE, FALSE, 0);

    //分CPU利用率显示
	cpu0_lable = gtk_label_new ("CPU0 ");
	gtk_widget_set_size_request (cpu0_lable, 120, 30);
	gtk_widget_show (cpu0_lable);
    gtk_box_pack_start (GTK_BOX (hbox), cpu0_lable, FALSE, FALSE, 0);

	cpu1_lable = gtk_label_new ("CPU1 ");
	gtk_widget_set_size_request (cpu1_lable, 120, 30);
	gtk_widget_show (cpu1_lable);
    gtk_box_pack_start (GTK_BOX (hbox), cpu1_lable, FALSE, FALSE, 0);

	cpu2_lable = gtk_label_new ("CPU2 ");
	gtk_widget_set_size_request (cpu2_lable, 120, 30);
	gtk_widget_show (cpu2_lable);
    gtk_box_pack_start (GTK_BOX (hbox), cpu2_lable, FALSE, FALSE, 0);

	cpu3_lable = gtk_label_new ("CPU3 ");
	gtk_widget_set_size_request (cpu3_lable, 120, 30);
	gtk_widget_show (cpu3_lable);
    gtk_box_pack_start (GTK_BOX (hbox), cpu3_lable, FALSE, FALSE, 0);


//内存和交换历史

    frame = gtk_frame_new ("Memory");//添加用于显示内存信息的边框构件
    gtk_widget_show (frame);
    gtk_box_pack_start (GTK_BOX (vbox), frame, FALSE, FALSE, 10);

    //内存绘图区域设置
    mem_draw_area = gtk_drawing_area_new ();
    gtk_widget_show (mem_draw_area);
    gtk_drawing_area_size (GTK_DRAWING_AREA (mem_draw_area), 400, 100);//Set DrawArea
    gtk_container_add (GTK_CONTAINER (frame), mem_draw_area);
    g_signal_connect (mem_draw_area, "expose_event",
                      G_CALLBACK (mem_expose_event), NULL);
    g_signal_connect (mem_draw_area, "configure_event",
                      G_CALLBACK (mem_configure_event), NULL);

    //内存信息显示
    mem_label = gtk_label_new ("Memory ");//RAM Label
	gtk_widget_set_size_request (mem_label, 150, 30);
    gtk_widget_show (mem_label);
    gtk_box_pack_start (GTK_BOX (vbox), mem_label, FALSE, FALSE, 0);

    ///交换分区信息显示
    swap_label = gtk_label_new ("Swap ");//Exchange Label
	gtk_widget_set_size_request (swap_label, 150, 30);
    gtk_widget_show (swap_label);
    gtk_box_pack_start (GTK_BOX (vbox), swap_label, FALSE, FALSE, 10);

//网络历史

    //添加hbox排列接收和发送的信息
    hbox = gtk_hbox_new (FALSE, 0);
    gtk_widget_show(hbox);
	gtk_box_pack_start (GTK_BOX (vbox), hbox, FALSE, FALSE, 0);

    receive_label = gtk_label_new("Receiving \n\nTotal Receive ");
    gtk_widget_set_size_request (receive_label, 250, 50);
    gtk_widget_show(receive_label);
    gtk_box_pack_start (GTK_BOX(hbox), receive_label, FALSE, FALSE, 0);

    transmit_label = gtk_label_new("Transmitting \n\nTotal Transmit ");
    gtk_widget_set_size_request (transmit_label, 250, 50);
    gtk_widget_show(transmit_label);
    gtk_box_pack_start (GTK_BOX(hbox), transmit_label, FALSE, FALSE, 0);

    //设置本页的标题
    label = gtk_label_new ("Resource");//Add Title
	gtk_widget_set_size_request (label, 120, 20);
    gtk_widget_show (label);
    //将本页将如打notebook中，是第二页
    gtk_notebook_set_tab_label (GTK_NOTEBOOK (notebook),
                                gtk_notebook_get_nth_page (GTK_NOTEBOOK (notebook), 1), label);
}

void draw_cpu_load_graph ()//CPU使用记录绘图
{
    if (cgraph == NULL)
        return;
    int i;
    gchar buffer[64];
    gint width,height,current,step;

    //显示C总PU信息
    sprintf (buffer, "CPU :%.2f%%",100 * rate);
    gtk_label_set_text(GTK_LABEL(cpu_lable),buffer);

    //显示分CPU信息
    sprintf (buffer, "CPU0 :%.2f%%",cpu_rate[0]);
    gtk_label_set_text(GTK_LABEL(cpu0_lable),buffer);

    sprintf (buffer, "CPU1 :%.2f%%",cpu_rate[1]);
    gtk_label_set_text(GTK_LABEL(cpu1_lable),buffer);

    sprintf (buffer, "CPU2 :%.2f%%",cpu_rate[2]);
    gtk_label_set_text(GTK_LABEL(cpu2_lable),buffer);

    sprintf (buffer, "CPU3 :%.2f%%",cpu_rate[3]);
    gtk_label_set_text(GTK_LABEL(cpu3_lable),buffer);

    //Clear PastGraph
    gdk_draw_rectangle (GDK_DRAWABLE (cgraph), window->style->white_gc, TRUE, 0, 0,
                        cpu_draw_area->allocation.width,
                        cpu_draw_area->allocation.height);
    width = cpu_draw_area->allocation.width;//Get Size
    height = cpu_draw_area->allocation.height;

    current = (int)(rate*(double)height);//Adjust Height
    cpu_graph[DENSITY-1] = height - current;  //Move Direction
    for ( i = 0 ; i < DENSITY - 1 ; i ++)
    {
        cpu_graph[i] = cpu_graph[i+1];
    }

    step = width/DENSITY;//stepsets
    GdkGC *gc = gdk_gc_new(GDK_DRAWABLE(cgraph));

    for (i = DENSITY - 1; i >= 1 ; i -- )
    {
        gdk_draw_line (cgraph, gc, i * step, cpu_graph[i],
                       (i - 1) * step, cpu_graph[i-1]  );
    }

    gtk_widget_queue_draw (cpu_draw_area);   //触发信号,刷新图片的整个区域
}

void draw_mem_load_graph ()
{
    if (mgraph == NULL) return;
    int fd,i;
    gint width,height,current,step;
    gdouble mem_total,mem_available,mem_used;
    gdouble swap_total,swap_free,swap_used;
    gdouble mem_rate,swap_rate;
    gchar s[10];
    gchar buffer[540];
    gchar mem_tmp[19][20];
    gchar *start,*stop,*tmp;
    GString *string;

    //将文件中的信息提取出来
    fd = open ("/proc/meminfo", O_RDONLY);
    read (fd, buffer, sizeof (buffer));
    stop = buffer;
    close(fd);
    for (i = 0; i < 19; i++)
    {
        start = strstr (stop, ":");
        start++;
        stop = strstr (start, "kB");
        stop--;
        tmp = mem_tmp[i];
        while (start != stop) *tmp++ = *start++;
        *tmp = '\0';
    }

    mem_total = atoi (mem_tmp[0]);
    mem_available = atoi (mem_tmp[2]);
    mem_used = mem_total - mem_available;
    mem_rate = mem_used / mem_total;

    swap_total = atoi(mem_tmp[14]);
    swap_free = atoi(mem_tmp[15]);
    swap_used = swap_total - swap_free;
    swap_rate = swap_used / swap_total;

//Memory Label
    string = g_string_new ("");//Refresh info_Num
    g_string_append (string, "Memory  Used: ");
    //按情况选取GB，MB,KB这些单位
    if((mem_used / (1024 * 1024) ) > 1) sprintf (s, "%.1lf GB",(mem_used / (1024 * 1024) ));
    else
    {
        if((mem_used / 1024) > 1) sprintf (s, "%.1lf MB",(mem_used / 1024));
        else sprintf (s, "%.1lf KB",mem_used);
    }
    sprintf (s, " (%.1lf%%), ",mem_rate * 100);
    g_string_append(string, s);

    g_string_append (string, "Total: ");
    if((mem_total / (1024 * 1024) ) > 1) sprintf (s, "%.1lf GB",(mem_total / (1024 * 1024) ));
    else
    {
        if((mem_total / 1024) > 1) sprintf (s, "%.1lf MB",(mem_total / 1024));
        else sprintf (s, "%.1lf KB",mem_total);
    }
    gtk_label_set_text (GTK_LABEL (mem_label), string->str);
    //清空string
    g_string_free (string, TRUE);

//Swap Label
    string = g_string_new ("");//Refresh info_Num
    g_string_append (string, "Swap       Used: ");
    //按情况选取GB，MB，KB单位
    if((swap_used / (1024 * 1024) ) > 1) sprintf (s, "%.1lf GB",(swap_used / (1024 * 1024) ));
    else
    {
        if((swap_used / 1024) > 1) sprintf (s, "%.1lf MB",(swap_used / 1024));
        else sprintf (s, "%.1lf KB",swap_used);
    }
    sprintf (s, " (%.1lf%%), ",swap_rate * 100);

    g_string_append (string, "Total: ");
    if((swap_total / (1024 * 1024) ) > 1) sprintf (s, "%.1lf GB",(swap_total / (1024 * 1024) ));
    else
    {
        if((swap_total / 1024) > 1) sprintf (s, "%.1lf MB",(swap_total / 1024));
        else  sprintf (s, "%.1lf KB",swap_total);
    }
    gtk_label_set_text (GTK_LABEL (swap_label), string->str);
    g_string_free (string, TRUE);

    //ClearGraph
    gdk_draw_rectangle (mgraph, window->style->white_gc, TRUE, 0, 0,
                        mem_draw_area->allocation.width,
                        mem_draw_area->allocation.height);

    width = mem_draw_area->allocation.width;//GetSize
    height = mem_draw_area->allocation.height;
    current = ((float)(mem_total-mem_available)/(float)mem_total) * height;//Adjust Size
    mem_graph[DENSITY-1] = height - current; //Move Dir
    for ( i = 0 ; i < DENSITY - 1 ; i ++)
    {
        mem_graph[i] = mem_graph[i+1];
    }
    step = width / DENSITY;

    GdkGC *gc = gdk_gc_new(GDK_DRAWABLE(mgraph));

    for (i = DENSITY - 1; i >= 1 ; i -- )
    {
        gdk_draw_line (mgraph, gc, i * step,
                       mem_graph[i], (i - 1) * step, mem_graph[i-1]);
    }
    gtk_widget_queue_draw ( mem_draw_area);
}

gboolean cpu_configure_event (GtkWidget *widget, GdkEventConfigure *event, gpointer data)
{
    if (cgraph)
    {
        g_object_unref (cgraph); //减少引用计数
    }

    //Pixmap Create
    cgraph = gdk_pixmap_new (widget->window,
                             widget->allocation.width, widget->allocation.height,-1);

    //RePaint in Pixmap
    gdk_draw_rectangle (cgraph, widget->style->white_gc, TRUE, 0, 0,
                        widget->allocation.width, widget->allocation.height);
    return TRUE;
}

gboolean cpu_expose_event (GtkWidget *widget, GdkEventExpose *event, gpointer data)
{
    gdk_draw_drawable (widget->window,
                       widget->style->fg_gc[GTK_WIDGET_STATE (widget)],
                       cgraph,
                       event->area.x, event->area.y,
                       event->area.x, event->area.y,
                       event->area.width, event->area.height);
    return TRUE;
}


//Graph Refreshing
gboolean mem_configure_event (GtkWidget *widget, GdkEventConfigure *event, gpointer data)
{
    if (mgraph)
    {
        g_object_unref (mgraph);
    }

    mgraph = gdk_pixmap_new (widget->window,
                             widget->allocation.width, widget->allocation.height,-1);

    gdk_draw_rectangle (mgraph, widget->style->white_gc, TRUE, 0, 0,
                        widget->allocation.width, widget->allocation.height);
    return TRUE;
}

gboolean mem_expose_event (GtkWidget *widget, GdkEventExpose *event, gpointer data)
{
    gdk_draw_drawable (widget->window,
                       widget->style->fg_gc[GTK_WIDGET_STATE (widget)],
                       mgraph,
                       event->area.x, event->area.y,
                       event->area.x, event->area.y,
                       event->area.width, event->area.height);
    return TRUE;
}

void get_network_info()
{
    FILE *fp;
    gint receive[8];
    static gint receive_old;
    gint receive_true;
    gint transmit[8];
    static gint transmit_old;
    gint transmit_true;
    gchar Interface[10];
    gchar buffer[256];
    gchar insert[10];
    GString *string;

    //打开文件后读取第四行信息
    fp=fopen("/proc/net/dev","r");
    fgets(buffer,sizeof(buffer),fp);
    fgets(buffer,sizeof(buffer),fp);
    fgets(buffer,sizeof(buffer),fp);
    fgets(buffer,sizeof(buffer),fp); 
    fclose(fp);

    //提取出第四行的相关信息
    sscanf(buffer,"%s%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d",
    Interface,&receive[0],&receive[1],&receive[2],&receive[3],
              &receive[4],&receive[5],&receive[6],&receive[7],
    &transmit[0],&transmit[1],&transmit[2],&transmit[3],
    &transmit[4],&transmit[5],&transmit[6],&transmit[7]);

    // printf("%s %d %d %d %d\n",Interface,receive[0],receive[1],transmit[0],transmit[1]);

//Receive
    receive_true = receive[1] - receive_old;
    string = g_string_new ("");//Refresh info_Num
    g_string_append (string, "Receiving : ");
    if((receive_true / (1024*1024*1024)) > 1) sprintf (insert, "%.1fGB/s",((float)receive_true / (1024*1024*1024)));
    else
    {
        if((receive_true / (1024*1024)) > 1) sprintf (insert, "%.1fMB/s",((float)receive_true / (1024*1024)));
        else 
        {
            if((receive_true / (1024)) > 1) sprintf (insert, "%.1fKB/s",((float)receive_true / (1024)));
            else  sprintf (insert, "%.0fB/s",(float)receive_true); 
        }
    }

    g_string_append (string, insert);
    g_string_append_c (string, '\n');
    g_string_append_c (string, '\n');
    g_string_append (string, "Total Receive : ");

    if((receive[0] / (1024*1024*1024)) > 1) sprintf (insert, "%.1fGB",((float)receive[0] / (1024*1024*1024)));
    else
    {
        if((receive[0] / (1024*1024)) > 1) sprintf (insert, "%.1fMB",((float)receive[0] / (1024*1024)));
        else 
        {
            if((receive[0] / (1024)) > 1) sprintf (insert, "%.1fKB",((float)receive[0] / (1024)));
            else  sprintf (insert, "%.0fB",((float)receive[0])); 
        }
    }
    g_string_append (string, insert);
    gtk_label_set_text (GTK_LABEL (receive_label), string->str);
    g_string_free (string, TRUE);
    

//Trasmit 
    transmit_true = transmit[1] - transmit_old;
    string = g_string_new ("");//Refresh info_Num
    g_string_append (string, "Transmitting : ");
    if((transmit_true / (1024*1024*1024)) > 1) sprintf (insert, "%.1fGB/s",((float)transmit_true / (1024*1024*1024)));
    else
    {
        if((transmit_true / (1024*1024)) > 1) sprintf (insert, "%.1fMB/s",((float)transmit_true / (1024*1024)));
        else 
        {
            if((transmit_true / (1024)) > 1) sprintf (insert, "%.1fKB/s",((float)transmit_true / (1024)));
            else  sprintf (insert, "%.0fB/s",((float)transmit_true)); 
        }
    }

    g_string_append (string, insert);
    g_string_append_c (string, '\n');
    g_string_append_c (string, '\n');
    g_string_append (string, "Total Transmit : ");

    if((transmit[0] / (1024*1024*1024)) > 1) sprintf (insert, "%.1fGB",((float)transmit[0] / (1024*1024*1024)));
    else
    {
        if((transmit[0] / (1024*1024)) > 1) sprintf (insert, "%.1fMB",((float)transmit[0] / (1024*1024)));
        else 
        {
            if((transmit[0] / (1024)) > 1) sprintf (insert, "%.1fKB",((float)transmit[0] / (1024)));
            else  sprintf (insert, "%.0fB",((float)transmit[0])); 
        }
    }

    g_string_append (string, insert);
    gtk_label_set_text (GTK_LABEL (transmit_label), string->str);
    g_string_free (string, TRUE);
    
    receive_old = receive[1];
    transmit_old = transmit[1];
}

