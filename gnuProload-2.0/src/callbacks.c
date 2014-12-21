#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <gtk/gtk.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <errno.h>

#include "callbacks.h"
#include "interface.h"
#include "support.h"

static gboolean busy = FALSE;

/* To enable access to the global window ptr */
extern GtkWidget *gmain_window_ptr;
/* This is where our logo is defined as raw C-source */
extern const guint8 my_pixbuf[];
extern int serial_fd;
extern int load_internal_buffer_with_hex_file(const char *filename);
extern void burn_hex_file(gpointer filename);
extern void verify_hex_file();
extern void erase_micro();
extern int set_serial();
extern void reset_serial();
extern void cleanup();
extern int check_connection();
extern void popup_error(const gchar *mesg);
extern gpointer burn_thread(gpointer filename);
extern void deactivate_buttons();
extern void reactivate_buttons();


void
on_load_button_clicked(GtkButton *button, gpointer user_data)
{
	GtkEntry *textentry_widget = NULL;
	GtkWidget *filechooser = NULL;
	GdkPixbuf *pixbuf = NULL;
	const gchar *filename = NULL;
	struct stat stat_buf;
	LOG_ENTER();
    if(!busy)
    {
		LOG_INFO(("Not busy...proceeding with loading file..."));
        /* First deactivate this button and also the menu item */
        deactivate_buttons();
        /* We need to create the filechooser dialog here */
        filechooser = create_filechooserdialog1();

        /* Set icon for About dialog here */
        pixbuf = gdk_pixbuf_new_from_inline (-1, my_pixbuf, FALSE, NULL);	
        gtk_window_set_icon(GTK_WINDOW(filechooser), pixbuf);

        /* Get the text entry field with this filename*/
        textentry_widget = GTK_ENTRY(lookup_widget(GTK_WIDGET(gmain_window_ptr),"entry1"));   
        /* Extract filename from the text entry widget */
        filename = gtk_entry_get_text(textentry_widget);
        if(filename != NULL)
        {
            /* Set the default opening directory, only if a valid directory is 
               preset within the text entry field. Otherwise, don't do anything.
             */
            if(!(stat(filename, &stat_buf)) && S_ISDIR(stat_buf.st_mode))
            {
                gtk_file_chooser_set_current_folder(GTK_FILE_CHOOSER(filechooser), filename);
            }
        }
        gtk_widget_show(filechooser);
    }
	LOG_LEAVE();
}
 

void
on_burn_button_clicked(GtkButton *button, gpointer  user_data)
{
    GtkEntry *textentry_widget = NULL;
	const gchar *filename = NULL;
	struct stat stat_buf;
	LOG_ENTER();
	if(!busy)	
    {	
		LOG_INFO(("Not busy...proceeding with 'Burning' file..."));
        /* Get the text entry field with this filename*/
        textentry_widget = GTK_ENTRY(lookup_widget(GTK_WIDGET(gmain_window_ptr),"entry1"));   

        /* Extract filename from the text entry widget */
        filename = gtk_entry_get_text(textentry_widget);
        if(strlen(filename)== 0)
        {
            /* popup error and bypass everything else in this function */
            popup_error("No filename entered or loaded!");
            return;
        }
        else
        {
            /* Check if file exists */
            if(stat(filename, &stat_buf) < 0)
            {
                popup_error(strerror(errno));
                return;
            }
            else
            {
                /* Check if file is a regular file */
                if(S_ISREG(stat_buf.st_mode))
                { 
                    busy = TRUE;
                    burn_thread((gpointer)filename);
                    while (gtk_events_pending ())
                    {
                        gtk_main_iteration ();
                    }
                    busy = FALSE;
                }  
                else
                {
                    popup_error("Not a regular file. Try again!");
                    return;
                }				
            }
        }	 
    }
	LOG_LEAVE();
}


void
on_erase_button_clicked(GtkButton *button, gpointer user_data)
{
	LOG_ENTER();
	if(!busy)
    {
		LOG_INFO(("Not busy...proceeding with 'Erasing' file..."));
        busy = TRUE;
        /* This is necessary so that the 'Erase' menu item does not appear frozen */
        while (gtk_events_pending ())
        {
            gtk_main_iteration ();
        }
        if((check_connection() == TRUE) && (set_serial() == TRUE))
        {
            erase_micro();
            reset_serial();
        }
        busy = FALSE;
    }
	LOG_LEAVE();
}

void
on_quit_button_clicked(GtkButton *button, gpointer user_data)
{
	LOG_ENTER();
	if(!busy)
    {
		LOG_INFO(("Not busy...quitting..."));
        cleanup();
        gtk_main_quit();
    }
	LOG_LEAVE();
}


void
on_filechooser_cancel_clicked(GtkButton *button, gpointer user_data)
{
	LOG_ENTER();
	LOG_INFO(("Cancel pressed in File Chooser..."));
    GtkFileChooser *filechooser_widget = NULL;
	/* First get the GtkWidget pointer for the file selection widget */
    filechooser_widget = GTK_FILE_CHOOSER(lookup_widget(GTK_WIDGET(button), "filechooserdialog1"));	
	gtk_widget_destroy(GTK_WIDGET(filechooser_widget));
	reactivate_buttons();
	LOG_LEAVE();
}


void
on_filechooser_open_clicked(GtkButton *button, gpointer user_data)
{
    GtkFileChooser *filechooser_widget = NULL;
    GtkEntry *textentry_widget = NULL;
	LOG_ENTER();
	LOG_INFO(("File loaded from File Chooser..."));
    /* First get the GtkWidget pointer for the file selection widget */
    filechooser_widget = GTK_FILE_CHOOSER(lookup_widget(GTK_WIDGET(button), "filechooserdialog1"));	

    /* Extract filename from the file selection dialog */
    const gchar *filename = gtk_file_chooser_get_filename(filechooser_widget);
    
    /* Set the text entry field with this filename*/
    textentry_widget = GTK_ENTRY(lookup_widget(GTK_WIDGET(gmain_window_ptr),"entry1"));   
    gtk_entry_set_text (textentry_widget, filename);
	
    gtk_widget_destroy(GTK_WIDGET(filechooser_widget)); 
	reactivate_buttons();
	LOG_LEAVE();
}

void
on_error_ok_button_clicked(GtkButton *button, gpointer user_data)
{
	LOG_ENTER();
	LOG_INFO(("Ok pressed in pop error..."));
	GtkWidget *popup = lookup_widget(GTK_WIDGET(button), "popup_dialog");
    gtk_widget_destroy(popup);  
	LOG_LEAVE();
}

gboolean
on_main_window_delete_event            (GtkWidget       *widget,
                                        GdkEvent        *event,
                                        gpointer         user_data)
{
    LOG_ENTER();
    return FALSE;
    LOG_LEAVE();
}


void
on_main_window_destroy                 (GtkObject       *object,
                                        gpointer         user_data)
{
	LOG_ENTER();
	LOG_INFO(("Hit main window destroy handler!"));
	cleanup();
	gtk_main_quit();
	LOG_LEAVE();
}

 
gboolean
on_filechooserdialog1_delete_event     (GtkWidget       *widget,
                                        GdkEvent        *event,
                                        gpointer         user_data)
{
  LOG_ENTER();
  return FALSE;
  LOG_LEAVE();
}


void
on_filechooserdialog1_destroy          (GtkObject       *object,
                                        gpointer         user_data)
{
	LOG_ENTER();
	LOG_INFO(("Hit filechooser destroy handler..."));
    gtk_widget_destroy(GTK_WIDGET(object));
	reactivate_buttons();
	LOG_LEAVE();
}

gboolean
on_popup_dialog_delete_event           (GtkWidget       *widget,
                                        GdkEvent        *event,
                                        gpointer         user_data)
{
  LOG_ENTER();
  return FALSE;
  LOG_LEAVE();
}


void
on_popup_dialog_destroy                (GtkObject       *object,
                                        gpointer         user_data)
{
	LOG_ENTER();
    LOG_INFO(("Hit pop up window destroy handler..."));
    gtk_widget_destroy(GTK_WIDGET(object));
	LOG_LEAVE();
}




void
on_load1_activate                       (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
    GtkButton *button = NULL;
	LOG_ENTER();
	LOG_INFO(("Load option selected from 'File' menu..."));
	button = GTK_BUTTON(lookup_widget(GTK_WIDGET(menuitem), "load_button"));
	on_load_button_clicked(button, NULL);
	LOG_LEAVE();
}


void
on_quit1_activate                      (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
    GtkButton *button = NULL;
	LOG_ENTER();
	LOG_INFO(("Quit option selected from 'File' menu..."));
	button = GTK_BUTTON(lookup_widget(GTK_WIDGET(menuitem), "quit_button"));
	on_quit_button_clicked(button, NULL);
	LOG_LEAVE();
}




void
on_copy1_activate                      (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
    GtkClipboard   *clip_board = NULL;
	GtkTextView *textview_widget = NULL;
	GtkTextBuffer *buffer = NULL;
	GtkTextIter start_iter, end_iter;
	gchar *text = NULL;
	
	LOG_INFO(("Copy option selected from 'Edit' menu..."));
	textview_widget = GTK_TEXT_VIEW(lookup_widget(gmain_window_ptr, "textview1"));
	buffer = gtk_text_view_get_buffer(textview_widget);
	gtk_text_buffer_get_start_iter(buffer,&start_iter);
	gtk_text_buffer_get_end_iter(buffer,&end_iter);
	text = gtk_text_buffer_get_text(buffer, &start_iter, &end_iter, TRUE);
	
	/*Copy to Proper Clipboard - Ctrl-V or 'Paste' from Menu in any editor*/
	clip_board = gtk_clipboard_get(GDK_SELECTION_CLIPBOARD);
	gtk_clipboard_set_text(clip_board, text, -1);
	
	/*Copy to generic selectio buffer, - middle mouse click in X */
	clip_board = gtk_clipboard_get(GDK_SELECTION_PRIMARY);
	gtk_clipboard_set_text(clip_board, text, -1);
}


void
on_about1_activate                     (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
    GtkWidget *about = NULL;
	GdkPixbuf *pixbuf = NULL;	
	LOG_INFO(("Showing about dialog..."));
	about = create_aboutdialog1();
	/* Set icon for About dialog here */
    pixbuf = gdk_pixbuf_new_from_inline (-1, my_pixbuf, FALSE, NULL);	
    gtk_window_set_icon(GTK_WINDOW(about), pixbuf);
	/* Set logo for About dialog here */
	gtk_about_dialog_set_logo(GTK_ABOUT_DIALOG(about), pixbuf);
	gtk_widget_show(about);
}

void
on_burn1_activate                      (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
    GtkButton *button = NULL;
	LOG_INFO(("Burn option selected from 'File' menu..."));
	button = GTK_BUTTON(lookup_widget(GTK_WIDGET(menuitem), "burn_button"));
	on_burn_button_clicked(button, NULL);
}

void
on_erase1_activate                     (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
    GtkButton *button = NULL;
	LOG_INFO(("Erase option selected from 'File' menu..."));
	button = GTK_BUTTON(lookup_widget(GTK_WIDGET(menuitem), "erase_button"));
	on_erase_button_clicked(button, NULL);
}

void
on_quick_start1_activate               (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
    GtkWidget *quickstart = NULL;
	GdkPixbuf *pixbuf = NULL;	
	LOG_ENTER();
	LOG_INFO(("QuickStart option selected from 'Help' menu..."));
	quickstart = create_dialog1();
	/* Set icon for About dialog here */
    pixbuf = gdk_pixbuf_new_from_inline (-1, my_pixbuf, FALSE, NULL);	
    gtk_window_set_icon(GTK_WINDOW(quickstart), pixbuf);
	gtk_widget_show(quickstart);
	LOG_LEAVE();
}

void
on_okbutton1_clicked                   (GtkButton       *button,
                                        gpointer         user_data)
{
	GtkWidget *dialog = NULL;
	LOG_INFO((__FUNCTION__));
	LOG_INFO(("User clicked 'OK' in About dialog...\n"));
    /* First get the GtkWidget pointer for the file selection widget */
    dialog = lookup_widget(GTK_WIDGET(button), "dialog1");	
	gtk_widget_destroy(dialog);
}



gboolean
on_dialog1_delete_event                (GtkWidget       *widget,
                                        GdkEvent        *event,
                                        gpointer         user_data)
{
  return FALSE;
}

void
on_aboutdialog1_response               (GtkDialog       *dialog,
                                        gint             response_id,
                                        gpointer         user_data)
{
    if(response_id == GTK_RESPONSE_CANCEL)
	{
		// Means user has pressed 'CLOSE' button in the 'about' dialog.
		gtk_widget_destroy(GTK_WIDGET(dialog));
	}
}
