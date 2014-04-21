#include <gtk/gtk.h>
#include <stdio.h>
#include <gdk-pixbuf/gdk-pixbuf.h>
#include <stdlib.h>
#include <glib.h>
#include <gio/gio.h>
#include <string.h>

GtkWindow *w = NULL;
GtkImage *i = NULL;
GList *jpgList = NULL;
GList* currentFile;
const char *trash_path = NULL; 

void set_image(const char* filename) {
	if (i != 0) {
		//gtk_container_remove(w, i);
		gtk_widget_destroy(i);
		i = NULL;
	}

	gtk_window_set_title(w, filename);

	int ww, wh;
	gtk_window_get_size(w, &ww, &wh);

	GError *ge = NULL;
	GdkPixbuf *pb = gdk_pixbuf_new_from_file(filename, &ge);
	if (pb ==  NULL) {
		printf("error: %s\n", ge->message);
		return;
	}
	
	int iw = gdk_pixbuf_get_width(pb);
	int ih = gdk_pixbuf_get_height(pb);
	float ar = (float)iw / ih;

	int nw, nh;

	if (iw > ih) {
		nw = ww;
		nh = (int)(nw / ar);
	} else {
		nh = wh;
		nw = (int)(nh * ar);
	}

	GdkPixbuf *pb2 = gdk_pixbuf_scale_simple(pb, nw, nh, GDK_INTERP_BILINEAR);
	g_object_unref(pb);

	i = gtk_image_new_from_pixbuf(pb2);
	g_object_unref(pb2);

	gtk_container_add(w, i);
	gtk_widget_show(i);

}

int compare_func(const GFileInfo* a, const GFileInfo* b) {
	const char *name_a, *name_b;
	
	name_a = g_file_info_get_name(a);
	name_b = g_file_info_get_name(b);

	return strcmp(name_a, name_b);	
}

void scan_source_dir(const char* source_dir) {
	GFile *src = g_file_new_for_path(source_dir);
	GError *err = NULL;

	chdir(source_dir);
	
	GFileEnumerator *files = g_file_enumerate_children(src, G_FILE_ATTRIBUTE_STANDARD_NAME, G_FILE_QUERY_INFO_NOFOLLOW_SYMLINKS, NULL, &err);
	if (files == NULL) {
		printf("error: %s", err->message);
		g_object_unref(err);
		return;
	}

	GFileInfo* fi;

	while ((fi = g_file_enumerator_next_file(files, NULL, NULL)) != NULL) {
		const char *filename = g_file_info_get_name(fi);

		if(g_str_has_suffix(filename, ".jpg"))
			jpgList = g_list_prepend(jpgList, fi);
	}

	GCompareFunc gcf = compare_func;
	jpgList = g_list_sort(jpgList, gcf);	

	/*GList *e = g_list_first(jpgList);	
	for(e = g_list_first(jpgList); e; e = g_list_next(e)) {
		fi = (GFileInfo*)e->data;
		printf("%s\n", g_file_info_get_name(fi));
	}*/

	currentFile = g_list_first(jpgList);
	set_image(g_file_info_get_name((GFileInfo*)currentFile->data));

	g_object_unref(src);
}

void check_and_set_current_file(GList *t) {
	if (t != NULL) {
		currentFile = t;
		set_image(g_file_info_get_name((GFileInfo*)currentFile->data));
	}
}

void next_image() {
	GList *t = g_list_next(currentFile);
	check_and_set_current_file(t);
}

void prev_image() {
	GList *t = g_list_previous(currentFile);
	check_and_set_current_file(t);
}

void trash_image() {
	char *cmd = (char*)malloc(sizeof(char)*1000);
	cmd[0] = 0;

	strcat(cmd, "mv ");
	strcat(cmd, g_file_info_get_name((GFileInfo*)currentFile->data));
	strcat(cmd, " ");
	strcat(cmd, trash_path);

	//printf("%s\n", cmd);
	system(cmd);
}

gboolean on_key_press_event(GtkWindow *wi, GdkEventKey *ev, gpointer data) {
	const char* key = gdk_keyval_name(ev->keyval);

	if (strcmp(key, "Left") == 0)
		prev_image();
	else if (strcmp(key, "Right") == 0)
		next_image();
	else if (strcmp(key, "d") == 0 || strcmp(key, "D") == 0)
		trash_image();

	return FALSE;
}

int main(int argc, char* argv[]) {

	if (argc < 3) {
		printf("usage: command source_dir trash_dir\n");
		return 0;
	}	

	gtk_init(NULL, NULL);	

	w = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_default_size(w, 800, 600);

	g_signal_connect(w, "delete-event", G_CALLBACK(gtk_main_quit), NULL);
	g_signal_connect(w, "key-press-event", G_CALLBACK(on_key_press_event), NULL);

	gtk_widget_show(w);

	trash_path = argv[2];	
	scan_source_dir(argv[1]);
	
	gtk_main();

	return 0;
}
