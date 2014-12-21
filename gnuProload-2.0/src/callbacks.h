#include <gtk/gtk.h>


/* For now put all 'user-defined' stuff in this file.*/
//#define GNU_PROLOAD_LOGGING_ALL   //Uncomment if you need logging

#ifdef GNU_PROLOAD_LOGGING_ALL
#define GNU_PROLOAD_LOG_ERROR
#define GNU_PROLOAD_LOG_INFO
#define GNU_PROLOAD_LOG_WARNING
#define GNU_PROLOAD_LOG_DEBUG
#define GNU_PROLOAD_LOG_FUNC
#else
#undef GNU_PROLOAD_LOG_ERROR
#undef GNU_PROLOAD_LOG_INFO
#undef GNU_PROLOAD_LOG_WARNING
#undef GNU_PROLOAD_LOG_DEBUG
#undef GNU_PROLOAD_LOG_FUNC
#endif /* GNU_PROLOAD_LOGGING_ALL */

#ifdef GNU_PROLOAD_LOG_ERROR
#define LOG_ERROR(x)    g_log("gnuProload", G_LOG_LEVEL_ERROR, x);
#else
#define LOG_ERROR(x)
#endif /* GNU_PROLOAD_LOG_ERROR */

#ifdef GNU_PROLOAD_LOG_INFO
#define LOG_INFO(x)  g_log("gnuProload", G_LOG_LEVEL_INFO, x);
#else
#define LOG_INFO(x)
#endif /* GNU_PROLOAD_LOG_INFO */


#ifdef GNU_PROLOAD_LOG_WARNING
#define LOG_WARNING(x)  g_log("gnuProload", G_LOG_LEVEL_WARNING, x);
#else
#define LOG_WARNING(x)
#endif /* GNU_PROLOAD_LOG_WARNING */

#ifdef GNU_PROLOAD_LOG_DEBUG
#define LOG_DEBUG(x)    g_log("gnuProload", G_LOG_LEVEL_DEBUG, x);
#define LOG_INFO_DATA(x) g_debug x;
#else
#define LOG_DEBUG(x)
#define LOG_INFO_DATA(x)
#endif /* GNU_PROLOAD_LOG_DEBUG */

#ifdef GNU_PROLOAD_LOG_FUNC
#define LOG_ENTER()    g_log("gnuProload", G_LOG_LEVEL_INFO,"Entering: %s()", __func__);
#define LOG_LEAVE()    g_log("gnuProload", G_LOG_LEVEL_INFO,"Leaving: %s()", __func__);
#else
#define LOG_ENTER()
#define LOG_LEAVE()
#endif /* GNU_PROLOAD_LOG_FUNC */


void
on_load_button_clicked                 (GtkButton       *button,
                                        gpointer         user_data);

void
on_burn_button_clicked                 (GtkButton       *button,
                                        gpointer         user_data);

void
on_erase_button_clicked                (GtkButton       *button,
                                        gpointer         user_data);

void
on_quit_button_clicked                 (GtkButton       *button,
                                        gpointer         user_data);

void
on_filechooser_cancel_clicked          (GtkButton       *button,
                                        gpointer         user_data);

void
on_filechooser_open_clicked            (GtkButton       *button,
                                        gpointer         user_data);

void
on_error_ok_button_clicked             (GtkButton       *button,
                                        gpointer         user_data);

void
on_main_window_destroy                 (GtkObject       *object,
                                        gpointer         user_data);

gboolean
on_main_window_delete_event            (GtkWidget       *widget,
                                        GdkEvent        *event,
                                        gpointer         user_data);

void
on_main_window_destroy                 (GtkObject       *object,
                                        gpointer         user_data);

gboolean
on_filechooserdialog1_delete_event     (GtkWidget       *widget,
                                        GdkEvent        *event,
                                        gpointer         user_data);

void
on_filechooserdialog1_destroy          (GtkObject       *object,
                                        gpointer         user_data);

gboolean
on_popup_dialog_delete_event           (GtkWidget       *widget,
                                        GdkEvent        *event,
                                        gpointer         user_data);

void
on_popup_dialog_destroy                (GtkObject       *object,
                                        gpointer         user_data);



void
on_load1_activate                      (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_quit1_activate                      (GtkMenuItem     *menuitem,
                                        gpointer         user_data);


void
on_copy1_activate                      (GtkMenuItem     *menuitem,
                                        gpointer         user_data);



void
on_about1_activate                     (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_copy1_activate                      (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_burn1_activate                      (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_load1_activate                      (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_burn1_activate                      (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_quit1_activate                      (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_copy1_activate                      (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_about1_activate                     (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_erase1_activate                     (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_quick_start1_activate               (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_about1_activate                     (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_quick_start1_activate               (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_okbutton1_clicked                   (GtkButton       *button,
                                        gpointer         user_data);

gboolean
on_okbutton1_delete_event              (GtkWidget       *widget,
                                        GdkEvent        *event,
                                        gpointer         user_data);

gboolean
on_dialog1_delete_event                (GtkWidget       *widget,
                                        GdkEvent        *event,
                                        gpointer         user_data);

void
on_okbutton1_clicked                   (GtkButton       *button,
                                        gpointer         user_data);

void
on_aboutdialog1_response               (GtkDialog       *dialog,
                                        gint             response_id,
                                        gpointer         user_data);
