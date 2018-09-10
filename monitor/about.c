#include "monitor.h"

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