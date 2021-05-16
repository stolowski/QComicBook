SET(DATA_FILES bestfit.png 
	     bookmark.png 
	     fitheight.png 
	     fitwidth.png 
	     fforward.png 
	     fbackward.png 
	     info.png 
	     japanese.png 
             lens.png
	     nextpage.png 
	     openarch.png 
	     opendir.png 
	     orgsize.png 
	     pagebottom.png 
	     pagetop.png 
	     prevpage.png 
	     rright.png 
	     rleft.png 
	     qcomicbook.png 
	     settings.png 
	     thumbnails.png 
	     twopages.png 
	     wholepage.png 
	     qcomicbook-splash.png)

#INSTALL(FILES ${DATA_FILES} DESTINATION share/${PACKAGE})
INSTALL(FILES qcomicbook.png DESTINATION share/icons/hicolor/48x48/apps)
INSTALL(FILES qcomicbook.desktop DESTINATION share/applications)

