#include <gtk/gtk.h>
 
// "switch-page"信号处理函数，page_num指第几个页面，从0开始
void deal_switch_pape(GtkNotebook *notebook, gpointer page,
				guint page_num, gpointer user_data)
{
	printf("我是第%d个页面\n", page_num+1);
}
 
int main( int argc, char *argv[] ) 
{ 
	gtk_init (&argc, &argv); 	// 初始化
	 
	GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL); // 创建主窗口
	gtk_widget_set_size_request(window, 400, 300);	//设置窗口大小
	 
	GtkWidget *notebook = gtk_notebook_new(); 	// 创建笔记本控件
	gtk_container_add(GTK_CONTAINER(window), notebook); // 笔记本放进窗口
 
	// 页标签的位置，可以有四种位置：上、下、左或右。
	gtk_notebook_set_tab_pos(GTK_NOTEBOOK (notebook), GTK_POS_TOP);  //上面
 
 
	// 第一个页面
	GtkWidget *label = gtk_label_new("Page one");	//创建标签
	GtkWidget *box = gtk_hbox_new(TRUE, 5);	// 创建布局
	GtkWidget *button_one = gtk_button_new_with_label("我是第一个页面");
	GtkWidget *button_two = gtk_button_new_with_label("我是第一个页面的按钮");
	gtk_container_add(GTK_CONTAINER(box), button_one);
	gtk_container_add(GTK_CONTAINER(box), button_two);
 
	/* gtk_notebook_append_page( GtkNotebook *notebook, GtkWidget *child, 
	 *                     GtkWidget  *tab_label );
	 * 插入页面到笔记本的后端（append）或前端（prepend）来添加页面。
	 * child是放在笔记本页面里的子控件，它必须另外创建，一般是容器
	 * tab_label是要添加的页面的标签。
	 */
	gtk_notebook_append_page(GTK_NOTEBOOK(notebook), box, label); 
 
	// 第二个页面
	label = gtk_label_new("Page two");	//创建标签
	box = gtk_hbox_new(TRUE, 5);	// 创建布局
	button_one = gtk_button_new_with_label("我是第二个页面");
	button_two = gtk_button_new_with_label("我是第二个页面的按钮");
	gtk_container_add(GTK_CONTAINER(box), button_one);
	gtk_container_add(GTK_CONTAINER(box), button_two);
	gtk_notebook_append_page(GTK_NOTEBOOK(notebook), box, label); 
 
	// 第三个页面，在指定位置添加页面，从0开始
	button_one = gtk_button_new_with_label("我是通过insert_page实现的，第三个页面");
	label = gtk_label_new("Page three"); 
	gtk_notebook_insert_page(GTK_NOTEBOOK(notebook), button_one, label, 2); 
	
	// 处理信号，当切换页面时，会触发"switch-page"信号
	g_signal_connect(notebook, "switch-page",
					G_CALLBACK(deal_switch_pape), NULL);
	
	gtk_widget_show_all(window); //显示所有控件
 
	// 设置起始页(第2页)，从0开始算，相当于设置第2个页面显示
	gtk_notebook_set_current_page(GTK_NOTEBOOK(notebook), 1); 
	 
	gtk_main(); 		// 主事件循环
	 
	return 0; 
} 
