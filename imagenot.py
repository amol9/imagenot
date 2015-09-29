#!/usr/bin/python

from gi.repository import Gtk
from gi.repository.GdkPixbuf import Pixbuf, InterpType
from gi.repository import Gdk
import sys
import os
import glob
import shutil

class MyWindow(Gtk.Window):

	def __init__(self):
		Gtk.Window.__init__(self, title="Hello World")
		Gtk.Window.set_default_size(self, 800, 600)

		self.connect("key_press_event", self.on_key_pressed)
		self.image = Gtk.Image()
		

	def on_key_pressed(self, event, data):
		(b, k) = data.get_keyval()
		key = Gdk.keyval_name(k)

		if key == "Right":
			self.next_image()
		elif key == "Left":
			self.prev_image()
		elif key == "d" or key == "D":
			self.delete_image()
		

	def set_image(self, filename):
		Gtk.Window.set_title(self, filename)
		self.remove(self.image)

		pb = Pixbuf.new_from_file(filename)

		iw = Pixbuf.get_width(pb)
		ih = Pixbuf.get_height(pb)
		ar = (float)(iw) / ih
		
		(w, h) = Gtk.Window.get_size(self)
		if (iw > ih):
			nw = w
			nh = (int)(nw / ar)
		else:
			nh = h
			nw = (int)(nh * ar)

		pb2 = Pixbuf.scale_simple(pb, nw, nh, InterpType.BILINEAR)

		self.image.set_from_pixbuf(pb2)
		self.add(self.image)


	def next_image(self):
		if (self.currentFile < len(self.jpgList) - 1):
			self.currentFile += 1
		self.set_image(self.jpgList[self.currentFile])


	def prev_image(self):
		if (self.currentFile > 0):
			self.currentFile -= 1
		self.set_image(self.jpgList[self.currentFile])


	def delete_image(self):
		print "trashing ", self.jpgList[self.currentFile], " to ", trashPath
		shutil.move(self.jpgList[self.currentFile], trashPath)

	
	def set_sourcePath(self, sourcePath):
		os.chdir(sourcePath)
		self.jpgList = sorted(glob.glob('*.jpg'))
		self.currentFile = 0
		self.set_image(self.jpgList[0])

	def set_trashPath(self, trashPath):
		self.trashPath = trashPath


#== main program ===============>

argc = len(sys.argv)
if (argc < 3):
	print "usage: command source_folder trash_folder"
	quit()

trashPath = sys.argv[2]

win = MyWindow()
win.set_sourcePath(sys.argv[1])
win.set_trashPath(trashPath)
win.connect("delete-event", Gtk.main_quit)
win.show_all()
Gtk.main()

os.system("gnome-open " + trashPath)
