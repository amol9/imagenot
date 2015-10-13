#include <gtk/gtk.h>
#include <stdio.h>
#include <gdk-pixbuf/gdk-pixbuf.h>
#include <stdlib.h>
#include <glib.h>
#include <gio/gio.h>
#include <string.h>
#include <sys/stat.h>


const char* extensions[] = {".jpg", ".bmp", ".png", ".jpeg", ".gif"};

GtkWindow *w 		= NULL;
GtkImage *i 		= NULL;
GList *imageList 	= NULL;
GList* currentFile	= NULL;
const char *trash_path 	= NULL; 
const char *source_path = NULL;


void set_image(const char* filename) {
	if (i != 0) {
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

const char* join_path(const char* path1, const char* path2) {
	char last_char = path1[strlen(path1) - 1];
	char* fullpath = (char*) malloc(strlen(path1) + strlen(path2) + last_char == '/' ? 1 : 2);

	const char* fmt_string = last_char == '/' ? "%s%s" : "%s/%s";
	sprintf(fullpath, fmt_string, path1, path2);
	return fullpath;
}

gboolean is_image_filename(filename) {
	int i;
	for (i = 0; i < (sizeof(extensions) / sizeof(char*)); i++) {
		if (g_str_has_suffix(filename, extensions[i])) {
		       return 1;
		}
	}
	return 0;	
}

void scan_source_dir() {
	GFile *src = g_file_new_for_path(source_path);
	GError *err = NULL;

	GFileEnumerator *files = g_file_enumerate_children(src, G_FILE_ATTRIBUTE_STANDARD_NAME, G_FILE_QUERY_INFO_NOFOLLOW_SYMLINKS, NULL, &err);
	if (files == NULL) {
		printf("error: %s", err->message);
		g_object_unref(err);
		return;
	}

	GFileInfo* fi;

	while ((fi = g_file_enumerator_next_file(files, NULL, NULL)) != NULL) {
		const char *filename = g_file_info_get_name(fi);

		if(is_image_filename(filename))
			imageList = g_list_prepend(imageList, fi);
	}

	GCompareFunc gcf = compare_func;
	imageList = g_list_sort(imageList, gcf);	

	currentFile = g_list_first(imageList);
	const char* currentFilePath = join_path(source_path, g_file_info_get_name((GFileInfo*)currentFile->data));
	set_image(currentFilePath);
	free(currentFilePath);

	g_object_unref(src);
}

void check_and_set_current_file(GList *t) {
	if (t != NULL) {
		currentFile = t;
		const char* currentFilePath = join_path(source_path, g_file_info_get_name((GFileInfo*)currentFile->data));
		set_image(currentFilePath);

		free(currentFilePath);
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
	const char* currentFilePath = join_path(source_path, g_file_info_get_name((GFileInfo*)currentFile->data));
	char *cmd = (char*)malloc(strlen(currentFilePath) + strlen(trash_path) + 10);
	sprintf(cmd, "mv %s %s", currentFilePath, trash_path);

	system(cmd);

	free(currentFilePath);
	free(cmd);
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

gboolean is_dir(const char* path) {
	struct stat path_stat;
	if (stat(path, &path_stat) == -1) {
		return 0;
	}
	return S_ISDIR(path_stat.st_mode);
}

int main(int argc, char* argv[]) {

	if (argc < 3) {
		printf("arguments: source_dirpath trash_dirpath\n");
		return 0;
	}	
	
	source_path = argv[1];
	trash_path = argv[2];

	if (! is_dir(source_path)) {
		printf("%s is not a directory\n", source_path);
		return -1;
	}

	if (! is_dir(trash_path)) {
		printf("%s is not a directory\n", trash_path);
		return -1;
	}

	gtk_init(NULL, NULL);	

	w = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_default_size(w, 800, 600);

	g_signal_connect(w, "delete-event", G_CALLBACK(gtk_main_quit), NULL);
	g_signal_connect(w, "key-press-event", G_CALLBACK(on_key_press_event), NULL);

	gtk_widget_show(w);

	scan_source_dir();
	
	gtk_main();

	return 0;
}

