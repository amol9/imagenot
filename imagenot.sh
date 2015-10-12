#! /bin/bash

gtk()
{ 
printf '%s\n' "$*" >&"${COPROC[1]}" 
read -ru "$COPROC" 
}
 
coproc gtk-server -stdin

gtk gtk_init NULL NULL 

gtk gtk_window_new GTK_WINDOW_TOPLEVEL
window=$REPLY 

gtk gtk_window_set_default_size "$window" 800 600

#gtk gtk_button_new_with_label hello 
#button=$REPLY

gtk gtk_image_new_from_file "fwp.jpg"
image=$REPLY

gtk gtk_container_add "$window" "$image" 
gtk gtk_widget_show_all "$window" 

until [[ $event = $window ]];do
  gtk gtk_server_callback wait
  event=$REPLY
done

gtk gtk_server_exit
wait "$COPROC_PID"
