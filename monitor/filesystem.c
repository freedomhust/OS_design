#include "monitor.h"

void create_filesystem_page()
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
        column = gtk_tree_view_column_new_with_attributes (col_name[i],
                 renderer,"text",i,NULL);//新建一列
        gtk_tree_view_append_column (GTK_TREE_VIEW (mtree_view), column);//将该列加到树中 
    }

    get_filesystem_info(memory_store);

    //为本页添加标题
    label = gtk_label_new ("file system");
    gtk_widget_set_size_request (label, 120, 20);
    gtk_widget_show (label);
    //将本页添加到notebook中，是第三页
    gtk_notebook_set_tab_label (GTK_NOTEBOOK (notebook),
                                gtk_notebook_get_nth_page (GTK_NOTEBOOK (notebook),2), label);
}

void get_filesystem_info (GtkListStore *store)
{
    GtkTreeIter iter;
    FILE *fp;
    gint i;
    gchar s[1024];
    gchar *mem[9];
    gdouble mem_rate;
    gchar dev_rate[15];
    struct statfs diskInfo; 
    unsigned long long blocksize;
    unsigned long long totalsize;
    gchar Total[15];
    // unsigned long long freeDisk;
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
            //筛选出设备号
            if(s[0] == '/'){
                mem[0] = strtok (s," ");
                for(i = 1; i < 9; i++) mem[i] = strtok(NULL," ");
                statfs(mem[1],&diskInfo);
                // printf("%s\n",mem[0]);
                blocksize = diskInfo.f_bsize; //每个block里包含的字节数 
                totalsize = blocksize * diskInfo.f_blocks; //总的字节数，f_blocks为block的数目

                if((totalsize>>20) < 1024)   sprintf(Total,"%5lluMB", totalsize>>20);
                else sprintf(Total , "%5lluGB", totalsize>>30);


                availableDisk = diskInfo.f_bavail * blocksize; //可用空间大小
                if((availableDisk>>20) < 1024)   sprintf(available,"%5lluMB", availableDisk>>20);
                else sprintf(available , "%5lluGB", availableDisk>>30);


                usedDisk = totalsize - availableDisk;

                if((usedDisk>>20) < 1024)   sprintf(used,"%5lluMB", usedDisk>>20);
                else sprintf(used , "%5lluGB", usedDisk>>30);

                mem_rate = (double)usedDisk / totalsize;
                sprintf (dev_rate, "%.2f%%",100 * mem_rate);

            gtk_list_store_append (store, &iter);//增加到列表
            gtk_list_store_set (store, &iter,
                                DEV_COLUMN,mem[0],
                                DIR_COLUMN,mem[1],
                                TYPE_COLUMN,mem[2],
                                TOTAL_COLUMN,Total,
                                USED_COLUMN,used,
                                FREE_COLUMN,available,
                                RATE_COLUMN,dev_rate,
                                -1);
            
            }
        }
    }
}