#ifndef _HMONITOR_H_
#define _HMONITOR_H_

#include <fcntl.h>
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
#include <pwd.h>
#include <sys/types.h>


#define DENSITY 100         //图形点密度
#define PMAX    100         //最大进程数目

enum
{
    NAME_COLUMN,   //进程名列
    USER_COLUMN,   //用户列
    CPU_COLUMN,    //CPU信息列
    PID_COLUMN,    //进程PID列
    STATUS_COLUMN, //进程状态列
    MEMORY_COLUMN, //内存信息列
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
GtkWidget *resource_hbox;
GtkWidget *label;     //标签
GtkWidget *frame;     //框架
GtkWidget *window;    //窗口
GtkWidget *menubar;   //菜单栏
GtkWidget *notebook;  //笔记本
GtkWidget *main_vbox; 
GtkWidget *button_one;
GtkWidget *button_two;
GtkWidget *box;

extern GtkWidget *mem_label;
extern GtkWidget *swap_label;

extern GtkWidget *cpu_lable;
extern GtkWidget *cpu0_lable;
extern GtkWidget *cpu1_lable;
extern GtkWidget *cpu2_lable;
extern GtkWidget *cpu3_lable;

extern GtkWidget *receive_label;
extern GtkWidget *transmit_label;

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
extern gdouble rate;                           //cpu Use_Rate
extern gdouble cpu_rate[4];                    //分cpu利用率
extern gdouble total;                          //当前进程总数
GString   *info;

extern GdkPixmap *cgraph;                   //pixmap_Pointer_CPU
extern GdkPixmap *mgraph;                   //pixmap_Pointer_RAM

//Others
GtkListStore *process_store;
GtkListStore *memory_store;
GtkCellRenderer *renderer;//用于显示tree view中的每个列标题
GtkTreeViewColumn *column;//tree view控件列数

void create_process_page();
void create_resource_page();
void create_sysinfo_page();
void create_filesystem_page();

void get_process_info (GtkListStore *store);
void get_cpu_info(GString *string);
void get_os_info (GString *string);
void get_filesystem_info (GtkListStore *store);

void destroy_window (GtkWidget *, gpointer);

void prefresh_button_clicked (gpointer data);
void pkill_button_clicked(gpointer data);

gboolean handle_timeout (gpointer data);
gboolean get_cpu_rate (gpointer data);
gboolean load_graph_refresh (GtkWidget *widget);

void show_dialog (gchar *, gchar *);

void draw_cpu_load_graph ();
void draw_mem_load_graph ();
void get_network_info ();

gboolean cpu_configure_event (GtkWidget *widget, GdkEventConfigure *event, gpointer data);
gboolean cpu_expose_event (GtkWidget *widget, GdkEventExpose *event, gpointer data);
gboolean mem_configure_event (GtkWidget *widget, GdkEventConfigure *event, gpointer data);
gboolean mem_expose_event (GtkWidget *widget, GdkEventExpose *event, gpointer data);

#endif 

