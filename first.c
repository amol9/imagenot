#include <gtk/gtk.h>
#include <stdio.h>
#include <gdk-pixbuf/gdk-pixbuf.h>
#include <stdlib.h>
#include <glib.h>
#include <gio/gio.h>

GtkWindow *w = NULL;
GtkImage *i = NULL;
GList *jpgList = NULL;

void set_image(const char* filename) {
	if (i != 0)
		gtk_container_remove(w, i);

	int ww, wh;
	gtk_window_get_size(w, &ww, &wh);

	GError *ge = NULL;
	GdkPixbuf *pb = gdk_pixbuf_new_from_file(filename, &ge);
	if (pb ==  NULL) {
		printf("error: %s", ge->message);
		return;
	}
	
	int iw = gdk_pixbuf_get_width(pb);
	int ih = gdk_pixbuf_get_height(pb);
	float ar = iw / ih;

	printf("ar = %f", ar);

	int nw, nh;

	if (iw > ih) {
		nw = ww;
		nh = (int)(nw / ar);
	} else {
		nh = wh;
		nw = (int)(nh * ar);
	}

	GdkPixbuf *pb2 = gdk_pixbuf_scale_simple(pb, nw, nh, GDK_INTERP_BILINEAR);

	i = gtk_image_new_from_pixbuf(pb2);
	//i = gtk_image_new_from_file("fwp.jpg");
	gtk_container_add(w, i);
	gtk_widget_show(i);

}

void scan_source_dir(const char* source_dir) {
	GFile *src = g_file_new_for_path(source_dir);
	GError *err = NULL;
	
	GFileEnumerator *files = g_file_enumerate_children(src, G_FILE_ATTRIBUTE_STANDARD_NAME, G_FILE_QUERY_INFO_NOFOLLOW_SYMLINKS, NULL, &err);
	if (files == NULL) {
		printf("error: %s", err->message);
		return;
	}

	GFileInfo* fi;
	while ((fi = g_file_enumerator_next_file(files, NULL, NULL)) != NULL) {
		jpgList = g_list_prepend(jpgList, fi);
		printf("%s", g_file_info_get_name(fi));
		//char r; scanf("%c", &r);	
	}
	
	fi = (GFileInfo*)g_list_first(jpgList);
	printf("%s", g_file_info_get_name(fi));
	/*while((fi = g_list_next(jpgList)) != NULL) {
		printf("%s", g_file_info_get_name(fi));
	}*/
}

int main(int argc, char* argv[]) {

	if (argc < 3) {
		printf("usage: command source_dir trash_dir\n");
		return 0;
	}	

	gtk_init(NULL, NULL);	

	w = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	g_signal_connect(w, "delete-event", G_CALLBACK(gtk_main_quit), NULL);

	gtk_widget_show(w);
	
	scan_source_dir(argv[1]);
	
	gtk_main();

	return 0;
}
