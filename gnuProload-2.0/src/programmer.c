/*
  Internal file which implements the programmer proper. This file will have 
  functions to burn the hex file into the micro and also erase/verify the micro.
*/

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif
#include <errno.h>
#include <termios.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/signal.h>
#include <stdlib.h>
#include <string.h>
#include <gtk/gtk.h>
#include <glib/gprintf.h>
#include <string.h>
#include <libgen.h>
#include <stdarg.h>

#include "callbacks.h"
#include "interface.h"
#include "support.h"


/* This is where our logo is defined as raw C-source */
extern const guint8 my_pixbuf[];

gboolean is_verify = FALSE;			 
			 
#define FILE_LOADED  "File loaded into internal buffer...\n\n"
#define MICRO_ERASED "Microcontroller erased...\n\n"
#define BURNING_MICRO "Burning microcontroller with file \'"
#define DONE_PROGRAMMING "Programming Completed, proceeding with Verification...\n\n"
#define VERIFYING_MICRO "Verifying..."
#define VERIFICATION_SUCCESSFUL "SUCCESSFUL!!\n\n\n"
#define VERIFICATION_FAILED "FAILED!!\n\n\n"
#define CONNECTION_TIMED_OUT "Serial port connection timed out on"
#define OVERWRITING_PREV_BUFF "\nOverwriting previous buffer...\n"
#define FLASH_SIZE "FLash size is: "
#define HW_CONNECTED "Is your hardware connected *and* powered on??\n\n"

#define BAUDRATE B57600
#define PROGRAMMER_DEV "/dev/ttyS0"
#define MICROCONTROLLER_ROM_SIZE 8192

#define INIT_CMD  0x0D
#define QUERY_ID_CMD 0x67
#define ERASE_CMD 0x65
#define GET_SUMMARY 0x70
#define WRITE_SIZE 0x73
#define COMMENCE_PROGRAM 0x77
#define READ_PROGRAM 0x72
  
//#define SIMULATE_BURNING   //Uncomment this if you want to simulate burning
//#define DEBUG              //Uncomment this if you don't want extra dumps
#ifdef DEBUG
#define DUMP_ROM  
#define DUMP_CMDS
#define DUMP_RESPONSE
#endif /* DEBUG */

/****************************** Global Data **********************************/
int serial_fd = -1;
/*****************************************************************************/

/********************************* Static data *******************************/
static unsigned char *ROM_SPACE = NULL; 
static unsigned int flash_size = 0;
static struct termios serial_old, serial_new;
/*****************************************************************************/

/******************************** Non static functions ***********************/
void burn_hex_file(gpointer filename);
void erase_micro();
void verify_hex_file();
int load_internal_buffer_with_hex_file(const char *filename);
int set_serial();
void reset_serial();
void cleanup();
int check_connection();
void popup_error(const gchar *mesg);
gpointer burn_thread(gpointer filename);
void deactivate_buttons();
void reactivate_buttons();
/*****************************************************************************/


/* To enable access to the global */
extern GtkWidget *gmain_window_ptr;

/********************************* static functions **************************/
//static void update_text_view(const char *mesg);
static void append_text_view(const char *mesg);
static void send_cmd(unsigned char cmd);
static void get_response(unsigned char wait_char);
static void update_progressbar(unsigned int byte_count, const gchar *text);
/*****************************************************************************/


/****************************** Function Definitions *************************/

int load_internal_buffer_with_hex_file(const char *filename)
{
	unsigned int addr, hex_val, max_addr = 0, max_count = 0;
    char inp_line[80]={0}, str[5]={0};
    int count, type, i=0;
    FILE *fp = NULL;
		
	/* Open hex file for reading */
	if((fp=fopen(filename,"r"))==NULL)
    {
		/* This code would NEVER get executed */
        /* code to pop error */
		popup_error("File does not exist!");
		return FALSE;
    }
	else
	{
		/* Check if opened file is a valid hex file. The way to check this is to
           make sure each line has a colon ':' at its beginning and also		   
		 */
		while(  (fgets(inp_line,80,fp)))
		{
			if(inp_line[0] != ':')
			{
				LOG_WARNING(("File not a valid intel hex file!!"));
				/* code to pop up error */
				popup_error("File not in Intel(r) HEX format!\n");
				return FALSE;
			}
		}
		/* Need to rewind the file pointer so that buffer can be created */
		rewind(fp);
	}

    /* Allocate memory for the ROM space, but first check if, already some other
       file was loaded. If yes, just free it.
	 */
	if(ROM_SPACE != NULL)
	{
		append_text_view(OVERWRITING_PREV_BUFF);
		g_free(ROM_SPACE);
		ROM_SPACE = NULL;
	}
    ROM_SPACE = g_malloc(MICROCONTROLLER_ROM_SIZE * sizeof(unsigned char));
    if(ROM_SPACE == NULL)
    {
        LOG_ERROR(("Unable to allocate memory, aborting..."));
        /* without any memory available, it does not make any sense to continue running */
        gtk_main_quit();
    }
	/* memset() with all FFs */ 
    memset(ROM_SPACE,0xFF,MICROCONTROLLER_ROM_SIZE*sizeof(unsigned char));
    max_addr = 0x00;
	
	while(  (fgets(inp_line,80,fp)))
    {
        sscanf(inp_line,":%2X%4X%2X",&count,&addr,&type);
        /* getting the maximum value of the address and its associated count */
        if((addr >= max_addr) && (count > 0x00))
        {
            max_addr = addr;
            max_count = count;
        }
        for(i=0;i<=2*(count-1);i+=2)
        {
            str[0]=inp_line[9+i];
            str[1]=inp_line[9+i+1];
            str[2]='\0';
            sscanf(str,"%2X",&hex_val);

            ROM_SPACE[addr]=hex_val;
            addr++;
        }
    }
    flash_size = max_addr+max_count;
    append_text_view(FILE_LOADED);	
#ifdef DUMP_ROM
    LOG_INFO(("*********Dumping rom data***********"));
    for(i=0;i<MICROCONTROLLER_ROM_SIZE;i++)
    {
        LOG_INFO_DATA(("%X ",ROM_SPACE[i]));
    }
#endif
	fclose(fp);
	return TRUE;
}

void burn_hex_file(gpointer filename)
{
    unsigned int thou = 0,hund = 0,tens = 0,units = 0;
    unsigned int f_size = flash_size, i = 0;
	char *full_path = NULL, *bname = NULL;
	gchar byte_info[13]; //choose size carefully. Otherwise it would screw up 
	                     //the stack
	/* First check if file was loaded into internal buffer */
	if(ROM_SPACE == NULL)
	{
		popup_error("File not entered!");
		//gtk_main_
	}
	else
	{
	append_text_view(BURNING_MICRO);
	if((full_path = strdup(filename)))
	{
		bname = basename(full_path);
	    append_text_view(bname);
		append_text_view("\'");
		free(full_path);
	}
	append_text_view("\n");
    send_cmd(WRITE_SIZE);
    get_response(0x11);

    thou = f_size/1000;
    f_size %= 1000;
    send_cmd(thou+0x30);
    get_response(0x11);

    hund = f_size/100;
    f_size %= 100;
    send_cmd(hund+0x30);
    get_response(0x11);

    tens = f_size/10;
    f_size %= 10;
    send_cmd(tens+0x30);
    get_response(0x11);

    units = f_size;
    send_cmd(units+0x30);
    send_cmd(0x0a); 
    get_response('>');
    g_sprintf(byte_info,"%d%d%d%d bytes.\n\n",thou,hund,tens,units);
	append_text_view(FLASH_SIZE);
    append_text_view(byte_info);
    send_cmd(COMMENCE_PROGRAM);    
    get_response(0x11);

    for(i=0;i<flash_size;i++)
    {
#ifdef DUMP_ROM
        printf("Sending Byte no: %d : ",i);
#endif
        send_cmd(ROM_SPACE[i]);
		update_progressbar(i, NULL);
        if(i<flash_size - 1)
        {
            /* we should NOT wait for ack for the last byte */
            get_response(0x11);
        }
    }
    append_text_view(DONE_PROGRAMMING);
  }
}

static void send_cmd(unsigned char cmd)
{
#ifdef DUMP_CMDS    
    LOG_INFO_DATA(("Writing data: %X", cmd));
#endif
#ifndef SIMULATION	
    write(serial_fd,&cmd,1);
#endif
}
 
static void get_response(unsigned char wait_char)
{
#ifndef SIMULATE_BURNING	
    unsigned char resp = 0x00;
    while(read(serial_fd,&resp,1))
    {
        /* substitute '.' for all unprintable characters, else print it as is. */
#ifdef DUMP_RESPONSE        
        if(resp < ' ')
        {
            printf(".");
        }
        else
        {
            printf("%c",resp);
        } 
#endif        
        if(resp == wait_char)
        {
            break;
        }
    }
#else
	return;
#endif	
}

void erase_micro()
{
	send_cmd(INIT_CMD);
    get_response('>');

    send_cmd(QUERY_ID_CMD);
    get_response('>');

    send_cmd(ERASE_CMD);
    get_response('>');

    send_cmd(GET_SUMMARY);
    get_response('>');
    append_text_view(MICRO_ERASED);
}

void verify_hex_file()
{
    unsigned char *VERIFY_BUFF = NULL;
#ifndef SIMULATE_BURNING	
	unsigned char resp[2] = {0};
#endif	
    unsigned int thou = 0,hund = 0,tens = 0,units = 0;
    unsigned int f_size = flash_size, i = 0, j = 0;
    long int data_byte = 0;

    send_cmd(WRITE_SIZE);
    get_response(0x11);
    append_text_view(VERIFYING_MICRO);
    fflush(stdout);

    thou = f_size/1000;
    f_size %= 1000;
    send_cmd(thou+0x30);
    get_response(0x11);

    hund = f_size/100;
    f_size %= 100;
    send_cmd(hund+0x30);
    get_response(0x11);

    tens = f_size/10;
    f_size %= 10;
    send_cmd(tens+0x30);
    get_response(0x11);

    units = f_size;
    send_cmd(units+0x30);
    send_cmd(0x0a); 
    get_response('>');

    send_cmd(READ_PROGRAM);
    /* Allocate memory for the ROM space*/
    VERIFY_BUFF = g_malloc(MICROCONTROLLER_ROM_SIZE * sizeof(unsigned char));
    if(VERIFY_BUFF == NULL)
    {
		LOG_ERROR(("Unable to allocate memory, aborting..."));
        exit(1);
    }
    /* memset() with all FFs */
    memset(VERIFY_BUFF,0xFF,MICROCONTROLLER_ROM_SIZE*sizeof(unsigned char));
#ifndef SIMULATE_BURNING	
    read(serial_fd,&resp,1);   /* reading the initial 'r' */
#endif
    for(i=0,j=0;j<flash_size;j++)
    {
#ifndef SIMULATE_BURNING		
        read(serial_fd,&resp,2);
        data_byte = strtol((const char *)resp,(char **)NULL,16);
#endif	
        update_progressbar(i, NULL);		
        VERIFY_BUFF[i++] = data_byte;
#ifdef DUMP_CMDS
		LOG_INFO_DATA(("Verifying byte no: %d %X", i, (unsigned int)data_byte));
#endif		
    } 
    get_response('>');
    if(!memcmp(ROM_SPACE,VERIFY_BUFF,MICROCONTROLLER_ROM_SIZE*sizeof(unsigned char)))
    {
		append_text_view(VERIFICATION_SUCCESSFUL);
    }
    else
    {
		append_text_view(VERIFICATION_FAILED);
    }
    send_cmd(GET_SUMMARY);
    get_response('>');
	g_free(VERIFY_BUFF);
}

int set_serial()
{
#ifdef SIMULATE_BURNING
	return TRUE;
#endif	
	int retval = TRUE;
	serial_fd = open(PROGRAMMER_DEV, O_RDWR | O_NOCTTY); 
    if (serial_fd < 0)
    {
		append_text_view(strerror(errno));
		append_text_view("\n");
		retval = FALSE;
		/* insert code to pop up error */
    }	
	
	if(retval == TRUE)
	{
	/* get the current serial and stdin settings for restoring later */
    /* also get a working copy for each of them */
    tcgetattr(serial_fd, &serial_old );
    tcgetattr(serial_fd, &serial_new );   /* working copy */
	  
	serial_new.c_cflag = BAUDRATE | CS8 | CLOCAL | CREAD;
    serial_new.c_iflag = IGNPAR;
    serial_new.c_oflag = 0;
    serial_new.c_lflag = 0;
    serial_new.c_cc[VMIN]=1;
    serial_new.c_cc[VTIME]=0;
    tcflush(serial_fd, TCIFLUSH);
    tcsetattr(serial_fd,TCSANOW,&serial_new);
	}
	
	return retval;
}

void reset_serial()
{
	LOG_INFO(("resetting serial line..."));
#ifndef SIMULATE_BURNING	
	tcsetattr(serial_fd, TCSANOW, &serial_old );   /* restoring back */
	close(serial_fd);
#endif
}


/*static void update_text_view(const char *mesg)
{
	GtkTextView *textview_widget = NULL;
	GtkTextBuffer *buffer = NULL;
	textview_widget = GTK_TEXT_VIEW(lookup_widget(gmain_window_ptr, "textview1"));
	buffer = gtk_text_view_get_buffer(textview_widget);
	gtk_text_buffer_set_text(buffer, (gchar*)mesg, -1);
	gtk_text_view_set_buffer(GTK_TEXT_VIEW(textview_widget),buffer);
}
*/

static void append_text_view(const char *mesg)
{
	GtkTextView *textview_widget = NULL;
	GtkTextBuffer *buffer = NULL;
	GtkTextIter iter;
	GtkTextMark *mark;
	textview_widget = GTK_TEXT_VIEW(lookup_widget(gmain_window_ptr, "textview1"));
	buffer = gtk_text_view_get_buffer(textview_widget);
	gtk_text_buffer_get_end_iter(buffer,&iter);
	gtk_text_buffer_insert(buffer,&iter,mesg,-1);
	gtk_text_view_set_buffer(GTK_TEXT_VIEW(textview_widget),buffer);
	gtk_text_buffer_get_end_iter(buffer,&iter);
	mark =  gtk_text_buffer_create_mark(buffer, "end_mark", &iter, FALSE);
	/* Now place scroll the viewer so that the last text is visible */
	gtk_text_view_scroll_to_mark(textview_widget, mark, 0.0, FALSE, 1.0, 1.0); 
	while (gtk_events_pending ())
	{
	    gtk_main_iteration ();
	}
}

void popup_error(const gchar *mesg)
{
	GtkWidget *popup = create_popup_dialog();
	GtkLabel *label = NULL;
	GdkPixbuf *pixbuf = NULL;	
	label = GTK_LABEL(lookup_widget(popup, "error_label"));
	/* Set icon for About dialog here */
    pixbuf = gdk_pixbuf_new_from_inline (-1, my_pixbuf, FALSE, NULL);	
    gtk_window_set_icon(GTK_WINDOW(popup), pixbuf);
	gtk_label_set_text(label, mesg);
	gtk_widget_show(popup);
}

void cleanup()
{
	LOG_INFO(("Cleaning up..."));
	/* Free up the memory */
	if (ROM_SPACE != NULL)
	{
		g_free(ROM_SPACE);
		ROM_SPACE = NULL;
	}
	/* Reset the serial line */
	reset_serial();
	//close(serial_fd);
}

int check_connection()
{
#ifndef SIMULATE_BURNING	
	int retval = TRUE;
	int maxfd = 0;
	int nret = 0;
	//int fd = -1;   /* Our local temporary file descriptor */
	fd_set inp_fdset;
	unsigned char cmd = INIT_CMD;
	struct timeval tv;
	serial_fd = open(PROGRAMMER_DEV, O_RDWR | O_NOCTTY | O_NONBLOCK); 
	if (serial_fd < 0)
    {
		append_text_view(strerror(errno));
		append_text_view("\n");
		retval = FALSE;
		/* insert code to pop up error */
    }
    else
	{
	    /* get the current serial and stdin settings for restoring later */
        /* also get a working copy for each of them */
        tcgetattr(serial_fd, &serial_old );
        tcgetattr(serial_fd, &serial_new );   /* working copy */
	    serial_new.c_cflag = BAUDRATE | CS8 | CLOCAL | CREAD;
        serial_new.c_iflag = IGNPAR;
        serial_new.c_oflag = 0;
        serial_new.c_lflag = 0;
        serial_new.c_cc[VMIN]=1;
        serial_new.c_cc[VTIME]=0;
        tcflush(serial_fd, TCIFLUSH);
        tcsetattr(serial_fd,TCSANOW,&serial_new);		
		
		FD_ZERO(&inp_fdset);
		FD_SET(serial_fd, &inp_fdset);
		maxfd = serial_fd + 1;   /* We are opening only file, so fd + 1 */
		tv.tv_sec = 1;
		tv.tv_usec = 0;
		/* Write some data to the port, say the INIT command so as to evoke
		   a response from it.
		*/
		write(serial_fd, &cmd, 1);
		if((nret = select(maxfd, &inp_fdset, NULL, NULL, &tv)) == 0  )
		{
	        append_text_view(CONNECTION_TIMED_OUT);
			append_text_view(PROGRAMMER_DEV);
			append_text_view("\n");
			append_text_view(HW_CONNECTED);
			LOG_WARNING(("Connection timed out...make sure programmer board is \
                            connected AND powered on!"));
			retval = FALSE;
		}
		else if(nret < 0)
 		{
			append_text_view(strerror(errno));
			retval = FALSE;
		}
		else if(FD_ISSET(serial_fd,&inp_fdset))
		{
			LOG_INFO(("select() call was successful"));
			/* Now we must read the response, otherwise we would hang forever */
			get_response('>');
			LOG_INFO(("Got Response after select()!!"));
		}
		else
		{
			append_text_view("Unforseen Error, contact developer at \
seemanta@gmail.com");
		}
		/* Now close the serial port descriptor */
		tcsetattr(serial_fd, TCSANOW, &serial_old );   /* restoring back */
		close(serial_fd);
	}		
	return retval;
#else
	return TRUE;
#endif	
}


 
gpointer burn_thread(gpointer filename)
{
	//GtkProgressBar *progress_bar = NULL;
	if((load_internal_buffer_with_hex_file(filename)) && \
	 check_connection() == TRUE)
    {
		if(set_serial() == TRUE)
		{ 
		    is_verify = FALSE;
			//deactivate_buttons();
			//progress_bar = GTK_PROGRESS_BAR(lookup_widget(gmain_window_ptr, "main_progressbar"));
			/* First clear the progress bar of any previous update. -1 will
			 * set progress to 0.
			 */
			update_progressbar(-1, "Burning...");
			erase_micro();
			///progress_bar = GTK_PROGRESS_BAR(lookup_widget(gmain_window_ptr, "main_progressbar"));
			//gtk_progress_bar_set_text(progress_bar, "Burning...");
			burn_hex_file(filename);
			///////pbar = GTK_PROGRESS_BAR(lookup_widget(gmain_window_ptr, "main_progressbar"));
			update_progressbar(-1, "Verifying...");
			////progress_bar = GTK_PROGRESS_BAR(lookup_widget(gmain_window_ptr, "main_progressbar"));
    		//gtk_progress_bar_set_text(pbar, "Verifying...");
			verify_hex_file();
			update_progressbar(-1, "");
			///////gtk_progress_bar_set_text(pbar, "");
			reset_serial();
			//reactivate_buttons();
		
		}
    }
	return NULL;
}

void update_progressbar(unsigned int byte_count, const gchar *text)
{
	GtkProgressBar *pbar = NULL;
	gdouble fraction = 0.0;
	pbar = GTK_PROGRESS_BAR(lookup_widget(gmain_window_ptr, "main_progressbar"));
	if(text)
	{
		gtk_progress_bar_set_text(pbar, text);
		LOG_INFO_DATA(("Updating progress bar with text: %s", text));
	}
	/* Calculate the fractional value */
	fraction = ((byte_count+1)/(gdouble)flash_size);
	gtk_progress_bar_set_fraction(pbar, fraction);
	while (gtk_events_pending ())
	{
	    gtk_main_iteration ();
	}
}


void deactivate_buttons()
{
    GtkWidget *button = NULL;
    GtkWidget *menuitem = NULL;
	
	button = lookup_widget(gmain_window_ptr, "load_button");
	menuitem = lookup_widget(gmain_window_ptr, "load_menu_item");
    gtk_widget_set_sensitive(button, FALSE);
	gtk_widget_set_sensitive(menuitem, FALSE);
	/* Keep the below code intact for the moment, albeit commented*/
/*
    button = lookup_widget(gmain_window_ptr, "burn_button");
	menuitem = lookup_widget(gmain_window_ptr, "burn_menu_item");
    gtk_widget_set_sensitive(button, FALSE);
	gtk_widget_set_sensitive(menuitem, FALSE);

	button = lookup_widget(gmain_window_ptr, "erase_button");
	menuitem = lookup_widget(gmain_window_ptr, "erase_menu_item");
    gtk_widget_set_sensitive(button, FALSE);
	gtk_widget_set_sensitive(menuitem, FALSE);

	button = lookup_widget(gmain_window_ptr, "quit_button");
	menuitem = lookup_widget(gmain_window_ptr, "quit_menu_item");
    gtk_widget_set_sensitive(button, FALSE);
	gtk_widget_set_sensitive(menuitem, FALSE);
	*/
}

void reactivate_buttons()
{
    GtkWidget *button = NULL;
    GtkWidget *menuitem = NULL;
	
	button = lookup_widget(gmain_window_ptr, "load_button");
	menuitem = lookup_widget(gmain_window_ptr, "load_menu_item");
    gtk_widget_set_sensitive(button, TRUE);
	gtk_widget_set_sensitive(menuitem, TRUE);
	/* Keep the below code intact for the moment, albeit commented*/
/*  
    button = lookup_widget(gmain_window_ptr, "burn_button");
	menuitem = lookup_widget(gmain_window_ptr, "burn_menu_item");
    gtk_widget_set_sensitive(button, TRUE);
	gtk_widget_set_sensitive(menuitem, TRUE);

	button = lookup_widget(gmain_window_ptr, "erase_button");
	menuitem = lookup_widget(gmain_window_ptr, "erase_menu_item");
    gtk_widget_set_sensitive(button, TRUE);
	gtk_widget_set_sensitive(menuitem, TRUE);

	button = lookup_widget(gmain_window_ptr, "quit_button");
	menuitem = lookup_widget(gmain_window_ptr, "quit_menu_item");
    gtk_widget_set_sensitive(button, TRUE);
	gtk_widget_set_sensitive(menuitem, TRUE);
*/
}
