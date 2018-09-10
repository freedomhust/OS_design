#include <gtk/gtk.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define _DEBUG_ 1
#define CK_TIME 1

gint sum(gpointer label);
int n = 0, i = 1;


void destroy_window (GtkWidget *widget, gpointer data);

int main(int argc, char **argv){
    GtkWidget *window;
    GtkWidget *vbox;
    GtkWidget *button;
    GtkWidget *label;
    gpointer data;

    gtk_set_locale(); 
    //初始化gtk库
    gtk_init (&argc, &argv);
    //新建一个顶层窗口
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    //使窗口获得伸缩功能
    gtk_window_set_resizable(GTK_WINDOW(window),TRUE);
    //窗口显示位置
    gtk_window_set_position(GTK_WINDOW(window),GTK_WIN_POS_NONE);
    //退出时调用的函数
    g_signal_connect(G_OBJECT(window),"delete_event",G_CALLBACK(destroy_window),NULL);
    //窗口标题
    gtk_window_set_title(GTK_WINDOW(window),"累加求和");
    //容器，这里指窗口的边框宽度
    gtk_container_set_border_width(GTK_CONTAINER(window),20);
    //新的纵向容器
    vbox = gtk_vbox_new(FALSE,10);
    //纵向容器的边框宽度
    gtk_container_border_width(GTK_CONTAINER(vbox),100);
    //将纵向容器控件加入到窗口控件中去
    gtk_container_add(GTK_CONTAINER(window),vbox);
    //使vbox能够被显示
    gtk_widget_show(vbox);
    //标签
    label = gtk_label_new("累加求和");
    //在vbox中从前往后进行排列,不随窗口拉伸而改变大小
    gtk_box_pack_start(GTK_BOX(vbox),label,FALSE,FALSE,10);
    //显示标签
    gtk_widget_show(label);
    //每隔1000ms进行一次刷新，运行一次sum函数
    g_timeout_add(1000,sum,(gpointer) label);
    //显示窗口
    gtk_widget_show(window);
    //初始化
    gtk_main();
    return 0;
}

gint sum(gpointer label){
    char string[50];
    n += i++;
    sprintf(string,"累加到%d的和为%d",i-1,n);
    gtk_label_set_text(label,string);
    if(i > 100){
        i = 1; n = 0;
    }
}

void destroy_window (GtkWidget *widget, gpointer data)//退出窗口管理程序
{
    gtk_main_quit ();
}