#include <gtk/gtk.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define _DEBUG_ 1
#define CK_TIME 1

gint systime(gpointer label);

void destory_progress(GtkWidget *widget){
    gtk_main_quit();
}

int main(void){
    time_t t;
    time(&t);
    GtkWidget *window;
    GtkWidget *vbox;
    GtkWidget *button;
    GtkWidget *label;
    gpointer data;
    gtk_init(NULL,NULL);
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_resizable(GTK_WINDOW(window),TRUE);
    gtk_window_set_position(GTK_WINDOW(window),GTK_WIN_POS_NONE);
    g_signal_connect(G_OBJECT(window),"destory",G_CALLBACK(destory_progress),NULL);
    gtk_window_set_title(GTK_WINDOW(window),"系统时间");
    gtk_container_set_border_width(GTK_CONTAINER(window),20);
    vbox = gtk_vbox_new(FALSE,10);
    gtk_container_border_width(GTK_CONTAINER(vbox),100);
    gtk_container_add(GTK_CONTAINER(window),vbox);
    gtk_widget_show(vbox);
    label = gtk_label_new(ctime(&t));
    gtk_box_pack_start(GTK_BOX(vbox),label,FALSE,FALSE,10);
    gtk_widget_show(label);
    gtk_widget_show(window);
    g_timeout_add(1000,systime,(gpointer)label);
    gtk_main();
}

gint systime (gpointer label){
    time_t t;
    time(&t);
    char string[100];
    sprintf(string,"%s",ctime(&t));
    gtk_label_set_text(label,string);
}