import org.gnome.gdk.Event;
import org.gnome.gtk.Button;
import org.gnome.gtk.Gtk;
import org.gnome.gtk.Label;
import org.gnome.gtk.VBox;
import org.gnome.gtk.Widget;
import org.gnome.gtk.Window;
import org.gnome.gtk.Image;
import org.gnome.gdk.Pixbuf;
import org.gnome.gdk.InterpType;
import org.gnome.gdk.EventKey;
import org.gnome.gdk.Keyval;

import java.io.FileNotFoundException;
import java.io.File;
import java.io.FilenameFilter;
import java.util.Arrays;
import java.lang.ProcessBuilder;
import java.io.IOException;


class MainWindow {
	private String sourcePath;
	private String trashPath;
	private String[] jpgList;
	private Window w;
	private Image i;
	private int currentFile;


	public MainWindow(String sp, String tp) {
		trashPath = tp;
		sourcePath = sp;

		String[] gtk_args = null;
		Gtk.init(gtk_args);

		w = new Window();
		w.setDefaultSize(800, 600);

		w.showAll();

		scanSourcePath();

		class MainWindowKPE implements Window.KeyPressEvent {
			private MainWindow d;

			public MainWindowKPE(MainWindow dd) {
				d = dd;
			}

			public boolean onKeyPressEvent(Widget w, EventKey e){
				Keyval key = e.getKeyval();
								
				switch(key.toString()) {
					case "Keyval.Right":
						d.nextImage();
						break;
					case "Keyval.Left":
						d.prevImage();
						break;
					case "Keyval.d":
					case "Keyval.D":
						d.trashImage();
						break;
				}
				return false;
			}
		}		

		w.connect(new MainWindowKPE(this));
    
		w.connect(new Window.DeleteEvent() {
		    public boolean onDeleteEvent(Widget source, Event event) {
			Gtk.mainQuit();
			return false;
		    }
		});
      
		Gtk.main();
	}


	private void setImage(String filename) {
		if (i != null) {
			w.remove(i);
			i = null;
		}

		w.setTitle(filename);

		Pixbuf pb = null;
		try {
			pb = new Pixbuf(sourcePath + "/" + filename);
		}
		catch (FileNotFoundException e) {
			System.out.println(e.toString());
			return;
		}

		int iw = pb.getWidth();
		int ih = pb.getHeight();
		float ar = (float)iw / ih;


		int ww = w.getWidth();
		int wh = w.getHeight();

		int nw, nh;

		if (iw > ih) {
			nw = ww;
			nh = (int)(nw / ar);
		} else {
			nh = wh;
			nw = (int)(nh * ar);
		}

		Pixbuf pb2 = pb.scale(nw, nh, InterpType.BILINEAR);
		
		i = new Image(pb2);
		w.add(i);
		w.showAll();
	}


	private void trashImage() {
		ProcessBuilder pb = new ProcessBuilder("mv", sourcePath + "/" + jpgList[currentFile], trashPath);
		try {
			pb.start();
		} catch (IOException e) {
		}
	}


	private void nextImage() {
		if (currentFile < jpgList.length - 1)
			currentFile++;
		setImage(jpgList[currentFile]);
	}


	private void prevImage() {
		if (currentFile > 0)
			currentFile--;
		setImage(jpgList[currentFile]);
	}


	private void scanSourcePath() {
		File sourceFolder = new File(sourcePath);

		jpgList = sourceFolder.list(new FilenameFilter() {
			public boolean accept(File dir, String filename) {
				if (filename.endsWith(".jpg"))
					return true;
				return false;
			}
		});

		Arrays.sort(jpgList);
		for (String jpg : jpgList) {
			System.out.println(jpg);
		}
		currentFile = 0;
		setImage(jpgList[0]);
	}


	public void openTrash() {
		ProcessBuilder pb = new ProcessBuilder("gnome-open", trashPath);
		try {
			pb.start();
		} catch (IOException e) {
		}
	}
}


public class ImageNot
{
    public static void main(String[] args) {
		int argn = args.length;

		if (argn < 2) {
			System.out.println("arguments: source_dirpath trash_dirpath");
			System.exit(1);
		}
		
		MainWindow d = new MainWindow(args[0], args[1]);
		d.openTrash();		
    }
}

